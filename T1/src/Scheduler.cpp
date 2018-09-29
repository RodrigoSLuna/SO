#include "../include/Scheduler.h"

using namespace std;

//Thread, DISCO,IMPRESSORA,FITA

//TODO
// tempo de execucao total do escalonador.


// Tempo dos processo [1,TEMPO_MAX_SERVICO]


//Alta prioridade:	Processos novos, processos que voltam do I/O (fita,impressora)
//Baixa prioridade:	Processos que retornam do Disco, processos que sofreram preempcao.
//IO_queue:			Processo que estao aguardando a serem atendidos pelo IO 

queue< Process > High_queue,Low_queue,IO_queue[4];
queue< Process > finished;
pthread_mutex_t l0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t l1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t io_lock[4] = {PTHREAD_MUTEX_INITIALIZER};
int PID;

int tempo_exec;
typedef pair<int,int> io_Process; // IO do processo 

void* IO_Schedulling_DISCO(void* args){
	while(true){
		if(finished.size()== MAX_P ) break;
		Process a;
		
		if(IO_queue[DISCO].size() == 1) printf("EH UM\n");
		pthread_mutex_lock(&io_lock[DISCO]);
		if(!IO_queue[DISCO].empty())
		{
			printf("Retirou da fila da DISCO\n");
			a = IO_queue[DISCO].front();
			IO_queue[DISCO].pop();
		}
		else
		{
			pthread_mutex_unlock(&io_lock[DISCO]);
			continue;
			
		}
		pthread_mutex_unlock(&io_lock[DISCO]);
		printf("Processo solicitou DISCO\n \t DISCO EXECUTANDO \n");
		sleep(T_DISCO);
		printf("Processo volta do Disco\n \t RETORNO DO DISCO \n");
		pthread_mutex_lock(&l0);
	
		a.status=PRONTO;
		Low_queue.push( a );
	
		pthread_mutex_unlock(&l0);


	}
}

