#include <iostream>
#include <queue>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cctype>

using namespace std;

struct Task {
  unsigned c, p, d, a; 
  unsigned id;
  bool isPeriodic; // periódicas = true, aperiódicas = false
  int remainingTime;
  char diagram; 
  int it; 
  int ct = 0;
};

void read( int &t, int &tp, int &ta, queue<Task>& tasks ) {
  int trash;
  //cout << "Insira o conjunto de teste:" << endl;
  cin >> t >> tp >> ta;
  cin >> trash >> trash >> trash;

  char letter = 'A'; 

  // Lê as tarefas periódicas
  for ( int i = 0; i < tp; i++ ) {
    Task task;
    cin >> task.c >> task.p >> task.d;
    task.isPeriodic = true;
    task.id = i;
    task.a = 0;
    task.remainingTime = task.c;
    task.diagram = letter++; 
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
    task.diagram = letter++;  
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
  sort( periodicTasks.begin(), periodicTasks.end(),
    []( const Task & a, const Task & b ) {
        if ( a.diagram == b.diagram ) return a.remainingTime < b.remainingTime;
        if ( a.p == b.p ) return a.diagram < b.diagram;
        return a.p < b.p; 
      }
  );

  // Ordena as tarefas aperiódicas por tempo de chegada (crescente)
  sort( aperiodicTasks.begin(), aperiodicTasks.end(),
    []( const Task & a, const Task & b ) {
        if ( a.a == b.a ) return a.diagram < b.diagram;
        return a.a < b.a; 
      }
    );

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
         << ", Diagrama: " << task.diagram 
         << ", Periódica: " << ( task.isPeriodic ? "Sim" : "Não" )
         << endl;
    tasks.push( task );
  }
}

void BS_Execution( int &t, queue<Task>& tasks ) {
  vector<char> ganttDiagram( t + 1 ); 
  vector<int> executionTimes( 26, 0 ); // Vetor de tempos de execução para cada letra ('A' a 'Z')
  vector<int> waitTime( 26, 0 );
  vector<int> totalTime( 26, 0 );
  vector<int> id;
  char see_again = '<';
  int id_atual = tasks.size();
  int num_tasks = tasks.size();
  int currentTime = 0;
  int preempcoes = 0;
  int context_change = 0;

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
        newTask.ct = currentTime;
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
    if(currentTask.diagram != see_again && see_again!= '<') preempcoes++;
    ganttDiagram[currentTime] = currentTask.diagram;
    currentTask.remainingTime -= 1;
    int letterIndex = toupper( currentTask.diagram ) - 'A'; // Converte a letra para índice ('A' = 0)
    executionTimes[letterIndex]++;
    
    if (currentTask.isPeriodic && currentTime >= (currentTask.ct + currentTask.d) ) 
      ganttDiagram[currentTime] = tolower( currentTask.diagram );

    for(int i = 0; i < tasks.size(); i++){
        Task taks_a = tasks.front();
        tasks.pop();
        int letterIndex = toupper( taks_a.diagram ) - 'A'; // Converte a letra para índice ('A' = 0)
        waitTime[letterIndex]++ ;
        waitTime[0] = 0;
        tasks.push(taks_a);
    }

    if ( currentTask.remainingTime > 0 ){
        tasks.push( currentTask );
        see_again = currentTask.diagram;
    } else{
      see_again = '<';
    }

    currentTime++;
  }

  //cout << "Diagrama de Gantt: ";
  for ( int i = 0; i < ganttDiagram.size(); i++ ) {
    cout << ganttDiagram[i] << ' ';
  }
  cout << endl;

  for( int i = 1 ; i < id.size(); i++ ) {
    if( id[i - 1] != id[i] ) context_change++;
  }
  
  for ( int i = 1; i < ganttDiagram.size(); i++ ) {
    if(ganttDiagram[i-1] != '.' && ganttDiagram[i] == '.') preempcoes++;
  }
  
  cout << context_change + 1 << " " << preempcoes << endl;
  //cout << "trocas de contexto : " << context_change +1 << endl;
  //cout << "premmpções: " << preempcoes << endl;
  //cout << "tempos total de exec e de espera:" << endl;
  for( int i = 0; i < num_tasks; i++ ) {
      cout << waitTime[i] + executionTimes[i] << " " << waitTime[i]  << endl;
  }
}

int main() {
  int t, tp, ta;

  while ( true ) {
    queue<Task> tasks;
    read( t, tp, ta, tasks );

    if ( t == 0 && tp == 0 && ta == 0 ) break;

    BS_Execution( t, tasks );
  
    cout << endl;
  }
}
