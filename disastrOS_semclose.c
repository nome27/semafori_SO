#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
  int sem_id= running->syscall_args[0];   //primo parametro id del semaforo

  SemDescriptor* sem_descr=SemDescriptorList(&running->sem_descriptors, sem_id);
  
  //nel caso in cui la lista dei descrittori sia vuota 
  if (!sem_descr) {  
    //DA SISTEMAREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
    running->syscall_retvalue =DSOS_ERRDESCR;  //errore
    return;
  }
  //se invece sem_descr non è vuota,
  //rimnuovo il semaforo dalla lista dei descrittore
  List_detach(&running->sem_descriptors, (ListItem*)sem_descr);
   
  Semaphore* sem= sem_descr->semaphore;   //prendo il semaforo sem
  //se il semaforo non è presente restituisco un errore
  if (!sem) {
    running->syscall_retvalue =DSOS_ERRNOTOPENED;  
    return ;
  }

  //elimino il primo elemento di waiting_descriptors dalla lista sem->waiting_descriptors
  SemDescriptorPtr* sem_descr_ptr= (SemDescriptorPtr*) List_detach(&sem->descriptors, (ListItem*) sem_descr->ptr);
    
  //soccome sto facendo la close, devo lberare memoria
  SemDescriptor_free(sem_descr);
  SemDescriptorPtr_free(sem_descr_ptr);
 
  if(sem->descriptors.size == 0 && sem->waiting_descriptors.size==0){
    List_detach(&semaphores_list, (ListItem*)sem);
    Semaphore_free(sem);
  }
    
  running->syscall_retvalue = 0;
  return;
}

}