void* IO_Schedulling_IMPRESSORA(void* args){
	while(true){
		if(finished.size() == MAX_P ) break;
		Process a;
		pthread_mutex_lock(&io_lock[IMPRESSORA]);
		if(IO_queue[IMPRESSORA].size() == 1)
		printf("Consegui o Lock: %d\n", IO_queue[IMPRESSORA].size());
		if(!IO_queue[IMPRESSORA].empty())
		{
			printf("Retirou da fila da IMPRESSORA\n");
			a =	IO_queue[IMPRESSORA].front();
			IO_queue[IMPRESSORA].pop();
			printf("Retirou da fila");

		}
		else
		{
			pthread_mutex_unlock(&io_lock[IMPRESSORA]);
			continue;
			
		}
		pthread_mutex_unlock(&io_lock[IMPRESSORA]);
		printf("Processo solicitou IMPRESSORA\n \t IMPRESSORA EXECUTANDO \n");
		sleep(T_IMPRESSORA);
		printf("Processo volta da IMPRESSORA\n \t RETORNO DA IMPRESSORA \n");
		pthread_mutex_lock(&l0);
	
		a.status=PRONTO;
		High_queue.push( a );
	
		pthread_mutex_unlock(&l0);

	}
}
void* IO_Schedulling_FITA(void* args){
	while(true){
		if(finished.size()== MAX_P ) break;
		Process a;
		pthread_mutex_lock(&io_lock[FITA]);
		if(!IO_queue[FITA].empty())
		{
			printf("Retirou da fila da FITA\n");
			a=IO_queue[FITA].front();
			IO_queue[FITA].pop();
		}
		else
		{
			pthread_mutex_unlock(&io_lock[FITA]);
			continue;
			
		}
		
		pthread_mutex_unlock(&io_lock[FITA]);
		printf("Processo solicitou FITA\n \t FITA EXECUTANDO \n");
		sleep(T_FITA);
		printf("Processo volta da FITA\n RETORNO DA FITA \n");
		pthread_mutex_lock(&l0);

		a.status=PRONTO;
		High_queue.push( a );
		pthread_mutex_unlock(&l0);



	}
}
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
		//Fila de IO errada.
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

		pair<int,int> a_io = make_pair(0,0);
		//Processo eh novo
		if(a.status == NOVO || a.status == PRONTO){
			printf("(NOVO OU PRONTO): Escalonando o processo id:%d\n", a.pid );
			printf("pid: %d Tempo de chegada: %d\n",a.pid,a.tempo_chegada);
				// n pede IO nessa fatia de tempo
			if( a.IO_queue.begin() != a.IO_queue.end() )
				a_io = *(a.IO_queue.begin()); // retiro a solicitacao de IO
			// possivel segfault, segunda condition
			if(  !( !a.IO_queue.empty() and a.tempo_chegada <= a_io.first and a.tempo_chegada+time_slice >= a_io.first ) ){
				//processo sofre preempcao
				if(a.tempo_chegada + time_slice < a.tempo_servico ){
					// processo volta pra fila.
					a.tempo_chegada += time_slice;
					a.status = PREEMPTADO;
					printf("Processo sofreu preempcao %d\n",a.pid);
					pthread_mutex_lock(&l0);
					Low_queue.push( a  );
					pthread_mutex_unlock(&l0);

				}
				//processo finaliza
				else{
					printf("\t \tProcesso finalizou %d\n",a.pid);
					a.status = FINISHED;
					pthread_mutex_lock(&l0);
					finished.push( a );
					pthread_mutex_unlock(&l0);

				}
			}
				// solicita IO
			else{
				//ta feio
				if(a.IO_queue.begin() != a.IO_queue.end() ){	
					a_io = *(a.IO_queue.begin()); // retiro a solicitacao de IO
					a.IO_queue.erase( a.IO_queue.begin() ); // retiro a solicitacao de IO da queue do processo
				}

				while(a.tempo_chegada <= a_io.first ) a.tempo_chegada++;
				a.status = IO;							//mudo o status do processo para requisicao de IO
				printf("Processo solicita IO 1, %d\n",a.pid);
				pthread_mutex_lock(&io_lock[ a_io.second ]);	// destravo o lock para aquela fila de IO solicitada
				printf("TIPO DE IO: %d, pid: %d:\n",a_io.second,a.pid);
				
				IO_queue[a_io.second].push( a );	
				
				pthread_mutex_unlock(&io_lock[a_io.second ]);	// destravo o lock para aquela fila de IO solicitada
			}
			sleep(2);	
		}
		//Processo esta em IO ou em estado de preempcao
		else{
			printf("(PREEMPTADO): Escalonando o processo id:%d\n", a.pid );
			printf("Tempo de chegada: %d, pid:%d, io_first %d:  \n",a.tempo_chegada,a.pid,a_io.first);
			
			//logica errada caralho
			if( !( !a.IO_queue.empty() and a.tempo_chegada <= a_io.first and a.tempo_chegada+time_slice >= a_io.first )  ){
				//processo sofre preempcao
				if(a.tempo_chegada + time_slice < a.tempo_servico ){
						// processo volta pra fila.
					printf("PROCESSO SOFRE PREEMPCAO pid: %d\n",a.pid);
					a.tempo_chegada += time_slice;
					a.status = PREEMPTADO;
					pthread_mutex_lock(&l0);
					Low_queue.push( a  );
					pthread_mutex_unlock(&l0);

				}
					//processo finaliza
				else{
					printf("\t \t PROCESSO FINALIZA pid: %d\n",a.pid);
					a.status = FINISHED;
					finished.push( a );
				}

			}			// solicita IO
			else{
			//ta feio
				if(a.IO_queue.begin() != a.IO_queue.end() ){	
					a_io = *(a.IO_queue.begin()); // retiro a solicitacao de IO
					a.IO_queue.erase( a.IO_queue.begin() ); // retiro a solicitacao de IO da queue do processo
				}
				while(a.tempo_chegada <= a_io.first ) a.tempo_chegada++;
				a.status = IO;							//mudo o status do processo para requisicao de IO
				printf("Processo solicita IO 2 pid:%d\n",a.pid);
				pthread_mutex_lock(&io_lock[ a_io.second ]);	// destravo o lock para aquela fila de IO solicitada
				IO_queue[a_io.second].push( a );	
				pthread_mutex_unlock(&io_lock[a_io.second ]);	// destravo o lock para aquela fila de IO solicitada
			}
			sleep(2);
		}		
	}
}


