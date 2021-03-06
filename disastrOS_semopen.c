#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_constants.h"

void internal_semOpen(){
	int sem_id= running->syscall_args[0];  //id del semaforo
	int count= running-> syscall_args[1];    //contatore del semaforo	
	
	//Semaphore è la struttura dei semafori
	//le funzioni dei semafori sono in disastrOS_semaphore.c
    
	//se count <0 non posso allocare il semaforo
    if(count < 0 ){
        printf("  Allocazione semaforo con id=%d fallita \n", sem_id);
        running->syscall_retvalue =  DSOS_ERSEMOPEN;
        return;
   }

   //devo verificare il il semaforo è già nella lista dei semafori
   //controllo che non esista gia un semaforo con questo id
   Semaphore* sem=SemaphoreList_byId(&semaphores_list, sem_id);

	if(sem==0){ //se il semaforo non esiste
	    printf("creo il semaforo %d\n", sem_id);
		sem= Semaphore_alloc(sem_id, count);  //alloco il semaforo passando id e contatore se non esiste
		if(sem==0){ //controllo se il semaforo è stato allocato correttamente
            printf("    Allocazione semaforo con id=%d fallita\n", sem_id);
            running->syscall_retvalue = DSOS_ERSEMOPEN;
            return;
        }
		printf("allocazione del semaforo con id=%d riuscita \n", sem_id);
		List_insert(&semaphores_list, semaphores_list.last, (ListItem*)sem); //e inserisco il semaforo alla fine della lista dei semafori attivi
	}
	
	
	//alloco il descrittore del semaforo
	SemDescriptor* sem_descr= SemDescriptor_alloc(running-> last_sem_fd,sem,running);//prende in ingresso un intero, il semaforo, e il nome del pcb
	
	//controllo: se il sem_descr==0 errore
	if(sem_descr==0){
		running->syscall_retvalue = DSOS_ERSEMOPEN ;
        return;
	}
	
	(running->last_sem_fd)++;   //incrememnto l'ultimo numero sem_fd (num sem aperti)
	
	 List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) sem_descr);  //lo inserisco nella lista dei descrittori
	
	//e il puntatore al descrittore(precedentemente allocato) del semaforo (id)
	SemDescriptorPtr* sem_descr_ptr=SemDescriptorPtr_alloc(sem_descr);
	sem_descr->ptr=sem_descr_ptr; 
	
	//inserisco il puntatore al descrittore(sem_descr_ptr) alla lista dei puntatori ai descrittori dei semafori
	List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) sem_descr_ptr);
	
	running-> syscall_retvalue=sem_descr-> fd; //il valore di ritorno della syscall è il file descr del semaforo
    return;
}

