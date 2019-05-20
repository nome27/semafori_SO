#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
	int sem_id= running->syscall_args[0];  //id del semaforo
	int count= running-> syscall_args[1];    //contatore del semaforo	
	
	//Semaphore è la struttura dei semafori
	
	//le funzioni dei semafori sono in disastrOS_semaphore.c
	
	//devo verificare il il semaforo è già nella lista dei semafori
	Semaphore* sem_aux=SemaphoreList_byId(&semaphores_list, sem_id);
	
	if(sem_aux=NULL){ //se il semaforo non esiste
		Semaphore* sem= Semaphore_alloc(sem_id, count);  //alloco il semaforo passando id e contatore
		List_insert(&semaphores_list, semaphores_list.last, sem); //e inserisco il semaforo alla fine della lista dei semafori attivi
	}
	
	//alloco il descrittore del semaforo
	SemDescriptor* sem_descr= SemDescriptor_alloc(running-> last_sem_fd,sem,running);//prende in ingresso un intero, il semaforo, e il nome del pcb
	 
	running->last_sem_fd++;   //incrememnto l'ultimo numero sem_fd
	//e il puntatore al descrittore(precedentemente allocato) del semaforo (id)
	SemDescriptorPtr* sem_descr_ptr=SemDescriptorPtr_alloc(sem_descr);
	
	//inserisco il puntatore al descrittore(sem_descr_ptr) alla lista dei puntatori ai descrittori dei semafori
	List_insert(running->sem_descriptors, sem_descriptors.last, sem_descr_ptr);
	
	running-> syscall_retvalue=sem_descr-> fd;
   
    return;
}
	
	
	
