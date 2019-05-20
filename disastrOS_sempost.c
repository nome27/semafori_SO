#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  int sem_id= running->syscall_args[0];   //primo parametro id del semaforo

  SemDescriptor* sem_descr=SemDescriptorList(&running->sem_descriptors, sem_id);
  
  //nel caso in cui la lista dei descrittori sia vuota 
  if (!sem_descr) {  
    running->syscall_retvalue =DSOS_ERRDESCR;  //errore
    return;
  }
  //se invece sem_descr non è vuota, 
  //prendo il semaforo sem dal semdescriptor
  Semaphore* sem= sem_descr->semaphore;

  //se il semaforo non è presente restituisco un errore
  if (!sem) {
    running->syscall_retvalue =DSOS_ERRNOTOPENED;  
    return ;
  }

  sem->count++;

  if(sem->count<=0){   //se il contatore è <=0, inserisco il processo in running nella lista dei ready
    List_insert(&ready_list, ready_list.last, (ListItem*) running);
    
    //elimino il primo elemento di waiting_descriptors dalla lista sem->waiting_descriptors
    SemDescriptorPtr* sem_descr_ptr= (SemDescriptorPtr*) List_detach(&sem->waiting_descriptors, (ListItem*) sem->waiting_descriptors.first);
    
    List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) sem_descr_ptr); //lista dei descrittori attivi

    List_detach(&waiting_list, (ListItem*) sem_descr_ptr->descriptor->pcb);

    running->status = Ready;   //status del processo
    running = sem_descr_ptr->descriptor->pcb;  //metto in esecuzione il pcb del semaforo 

  }
  

  //se tutta l'operazione va a buon fine
  running->syscall_retvalue = 0;
  return;

}
