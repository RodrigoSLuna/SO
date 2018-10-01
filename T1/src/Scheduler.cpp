#include "../include/Scheduler.h"

using namespace std;
int tempo_execucao = 1;
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

//Thread para imprimir a saida no TXT
pthread_mutex_t CP_OUT = PTHREAD_MUTEX_INITIALIZER;
int PID;

void* IO_Schedulling_DISCO(void* args){
	ofstream D("DISCO.txt", ios_base::app | ios_base::out);
	while(true){
		if(finished.size()== MAX_P ) break;
		Process a;
		
		if(IO_queue[DISCO].size() == 1) 
		pthread_mutex_lock(&io_lock[DISCO]);
		if(!IO_queue[DISCO].empty())
		{
			a = IO_queue[DISCO].front();
			IO_queue[DISCO].pop();
			
			D << "====================================================" << endl << endl;
			D << "PID: " << a.pid << endl;
			printf("Retirou o processo de pid: %d da fila da DISCO\n",a.pid);
		}
		else
		{
			pthread_mutex_unlock(&io_lock[DISCO]);
			continue;
			
		}
		pthread_mutex_unlock(&io_lock[DISCO]);
		printf("Processo de pid: %d solicitou DISCO\n \t DISCO EXECUTANDO \n",a.pid);
		sleep(T_DISCO);
		printf("Processo de pid: %d volta do Disco\n \t RETORNO DO DISCO \n",a.pid);
		pthread_mutex_lock(&l0);
	
		a.status=PRONTO;
		Low_queue.push( a );
	
		pthread_mutex_unlock(&l0);
	}
}