void gen(pair<int,int> &D, pair<int,int>&F, pair<int,int > &I,int time){

	set<int> cnj;
	while( cnj.size() < time and cnj.size() != 3 ){
		//printf("%d %d\n",cnj.size(),time);
		int val = (rand()%time) +1;
		if(val != 0)
			cnj.insert( val );
	}

	//inicializando os valores
	if(!cnj.empty()){
		D.first = *(cnj.begin());
		D.second = DISCO;

		cnj.erase(cnj.begin());
	}
	else{
		D.first = 0;
		D.second = DISCO;
	}


	if(!cnj.empty()){
		F.first = *(cnj.begin());
		F.second = FITA;

		cnj.erase(cnj.begin());
	}
	else{
		F.first = 0;
		F.second = FITA;
	}

	if(!cnj.empty()){
		I.first = *(cnj.begin());
		I.second = IMPRESSORA;

		cnj.erase(cnj.begin());
	}
	else{
		I.first = 0;
		I.second = IMPRESSORA;
	}
}

void* CreateProcess(void *args)
{	

	Process a;
	pthread_mutex_lock(&l1);
	a.pid = PID++;
	pthread_mutex_unlock(&l1);
	a.status = NOVO;
	a.tempo_servico = (rand() % TEMPO_MAX_SERVICO) + 1 ;
	
	
	pair<int,int> io_DISCO,io_FITA,io_IMPRESSORA;

	//Cria o tempo de disco
	gen(io_DISCO,io_FITA,io_IMPRESSORA, a.tempo_servico);
	set<pair<int,int>> nothing;

	( rand()%2 == 0 or io_DISCO.first ==0) ? nothing.insert(io_DISCO) : a.IO_queue.insert( io_DISCO  );
	( rand()%2 == 0 or io_FITA.first ==0) ? nothing.insert(io_FITA) : a.IO_queue.insert( io_FITA  );
	( rand()%2 == 0 or io_IMPRESSORA.first ==0)? nothing.insert(io_IMPRESSORA) : a.IO_queue.insert( io_IMPRESSORA  );


	printf("CRIANDO PROCESSO pid:%d\n",a.pid);
	printf("TEMPO DE SERVICO: %d\n",a.tempo_servico);
	if(a.IO_queue.empty()) printf("SEM IO\n");
	for(auto v:a.IO_queue){
		printf("tempo:%d, IO:%d\n",v.first,v.second);
	}
	pthread_mutex_lock(&l0);
	High_queue.push(a);
	puts("EMPILHOU");
	pthread_mutex_unlock(&l0);
}







int main(){
	void* retval;
	pthread_t Create_P;
	pthread_t Scheduler;
	pthread_t IO_Scheduler_FITA;
	pthread_t IO_Scheduler_IMPRESSORA;
	pthread_t IO_Scheduler_DISCO;
	pthread_t Monitor;

	if( pthread_create(&Scheduler, NULL, Schedulling, NULL ) ){
		puts("ERRO NA CRIACAO DA THREAD Escalonador");
		exit(0);
	}

	if( pthread_create(&IO_Scheduler_FITA, NULL, IO_Schedulling_FITA, NULL ) ){
		puts("ERRO NA CRIACAO DA THREAD Escalonador_FITA");
		exit(0);
	}
	if( pthread_create(&IO_Scheduler_DISCO, NULL, IO_Schedulling_DISCO, NULL ) ){
		puts("ERRO NA CRIACAO DA THREAD Escalonador_DISCO");
		exit(0);
	}

	if( pthread_create(&IO_Scheduler_IMPRESSORA, NULL, IO_Schedulling_IMPRESSORA, NULL ) ){
		puts("ERRO NA CRIACAO DA THREAD Escalonador_IMPRESSORA");
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
	pthread_join(IO_Scheduler_IMPRESSORA, &retval);
	pthread_join(IO_Scheduler_DISCO, &retval);
	pthread_join(IO_Scheduler_FITA, &retval);



/*
	if( pthread_create(Scheduler, NULL, Monitoring, NULL ) ){
		puts("ERRO NA CRIACAO DA THREAD Monitor");
		exit(0);
	}
*/
	
	//pthread.join(Monitor, &retval);
	return 0;
}