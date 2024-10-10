#include <iostream>
#include <queue>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cctype>

using namespace std;

struct Task {
  unsigned c, p, d, a; // tempo de computação, período, deadline, arrival para as aperiódicas
  unsigned ex = 0;     // tempo total executado
  unsigned wt = 0;     // tempo de espera
  unsigned id;
  bool isPeriodic;     // periódicas = true, aperiódicas = false
  int remainingTime;
  char diagram; // letra que representa a tarefa
  int it; // tempo que a tarefa começou a sua execução
};

void read( int &t, int &tp, int &ta, queue<Task>& tasks ) {
  int trash;
  cout << "Insira o conjunto de teste:" << endl;
  cin >> t >> tp >> ta;
  cin >> trash >> trash >> trash;

  char letter = 'A';  // Inicia o contador de letras

  // Lê as tarefas periódicas
  for ( int i = 0; i < tp; i++ ) {
    Task task;
    cin >> task.c >> task.p >> task.d;
    task.isPeriodic = true;
    task.id = i;
    task.a = 0;
    task.remainingTime = task.c;
    task.diagram = letter++;  // Atribui uma letra e incrementa o contador
    tasks.push( task );
  }

  // Lê as tarefas aperiódicas
  for ( int j = tp; j < ( tp + ta ); j++ ) {
    Task task;
    cin >> task.a >> task.c;
    task.isPeriodic = false;
    task.id = j;
    task.p = 0;
    task.d = 0;
    task.remainingTime = task.c;
    task.diagram = letter++;  // Atribui uma letra e incrementa o contador
    tasks.push( task );
  }
}


void order( queue<Task>& tasks ) {
  vector<Task> periodicTasks;
  vector<Task> aperiodicTasks;

  // Transfere tarefas da fila para vetores temporários
  while ( !tasks.empty() ) {
    Task currentTask = tasks.front();
    tasks.pop();

    if ( currentTask.isPeriodic ) periodicTasks.push_back( currentTask );
                            else aperiodicTasks.push_back( currentTask );
  }

  // Ordena as tarefas periódicas por período (crescente)
  sort( periodicTasks.begin(), periodicTasks.end(), []( const Task & a, const Task & b ) {
    return a.p < b.p; // Ordena em ordem crescente de período
  } );

  // Ordena as tarefas aperiódicas por tempo de chegada (crescente)
  sort( aperiodicTasks.begin(), aperiodicTasks.end(), []( const Task & a, const Task & b ) {
    return a.a < b.a; // Ordena em ordem crescente de chegada
  } );

  // Recoloca as tarefas ordenadas na fila
  for ( const auto& task :  periodicTasks ) tasks.push( task );
  for ( const auto& task : aperiodicTasks ) tasks.push( task );
}

void printTasks( queue<Task> &tasks ) {
  cout << "Tarefas:" << tasks.size() << endl;

  for ( int i = 0; i < tasks.size(); i++ ) {
    Task task = tasks.front();
    tasks.pop();

    cout << "ID: " << task.id
         << ", Computação: " << task.c
         << ", Período: " << task.p
         << ", Deadline: " << task.d
         << ", Arrival: " << task.a
         << ", Diagrama: " << task.diagram  // Exibe a letra associada à tarefa
         << ", Periódica: " << ( task.isPeriodic ? "Sim" : "Não" )
         << endl;
    tasks.push( task );
  }
}