void* IO_Schedulling_IMPRESSORA(void* args){
	ofstream I("IMPRESSORA.txt", ios_base::app | ios_base::out);
	while(true)
	{
		if(finished.size() == MAX_P ) break;
		Process a;
		pthread_mutex_lock(&io_lock[IMPRESSORA]);
		
		if(IO_queue[IMPRESSORA].size() == 1)
		printf("Consegui o Lock: %d\n",IO_queue[IMPRESSORA].size());
		if(!IO_queue[IMPRESSORA].empty())
		{
			a =	IO_queue[IMPRESSORA].front();
			IO_queue[IMPRESSORA].pop();

			I << "====================================================" << endl << endl;

			I << "PID: " << a.pid << endl;
			printf("Retirou o processo de pid: %d da fila da IMPRESSORA\n",a.pid);
			printf("Retirou retirou o processo de pid: %d da fila da IMPRESSORA\n",a.pid);

		}
		else
		{
			pthread_mutex_unlock(&io_lock[IMPRESSORA]);
			continue;
			
		}
		pthread_mutex_unlock(&io_lock[IMPRESSORA]);
		printf("Processo de pid: %d solicitou IMPRESSORA\n \t IMPRESSORA EXECUTANDO \n",a.pid);
		sleep(T_IMPRESSORA);
		printf("Processo de pid: %d volta da IMPRESSORA\n \t RETORNO DA IMPRESSORA \n",a.pid);
		pthread_mutex_lock(&l0);
	
		a.status=PRONTO;
		High_queue.push( a );
	
		pthread_mutex_unlock(&l0);

	}
}
void* IO_Schedulling_FITA(void* args){
	ofstream F("FITA.txt", ios_base::app | ios_base::out);

	while(true)
	{
		if(finished.size()== MAX_P ) break;
		Process a;
		pthread_mutex_lock(&io_lock[FITA]);
		if(!IO_queue[FITA].empty())
		{
			a = IO_queue[FITA].front();
			IO_queue[FITA].pop();
			
			printf("Retirou o processo de pid: %d da fila da FITA\n",a.pid);

			F << "====================================================" << endl << endl;

			F << "PID: " << a.pid << endl;
		}
		else
		{
			pthread_mutex_unlock(&io_lock[FITA]);
			continue;
			
		}
		pthread_mutex_unlock(&io_lock[FITA]);
		printf("Processo de pid: %d solicitou FITA\n \t FITA EXECUTANDO \n",a.pid);
		sleep(T_FITA);
		printf("Processo de pid: %d volta da FITA\n RETORNO DA FITA \n",a.pid);
		pthread_mutex_lock(&l0);

		a.status=PRONTO;
		High_queue.push( a );
		pthread_mutex_unlock(&l0);



	}
}
//Matar o escalonador, quando produzir todas as threads.
void* Schedulling(void* args){

	ofstream Schedulling("Schedulling.txt", ios_base::app | ios_base::out);
	while(true)
	{

		// se todos os processos forem adicionados a fila dos terminados, entao termina a thread
		if(finished.size() == MAX_P) break;

		// secao critica
		pthread_mutex_lock(&l0);
		Process a;
		//se a fila de alta prioridade nao tiver vazia, retira o processo dessa fila e o escalona
		if( !High_queue.empty() )
		{
			a = High_queue.front();
			
			//puts("DESEMPILHOU");
			High_queue.pop();
			
			printf("Retirou o processo de pid %d da fila High_queue\n",a.pid);
			
			Schedulling << "====================================================" << endl;
			Schedulling << "Pop PID " << a.pid <<  " from High Queue: " << " Status: " << a.status << endl;		}
		//Fila de IO errada.
		//se nao, retira o elemento da fila de baixa prioridade, e o escalona
		else if(!Low_queue.empty())
		{
			a = Low_queue.front();
			Low_queue.pop();
			
			printf("Retirou o processo de pid %d  da fila LOW_queue\n",a.pid);


			Schedulling << "====================================================" << endl;
			Schedulling << "Pop PID " << a.pid <<  " from Low Queue: " << " Status: " << a.status << endl;
		}
		else 
		{
	//		tempo_execucao++;
			pthread_mutex_unlock(&l0);
			continue;
		}
		pthread_mutex_unlock(&l0);
		// cria um pair para ve o tipo e o tempo de io do processo
		pair<int,int> a_io = make_pair(0,0);
		//se o processo eh novo ou esta em estado de pronto escalona ele primeiro
		
		printf("\t PID:%d Tempo de Inicio da execucao %d\n",a.pid, tempo_execucao);
		Schedulling << "Inicio da fatia de tempo do processo " << tempo_execucao << endl;
		
		if(a.status == NOVO)
			a.tempo_inicio = tempo_execucao;

		if(a.status == NOVO || a.status == PRONTO)
		{

			printf("(NOVO OU PRONTO): Escalonando o processo de pid: %d\n", a.pid );
			printf("pid: %d Tempo de chegada: %d\n",a.pid,a.tempo_chegada);

				// n pede IO nessa fatia de tempo
			// verifica se os ios acabaram
			if( a.IO_queue.begin() != a.IO_queue.end() )
				a_io = *(a.IO_queue.begin()); // retiro a solicitacao de IO
			// possivel segfault, segunda condition
			//se ainda possui io, e o tempo de chegada do processo e menor ou igual o tempo de io do processo
			// e o tempo de chegada do processo + a fatia de tempo e maior ou igual ao tempo do IO, entao
			// o processo sofre preempcao
			if(  !( !a.IO_queue.empty() and a.tempo_chegada <= a_io.first and a.tempo_chegada+time_slice >= a_io.first ) )
			{
				//se o tempo de chegada mais a fatia de tempo e menor que o tempo de servico entao
				// o processo sofre preempcao

				//Tempo inicial em que o processo começa a execucao
				
				if(a.tempo_chegada + time_slice < a.tempo_servico )
				{

					//adiciono o tempo de chegada mais a fatia de tempo
					
					a.tempo_chegada += time_slice;
					// mudo o status do processo para preemptado
					a.status = PREEMPTADO;
					tempo_execucao  += time_slice;
					printf("\tPID: %d Tempo de Fim da execucao: %d\n",a.pid ,tempo_execucao);


					Schedulling << "PID " << a.pid << " Tempo de Fim da execucao " << tempo_execucao<< endl 
					<< " Processo Sofreu Preempcao "<< endl << endl;

					printf("Processo sofreu preempcao de pid = %d\n",a.pid);
					// processo volta pra fila
					// secao critica
					pthread_mutex_lock(&l0);
					Low_queue.push(  a  );
					pthread_mutex_unlock(&l0);

				}
				//se nao o processo finaliza
				else
				{
					printf("\t \tProcesso finalizou de pid = %d\n",a.pid);

					while(a.tempo_chegada <= a.tempo_servico) {
						a.tempo_chegada++;
						tempo_execucao++;
					}
					// muda o status do processo para terminado	
					printf("\tPID: %d Tempo de Fim da execucao: %d\n",a.pid ,tempo_execucao);
					
					Schedulling << "PID " << a.pid << " Tempo de Fim da execucao " << tempo_execucao<< endl 
					<< " Processo Terminou "<< endl << endl;

					a.tempo_fim = tempo_execucao;
					a.status = FINISHED;
					//SECAO CRITICA
					pthread_mutex_lock(&l0);
					// coloca o processo na fila de terminados 
					finished.push( a );
					pthread_mutex_unlock(&l0);

				}
			}
			// solicita IO
			//se o processo nao sofre preempcao
			// verifica se os ios acabaram
			else
			{
				//ta feio
				if(a.IO_queue.begin() != a.IO_queue.end() )
				{	
					a_io = *(a.IO_queue.begin()); // retiro a solicitacao de IO
					a.IO_queue.erase( a.IO_queue.begin() ); // retiro a solicitacao de IO da queue do processo
				}
				// adiciona  no tempo de chegada do processo quanto ele precisa usar do io que ele tem
				
				while(a.tempo_chegada <= a_io.first ){
					a.tempo_chegada++;
					tempo_execucao++;
				}
				
				printf("\tPID: %d Tempo de Fim da execucao: %d\n",a.pid ,tempo_execucao);


				string tipo_io = a_io.second == 1? " DISCO " : a_io.second == 2? " FITA " : " IMPRESSORA ";
				Schedulling << "PID " << a.pid << " Tempo de Final da fatia de tempo " << tempo_execucao<< endl 
					<< " Processo Requisitou IO " << tipo_io  << endl << endl;

				// muda o status do processo, fazendo com que ele solicite IO
				a.status = IO;
				//mudo o status do processo para requisicao de IO
				printf("Processo solicita IO 1,pid: %d\n",a.pid);
				pthread_mutex_lock(&io_lock[ a_io.second ]);	// destravo o lock para aquela fila de IO solicitada
				printf("TIPO DE IO: %d, pid: %d\n",a_io.second,a.pid);
				// coloca o processo na fila de io dele
				IO_queue[a_io.second].push( a );	
				
				pthread_mutex_unlock(&io_lock[a_io.second ]);	// destravo o lock para aquela fila de IO solicitada
			}
			sleep(2);	
		}
		//Se processo esta em IO ou em estado de preempcao
		else
		{
			printf("(PREEMPTADO): Escalonando o processo pid: %d\n", a.pid );
			printf("Tempo de chegada: %d, pid:%d, io_first %d:  \n",a.tempo_chegada,a.pid,a_io.first);
			
			//se ainda possui io, e o tempo de chegada do processo e menor ou igual o tempo de io do processo
			// e o tempo de chegada do processo + a fatia de tempo e maior ou igual ao tempo do IO, entao
			// o processo sofre preempcao
			if( !( !a.IO_queue.empty() and a.tempo_chegada <= a_io.first and a.tempo_chegada+time_slice >= a_io.first )  )
			{
				//processo sofre preempcao
				if(a.tempo_chegada + time_slice < a.tempo_servico ){
						// processo volta pra fila.


					a.tempo_chegada += time_slice;
					tempo_execucao += time_slice;
					printf("PROCESSO SOFRE PREEMPCAO pid: %d\n",a.pid);
					printf("\tPID: %d Tempo de Fim da execucao: %d\n",a.pid ,tempo_execucao);

					Schedulling << "PID " << a.pid << " Tempo de Final da fatia de tempo " << tempo_execucao<< endl 
					<< " Sofreu Preempcao "<< endl << endl;
					
					a.status = PREEMPTADO;
					pthread_mutex_lock(&l0);
					Low_queue.push( a  );
					pthread_mutex_unlock(&l0);

				}
					//processo finaliza
				else
				{
					while(a.tempo_chegada <= a.tempo_servico) a.tempo_chegada++, tempo_execucao++;
					// muda o status do processo para terminado	
					printf("\tPID: %d Tempo de Fim da execucao: %d\n",a.pid ,tempo_execucao);
					printf("\t \t PROCESSO FINALIZA pid: %d\n",a.pid);
					

					Schedulling << "PID " << a.pid << " Tempo de Final da fatia de tempo " << tempo_execucao<< endl 
					<< " Terminou "<< endl << endl;

					a.tempo_fim = tempo_execucao;
					a.status = FINISHED;
					finished.push( a );
				}

			}			// solicita IO
			else
			{
			//ta feio
				if(a.IO_queue.begin() != a.IO_queue.end() )
				{	
					a_io = *(a.IO_queue.begin()); // retiro a solicitacao de IO
					a.IO_queue.erase( a.IO_queue.begin() ); // retiro a solicitacao de IO da queue do processo
				}
				while(a.tempo_chegada <= a_io.first ){
					a.tempo_chegada++;
					tempo_execucao++;
				}
				
				string tipo_io = a_io.second == 1? " DISCO " : a_io.second == 2? " FITA " : " IMPRESSORA ";
				Schedulling << "PID " << a.pid << " Tempo de Final da fatia de tempo " << tempo_execucao<< endl 
					<< " Processo Requisitou IO " << tipo_io  << endl << endl;


				printf("\tPID: %d Tempo de Fim da execucao: %d\n",a.pid ,tempo_execucao);
				a.status = IO;							//mudo o status do processo para requisicao de IO
				printf("Processo solicita IO 2 pid: %d\n",a.pid);
				pthread_mutex_lock(&io_lock[ a_io.second ]);	// destravo o lock para aquela fila de IO solicitada
				IO_queue[a_io.second].push( a );	
				pthread_mutex_unlock(&io_lock[a_io.second ]);	// destravo o lock para aquela fila de IO solicitada
			}
			sleep(2);
		}		
	}
	Schedulling << "====================================================" << endl;
}

