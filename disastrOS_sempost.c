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

    return ;
  }

}
