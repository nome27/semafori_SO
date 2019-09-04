#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"


void internal_semPost(){
    int sem_id = running->syscall_args[0];  //primo parametro id del semaforo
    
    SemDescriptor* sem_descr = SemDescriptorList_byFd(&running->sem_descriptors, sem_id); //prendo descrittore del semaforo con quell'id
    
    //nel caso in cui la lista dei descrittori sia vuota 
    if(sem_descr==0){
        printf("sempost del semaforo %d fallita \n", sem_id);
        //running->syscall_retvalue =  DSOS_ERRDESCR;
        running->syscall_retvalue =  DSOS_ERSEMPOST;
        return;
    }
    
    Semaphore* sem = sem_descr->semaphore; //prendo il semaforo
    sem->count++; //incremento contatore di s
    
    if(sem->count <= 0){ //se il contatore è <=0, inserisco il processo in running nella lista dei ready
        List_insert(&ready_list, ready_list.last, (ListItem*) running);
    
        //elimino il primo elemento di waiting_descriptors dalla lista sem->waiting_descriptors
        
        //rimuovo il first descriptor del semaforo dalla lista dei descrittori in attesa
        SemDescriptorPtr* sem_descr_ptr = (SemDescriptorPtr*) List_detach(&sem->waiting_descriptors, (ListItem*) sem->waiting_descriptors.first); 
        List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) sem_descr_ptr); //lista dei descrittori attivi
        List_detach(&waiting_list, (ListItem*) sem_descr_ptr->descriptor->pcb); //rimuovo il processo dalla waiting_list tramite il suo puntatore al descrittore

        running->status = Ready;
        running = sem_descr_ptr->descriptor->pcb; //metto in esecuzione il processo rimosso dalla waiting_list (pcb del semaforo)
    }

    //se tutta l'operazione va a buon fine
    running->syscall_retvalue = 0;
    return;
}