// gera os tempos aleatorios  dos tipos de IO
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
	ofstream CreateProcess("Create_Process.txt", ios_base::app | ios_base::out);
	// cria o processo a
	Process a;

	//regiao critica do processo
	pthread_mutex_lock(&l1);
	a.pid = PID++;
	pthread_mutex_unlock(&l1);
	// seta o status do processo como novo
	a.status = NOVO;
	// escolhe randomicamente o tempo de servico do processo
	a.tempo_servico = (rand() % TEMPO_MAX_SERVICO) + 1 ;
	
	// cria um pair para cada tipo de IO
	pair<int,int> io_DISCO,io_FITA,io_IMPRESSORA;

	//Cria o tempo de disco
	gen(io_DISCO,io_FITA,io_IMPRESSORA, a.tempo_servico);
	// cria um set 
	set< pair < int,int > > nothing;
 
	( rand()%2 == 0 or io_DISCO.first ==0) ? nothing.insert(io_DISCO) : a.IO_queue.insert( io_DISCO  );
	( rand()%2 == 0 or io_FITA.first ==	0) ? nothing.insert(io_FITA) : a.IO_queue.insert( io_FITA  );
	( rand()%2 == 0 or io_IMPRESSORA.first ==0	)? nothing.insert(io_IMPRESSORA) : a.IO_queue.insert( io_IMPRESSORA  );

	// imprime o pid do processo criado
	printf("CRIANDO PROCESSO pid: %d\n",a.pid);
	// imprime o tempo de servico do processo
	printf("Processo de pid: %d : Tempo de servico: %d\n",a.pid,a.tempo_servico);
	

	pthread_mutex_lock(&CP_OUT);
	
	CreateProcess << "====================================================" << endl;

	CreateProcess << "Criando processo ID: " << a.pid << endl;
	CreateProcess << "Tempo de servico: " 	<< a.tempo_servico << endl;
	// se na hora de escolher o io aleatorio 
	if(a.IO_queue.empty())
		printf("SEM IO\n"), CreateProcess << "SEM IO\n\n";
	// imprime o tempo do io e o tipo do io

	for(auto v :a.IO_queue){
		printf("tempo:%d, IO:%d\n",v.first,v.second);
		CreateProcess << "Tempo: " << v.first << " IO: " << v.second << endl;
	}

	CreateProcess << "====================================================" << endl;
	CreateProcess << endl << endl;
	


	pthread_mutex_unlock(&CP_OUT);

	pthread_mutex_lock(&l0);

	// coloca o processo novo na fila alta de prioridade
	High_queue.push(a);
	//printf("EMPILHOU o processo de pid: %d\n",a.pid);
	pthread_mutex_unlock(&l0);
}

