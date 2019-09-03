#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"
#include "disastrOS_constants.h"

#define CYCLES 10
#define BUFFER_LENGTH 50

int filled_sem, empty_sem, read_sem, write_sem;
int buf[BUFFER_LENGTH];
int write_index=0;
int read_index=0;
int shared_variable;

//nel produttore(producer) faccio la wait in modo che mentre uno scrive, non può scrivere nessun altro
//la stessa cosa per la scrittura

int producer(){
  disastrOS_semWait(empty_sem);
  disastrOS_semWait(write_sem);

  //inizio sezione critica
  int ret= shared_variable;
  buf[write_index]= shared_variable;
  write_index= (write_index+1)% BUFFER_LENGTH;
  shared_variable++;
  //fine sezione critica
  
  disastrOS_semPost(write_sem);
  disastrOS_semPost(filled_sem); 

 // return shared_variable;
 return ret;
}

int consumer(){   //effettuo la lettura nel consumer
  int x;

  disastrOS_semWait(filled_sem);
  disastrOS_semWait(read_sem);
 
  //inizio sezione critica lettura
  x= buf[read_index];
  read_index=(read_index+1)% BUFFER_LENGTH;
  //fine sezione critica

  disastrOS_semPost(read_sem);
  disastrOS_semPost(empty_sem);

  return x;
}

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  //quando apro un figlio faccio partire il semaforo (quindi la open)
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  //int type=0;
  //int mode=0;

  //int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  //printf("fd=%d\n", fd);
  //printf("PID: %d, terminating\n", disastrOS_getpid());
  
  printf("apertura semafori\n");

  filled_sem = disastrOS_semOpen(1, 0);  //0 è il numero di posti del buffer occupati all'inizio
  empty_sem = disastrOS_semOpen(2, BUFFER_LENGTH);  //buffer_length indica il numero di posti liberi all'inizio
  read_sem = disastrOS_semOpen(3, 1);  //semaforo per la lettura
  write_sem = disastrOS_semOpen(4,1);  //semaforo per la scrittura

  for (int i=0; i<CYCLES; ++i){
   if(disastrOS_getpid()%2==0){ // se il pid  è pari parte la producer(scrittura nel buffer), altrimenti la consumer
     int prod=producer();
     printf("thread %d: ho scritto nel buffer il valore %d\n", disastrOS_getpid(),prod);
   }
   else{
     int ret= consumer();
     printf("thread %d: ho letto dal buffer il valore %d\n", disastrOS_getpid(),ret);
   }
  }

  disastrOS_printStatus();
  printf("sta terminando pid=%d\n",disastrOS_getpid());

  printf("chiusura semafori\n");


  disastrOS_semClose(filled_sem);
  disastrOS_semClose(empty_sem);
  disastrOS_semClose(read_sem);
  disastrOS_semClose(write_sem);

  disastrOS_exit(disastrOS_getpid());
}



void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
 // disastrOS_spawn(sleeperFunction, 0);

  //int fd[10]; 
  printf(" stato iniziale buffer\n");
  for(int i=0; i < BUFFER_LENGTH; i++){
    printf("%d ", buf[i]);
  }
   
   shared_variable=1;   //variabile globale settata a 1

  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;        //processi figlio vivi
  for (int i=0; i<10; ++i) {   //creo 10 thread
    disastrOS_spawn(childFunction, 0);
    printf("ho creato il %dth thread\n", i+1);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }

  printf(" stato finale buffer\n");
  for(int i=0; i < BUFFER_LENGTH; i++){
    printf("%d ", buf[i]);
  }


  printf("\n");
  printf("shutdown!");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}



