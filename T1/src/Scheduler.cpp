#include "../include/Scheduler.h"

using namespace std;
// implementar operador <

//Alta prioridade:	Processos novos, processos que voltam do I/O (fita,impressora)
//Baixa prioridade:	Processos que retornam do Disco, processos que sofreram preempcao.
//IO_queue:			Processo que estao aguardando a serem atendidos pelo IO 
queue< Process > High_queue,Low_queue,IO_queue;
queue< Process > finished;
pthread_mutex_t l0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t l1 = PTHREAD_MUTEX_INITIALIZER;
int PID;

//Matar o escalonador, quando produzir todas as threads.
void* Schedulling(void* args){

	while(true){
		if(finished.size() == MAX_P) break;



		pthread_mutex_lock(&l0);
		Process a;
		if( !High_queue.empty() ){
			puts("Retirou da fila High_queue");
			a = High_queue.front();
			//puts("DESEMPILHOU");
			High_queue.pop();
		}
		else if(!IO_queue.empty()){
			puts("Retirou da fila IO_queue");
			a = IO_queue.front();
			IO_queue.pop();
		}
		else if(!Low_queue.empty()){
			puts("Retirou da fila LOW_queue");
			a = Low_queue.front();
			Low_queue.pop();
		}
		else {
			pthread_mutex_unlock(&l0);
			continue;
		}
		pthread_mutex_unlock(&l0);

		//Processo eh novo
		if(a.status == NOVO || a.status == PRONTO){
			printf("(NOVO OU PRONTO): Escalonando o processo id:%d\n", a.pid );
			printf("Tempo de chegada: %d\n",a.tempo_chegada);
				// n pede IO nessa fatia de tempo
			if( !(a.tempo_chegada <= a.tempo_io and a.tempo_chegada+time_slice >= a.tempo_io) ){
				//processo sofre preempcao
				if(a.tempo_chegada + time_slice < a.tempo_servico ){
					// processo volta pra fila.
					a.tempo_chegada += time_slice;
					a.status = PREEMPTADO;
					printf("Processo sofreu preempcao\n");
					Low_queue.push( a  );
				}
				//processo finaliza
				else{
					printf("Processo finalizou %d\n",a.pid);
					a.status = FINISHED;
					finished.push( a );
				}
			}
				// solicita IO
			else{
					//ta feio
				while(a.tempo_chegada < a.tempo_io) a.tempo_chegada++;
				a.status = IO;
				printf("Processo solicita IO\n");
				IO_queue.push( a );
			}
			sleep(2);	
		}
			//Processo pede um IO .
		else if(a.status == IO){
			printf("(VOLTA do IO): Escalonando o processo id:%d\n", a.pid );			
			printf("Tempo de chegada: %d\n",a.tempo_chegada);
			if( a.tipo_io == DISCO ){
				printf("DISCO\n");
				pthread_mutex_lock(&l0);
				a.status = PRONTO;
				Low_queue.push(a);
				pthread_mutex_unlock(&l0);
			}
			else{
				printf("(FITA | IMPRESSORA)\n" );
				pthread_mutex_lock(&l0);
				a.status = PRONTO;
				High_queue.push(a);
				pthread_mutex_unlock(&l0);	
			}
			sleep(2);
		}
			// Processo que volta de uma preempcao.
		else{
			printf("(PREEMPTADO): Escalonando o processo id:%d\n", a.pid );
			printf("Tempo de chegada: %d\n",a.tempo_chegada);
			if( a.tempo_chegada + time_slice < a.tempo_io ){
				//processo sofre preempcao
				if(a.tempo_chegada + time_slice < a.tempo_servico ){
						// processo volta pra fila.
					printf("PROCESSO SOFRE PREEMPCAO\n");
					a.tempo_chegada += time_slice;
					a.status = PREEMPTADO;
					Low_queue.push( a  );
				}
					//processo finaliza
				else{
					printf("PROCESSO FINALIZA\n");
					a.status = FINISHED;
					finished.push( a );
				}
			}			// solicita IO
			else{
				printf("PROCESSO SOLICITA IO\n");
				printf("Tempo de chegada: %d\n",a.tempo_chegada);
					//ta feio
				while(a.tempo_chegada < a.tempo_io) a.tempo_chegada++;
				a.status = IO;
				IO_queue.push( a );
			}
			sleep(2);
		}		
	}
}


void* CreateProcess(void *args)
{	


	Process a;

	pthread_mutex_lock(&l1);
	a.pid = PID++;
	pthread_mutex_unlock(&l1);
	a.status = NOVO;
	a.tempo_servico = rand() % 5 ;
	a.tempo_io = (rand()%2)*( rand()%(a.tempo_servico) );
	a.tipo_io = rand()%5;

	printf("CRIANDO PROCESSO pid:%d\n",a.pid);
	printf("Tempo de servico: %d\n",a.tempo_servico);
	printf("Tempo_IO: %d\n", a.tempo_io);
	printf("Tipo IO: %d\n", a.tipo_io);
	pthread_mutex_lock(&l0);
	High_queue.push(a);
	puts("EMPILHOU");
	//printf("%d\n",High_queue.size());
	pthread_mutex_unlock(&l0);
}







int main(){
	void* retval;
	pthread_t Create_P;
	pthread_t Scheduler;
	pthread_t Monitor;

	if( pthread_create(&Scheduler, NULL, Schedulling, NULL ) ){
		puts("ERRO NA CRIACAO DA THREAD Escalonador");
		exit(0);
	}

	for(int i = 0;i<MAX_P;i++){
		if( pthread_create(&Create_P, NULL, CreateProcess, NULL ) ){
			puts("ERRO NA CRIACAO DA THREAD Processo");
			exit(0);
		}
		sleep(rand()%3);
	}
	pthread_join(Create_P, &retval);
	pthread_join(Scheduler, &retval);



/*
	if( pthread_create(Scheduler, NULL, Monitoring, NULL ) ){
		puts("ERRO NA CRIACAO DA THREAD Monitor");
		exit(0);
	}
*/
	
	//pthread.join(Monitor, &retval);
	return 0;
}