bool cmp(const Process &a, const Process &b){
	if(a.pid < b.pid) return true;
	return false;


}

int main(){
	std::srand(std::time(0));
	void* retval;
	pthread_t Create_P;
	pthread_t Scheduler;
	pthread_t IO_Scheduler_FITA;
	pthread_t IO_Scheduler_IMPRESSORA;
	pthread_t IO_Scheduler_DISCO;

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
		sleep(rand()%10);
	}
	pthread_join(Create_P, &retval);
	pthread_join(Scheduler, &retval);
	pthread_join(IO_Scheduler_IMPRESSORA, &retval);
	pthread_join(IO_Scheduler_DISCO, &retval);
	pthread_join(IO_Scheduler_FITA, &retval);

	ofstream FIM("FIM.txt", ios_base::app | ios_base::out);

	
	vector<Process > sorted_by_pid;

	while(!finished.empty()){
		Process a = finished.front();
		finished.pop();
		sorted_by_pid.push_back( a  );	
	}
	sort(sorted_by_pid.begin(), sorted_by_pid.end(), cmp);

	//fatia de tempo aberto no fim
	FIM << "\tPID\tTempo Inicio\tTempo Fim" << endl; 
	for(auto P : sorted_by_pid){
			
		FIM << "\t" << P.pid << "\t"  << P.tempo_inicio << "\t\t " << P.tempo_fim -1 << endl;

	}


	return 0;
}