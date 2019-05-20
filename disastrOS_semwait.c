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

  return;
}

