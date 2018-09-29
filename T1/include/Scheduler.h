#include <iostream>
#include <thread>
#include <queue>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <set>
#define MAX_P 			10
#define TEMPO_MAX_SERVICO 5


#define T_DISCO 		3
#define T_FITA  		2
#define T_IMPRESSORA 	1


#define DISCO 		1
#define FITA  		2
#define IMPRESSORA 	3

#define NOVO 		0
#define IO 			1
#define PREEMPTADO 	2
#define PRONTO		3
#define FINISHED 	4

#define time_slice 1
// estados novo, io, preemptado, 0,1,2 
void *CreateProcess(void **args);

typedef int TIPO_IO;
typedef int TEMPO_IO;
struct Process
{
 //um requisicao de Io por processo?
 	int pid,gpid,status;
 	// tempo e o tempo que o processo demora, tempo_io e o tempo que ele roda no io
 	// tempo_chegada: tempo em que o processo entra para a execucao, e total de fatia de tempo ja executada
 	int tempo_servico,tempo_chegada = 1;
 	// 0 io, 1 fita,2 impressora
 	std::set< std::pair< TEMPO_IO, TIPO_IO > > IO_queue;
};