void BS_Execution( int &t, queue<Task>& tasks ) {
  vector<char> ganttDiagram( t + 1 ); // Diagrama de Gantt
  vector<int> executionTimes( 26, 0 );   // Vetor de tempos de execução para cada letra ('A' a 'Z')
  vector<int> waitTime( 26, -1 );
  vector<int> totalTime( 26, 0 );
  vector<int> id;
  int id_atual = tasks.size();
  int num_tasks = tasks.size();
  int currentTime = 0;
  int preempcoes = 0;
  int context_change = 0;

  // Vetor para armazenar apenas as tarefas periódicas, nunca serão alteradas aqui,
  // esse serve só para gerar eventos e novas tarefas
  vector<Task> periodicTasks;
  vector<Task> aperiodicTasks;

  while( ! tasks.empty() ) {
    Task currentTask = tasks.front();
    tasks.pop();
    if ( currentTask.isPeriodic ) periodicTasks.push_back( currentTask );
                            else aperiodicTasks.push_back( currentTask );
  }

  int num_periodic = periodicTasks.size();
  while ( currentTime < t ) {

    for( int i = 0; i < periodicTasks.size(); i++ )
      if( currentTime % ( periodicTasks[i].p + periodicTasks[i].a ) == 0 ) {
        // Criando nova instância da tarefa com os mesmos parâmetros
        Task newTask = periodicTasks[i];
        id_atual ++;
        newTask.id = id_atual;
        tasks.push( newTask );
      }

    for( int i = 0; i < aperiodicTasks.size(); i++ )
      if( currentTime == aperiodicTasks[i].a ) {
        // Criando nova instância da tarefa com os mesmos parâmetros
        Task newTask = aperiodicTasks[i];
        tasks.push( newTask );
      }

    order( tasks );

    if ( tasks.empty() ) {
      ganttDiagram[currentTime] = '.';
      id.push_back( -1 );
      currentTime++;
      continue;
    }

    Task currentTask = tasks.front();
    tasks.pop();
    id.push_back( currentTask.id );
    if(currentTask.remainingTime == currentTask.c) currentTask.it = currentTime;
    ganttDiagram[currentTime] = currentTask.diagram;
    currentTask.remainingTime -= 1;
    currentTask.ex ++; // SOMANDO UM NA EXECUÇÃO
    //cout << " somei um na execução da tarefa: " << currentTask.diagram << endl;

    if ( currentTask.isPeriodic && (currentTask.it + currentTask.c) > /*deadline*/) { // verifica tarefas em atraso
      ganttDiagram[currentTime] = tolower( currentTask.diagram );
    }

    /////////////////////////////// SOMANDO UM NO WAIT TIME DE CADA TAREFA
    for(int i =0; i < tasks.size(); i++){
        Task taks_a = tasks.front();
        tasks.pop();
        taks_a.wt ++;
        //cout << "somei um na espera da tarefa: " << taks_a.diagram << endl;
        tasks.push(taks_a);
    }

    if ( currentTask.remainingTime > 0 ) tasks.push( currentTask );
    else{ // se a tarefa acabou ISSO AQUI N TA NO LUGAR CERTO
      cout << "a tarefa : "<< currentTask.diagram << "terminou!" <<endl;
      int letterIndex = toupper( currentTask.diagram ) - 'A'; // Converte a letra para índice ('A' = 0)
      executionTimes[letterIndex] += currentTask.ex;
      waitTime[letterIndex] += currentTask.wt;
      waitTime[0] = 0;
    }
for(int i = 0; i < num_tasks; i++){
  cout << "posição tempo de espera : " << i << " valor: " <<  waitTime[i] << endl;
  cout << "posição tempo execução : " << i << " valor: " <<  executionTimes[i] << endl;
}

    currentTime++;
    cout << "Tempo: " << currentTime << endl;
    //printTasks( tasks );
  }

  cout << "Diagrama de Gantt: ";
  for ( int i = 0; i < ganttDiagram.size(); i++ ) {
    cout << ganttDiagram[i] << ' ';
  }
  cout << endl;

  for ( int i = 0; i < aperiodicTasks.size(); i++ ) {
    waitTime[i + num_periodic] -= aperiodicTasks[i].a;
  }
  //aqui tem q ter o a linha de num preempções e num trocas de contexto
  for( int i = 1 ; i < id.size(); i++ ) {
    if( id[i - 1] != id[i] ) context_change++;
  }

  if( ganttDiagram[t - 1] == '.' ) context_change++;

  cout << "trocas de contexto : " << context_change << endl;
  cout << "tempos total de exec e de espera:" << endl;
  for( int i = 0; i < num_tasks; i++ ) {
    if( i == 0 ) {
      cout << waitTime[i] + executionTimes[i] << " " << waitTime[i] << endl;
    } else {
      cout << waitTime[i] + executionTimes[i] - 1 << " " << waitTime[i] - 1 << endl;
    }
  }
  cout << "Tempos de espera por tarefa (A-Z):" << endl;
  for ( int i = 0; i < 26; i++ ) {
    if ( waitTime[i] != -1 ) {
      char taskLetter = 'A' + i;
      cout << taskLetter << ": " << waitTime[i] << " unidades de tempo" << endl;
    }
  }

  cout << "Tempos de execução por tarefa (A-Z):" << endl;
  for ( int i = 0; i < 26; i++ ) {
    if ( executionTimes[i] > 0 ) {
      char taskLetter = 'A' + i;
      cout << taskLetter << ": " << executionTimes[i] << " unidades de tempo" << endl;
    }
  }
}

int main() {
  int t, tp, ta;

  while ( true ) {
    queue<Task> tasks;
    read( t, tp, ta, tasks );

    if ( t == 0 && tp == 0 && ta == 0 ) break;

    BS_Execution( t, tasks );
    //  printTasks( tasks );

    cout << endl;
  }
}
