#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int sem_id = running->syscall_args[0]; //prendo l'id sem_id del semaforo
  
  SemDescriptor* sem_descr = SemDescriptorList_byFd(&running->sem_descriptors, sem_id); // prendo il descrittore del semaforo tramite sem_id
  if (!sem_descr) {  //controllo che esista
    running->syscall_retvalue =DSOS_ERRDESCR;  //errore
    return;
  }

  Semaphore* sem= sem_descr->semaphore;      //prendo il semaforo sem dal sem_descriptor(lo salvo)
  
  if (!sem) {//se il semaforo non è presente resistuisco un errore
    running->syscall_retvalue =DSOS_ERRNOTOPENED;  
    return ;
  }
  
  sem->count--;   //decremento il contatore del semaforo


  if (sem->count < 0){
    SemDescriptorPtr* sem_descr_ptr= sem_descr->ptr;  //salvo il puntatore del descrittore
    List_detach(&sem->descriptors, (ListItem*)sem_descr_ptr); //rimuovo il descrittore del processo dalla lista dei descrittori(sem->descriptors)

    List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*) sem_descr->ptr); //lo inserisco in fondo alla lista di waiting
    running->status = Waiting; //stato del processo 
    List_insert(&waiting_list, waiting_list.last, (ListItem*) running); //inserisco il processo in esecuzione nella waiting_list
        
    PCB* pcb = (PCB*) List_detach(&ready_list, (ListItem*)ready_list.first); //prendo il primo elemento dalla lista dei ready
    running = (PCB*)pcb; //metto in esecuzione il primo elemento della lista di ready (quello appena preso)

    }

  running->syscall_retvalue = 0;
  return;
}

