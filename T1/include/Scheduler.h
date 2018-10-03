#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>
#include <queue>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <set>
#define MAX_P 			  1
#define TEMPO_MAX_SERVICO 5


#define T_DISCO 		3
#define T_FITA  		2
#define T_IMPRESSORA 	1


#define DISCO 		1
#define FITA  		2
#define IMPRESSORA 	3

#define NOVO 		0
#define IO 			1	//bloqueado
#define PRONTO		2
#define FINISHED 	3

#define time_slice  1

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
 	//tempo em que o processo inicia a execucao
 	int tempo_inicio,tempo_fim;
 	// 0 io, 1 fita,2 impressora
 	std::set< std::pair< TEMPO_IO, TIPO_IO > > IO_queue;
};