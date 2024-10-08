#include <iostream>
#include <queue>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Task {
  unsigned c, p, d, a; // tempo de computação, período, deadline, arrival para as aperiódicas
  unsigned ex = 0;     // tempo total executado
  unsigned wt = 0;     // tempo de espera
  unsigned id;
  bool isPeriodic;     // periódicas = true, aperiódicas = false
  int remainingTime;
  char diagram; // letra que representa a tarefa
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
    task.id = ( j + 2 ) * 5; // Para ter certeza que não vai dar conflito de id
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

    if ( currentTask.isPeriodic ) {
      periodicTasks.push_back( currentTask );
    } else {
      aperiodicTasks.push_back( currentTask );
    }
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
  for ( const auto& task : periodicTasks ) {
    tasks.push( task );
  }

  for ( const auto& task : aperiodicTasks ) {
    tasks.push( task );
  }
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
  vector<int> executionTimes(26, 0);     // Vetor de tempos de execução para cada letra ('A' a 'Z'
  int currentTime = 0;
  int last_id = -1;
  int preempcoes = 0;
  int context_change = 0;

  vector<Task> periodicTasks; // Vetor para armazenar apenas as tarefas periódicas, nunca serão alteradas aqui, esse serve só para gerar eventos e novas tarefas

  for( int i = 0; i < tasks.size(); i++ ) {
    Task currentTask = tasks.front();
    tasks.pop();
    if ( currentTask.isPeriodic ) {
      periodicTasks.push_back( currentTask );
    }
    tasks.push( currentTask );
  }
  //cout << "Depois de rolar: " << endl;
  //printTasks( tasks );

  while ( currentTime < t ) {
       if(tasks.empty()){
         for( int i = 0; i < periodicTasks.size(); i++ ) {
            if( currentTime > 0 && (currentTime +1)% (periodicTasks[i].p + periodicTasks[i].a) == 0 ) {
            // Criando nova instância da tarefa com os mesmos parâmetros
            Task newTask = periodicTasks[i];
            newTask.id =  periodicTasks.size() + rand() % ( 300 - periodicTasks.size() + 1 ); // gera um id aleatório para essa tarefa
            tasks.push( newTask ); // Adiciona nova tarefa
            }
        }
        ganttDiagram[currentTime] = '.';
        currentTime++;
        continue;
        }

    Task currentTask = tasks.front(); // Pega a tarefa mais antiga
    tasks.pop();
    //cout << "peguei a tarefa " << currentTask.diagram << " no tempo: " << currentTime << endl;
    // verificação para a execução das aperiódicas:
    if(currentTask.isPeriodic == false){
        if(currentTask.a > currentTime){
            ganttDiagram[currentTime] = '.';
            for( int i = 0; i < periodicTasks.size(); i++ ) {
                //cout << "tarefa "<< periodicTasks[i].diagram << "analisando"<< endl;
                //cout << "tempo + 1= " << currentTime+1 << "período da tarefa: " << periodicTasks[i].p << endl;
                if( currentTime > 0 && (currentTime + 1) % (periodicTasks[i].p + periodicTasks[i].a) == 0 ) {
                     // Criando nova instância da tarefa com os mesmos parâmetros
                    Task newTask = periodicTasks[i];
                    newTask.id =  periodicTasks.size() + rand() % ( 300 - periodicTasks.size() + 1 ); // gera um id aleatório para essa tarefa
                    tasks.push( newTask ); // Adiciona nova tarefa
                    //cout << " criei a tarefa "<< newTask.diagram << endl;
                } 
            }
            tasks.push( currentTask );
            if ( !tasks.empty() ) {
                order( tasks ); // Reordena as tarefas na fila após cada execução se necessário.
            }
            currentTime++;
            continue;
        }
    }
    
    
    currentTask.remainingTime -= 1;
    // adiciona a tarefa no diagrama final
    ganttDiagram[currentTime] = currentTask.diagram;
   // Atualiza o tempo de execução da tarefa correspondente à letra
    int letterIndex = currentTask.diagram - 'A'; // Converte a letra para índice ('A' = 0)
    executionTimes[letterIndex]++; // Incrementa o tempo de execução

    //lógica preempção
    
    if ( currentTask.remainingTime > 0 ) { 
      tasks.push( currentTask ); // Adiciona nova tarefa
    } else {
     // cout << "Tarefa " << currentTask.diagram << " ID " << currentTask.id << " concluída." << endl;
      last_id = currentTask.id;
    }
    for( int i = 0; i < periodicTasks.size(); i++ ) {
        //cout << "tarefa "<< periodicTasks[i].diagram << "analisando"<< endl;
        //cout << "tempo + 1= " << currentTime+1 << "período da tarefa: " << periodicTasks[i].p << endl;
      if( currentTime > 0 && (currentTime + 1) % (periodicTasks[i].p + periodicTasks[i].a) == 0 ) {
        // Criando nova instância da tarefa com os mesmos parâmetros
        Task newTask = periodicTasks[i];
        newTask.id =  periodicTasks.size() + rand() % ( 300 - periodicTasks.size() + 1 ); // gera um id aleatório para essa tarefa
        tasks.push( newTask ); // Adiciona nova tarefa
        //cout << " criei a tarefa "<< newTask.diagram << endl;
      }
    }


    currentTime++;
    cout << "Tempo: " << currentTime << endl;
    //printTasks( tasks );

    if ( !tasks.empty() ) {
      order( tasks ); // Reordena as tarefas na fila após cada execução se necessário.
    }
   //cout << "Depois: " << currentTime << endl;
   //printTasks( tasks );

  }

  cout << "Diagrama de Gantt: ";
  for ( int i = 0; i < ganttDiagram.size(); i++ ) {
    cout << ganttDiagram[i] << ' '; // Adicionando espaço entre os caracteres do diagrama
  }
  cout << endl;

 cout << "Tempos de execução por tarefa (A-Z):" << endl;
    for (int i = 0; i < 26; i++) {
        if (executionTimes[i] > 0) {
            char taskLetter = 'A' + i;
            cout << taskLetter << ": " << executionTimes[i] << " unidades de tempo" << endl;
        }
    }
}

int main() {
  int t, tp, ta;
  srand( static_cast<unsigned int>( time( 0 ) ) );

    while ( true ) {
        queue<Task> tasks;
        read( t, tp, ta, tasks );

        if ( t == 0 && tp == 0 && ta == 0 ) {
            break;
        }

        BS_Execution( t, tasks );
        //  printTasks( tasks );

        cout << endl; // Linha em branco entre os conjuntos de teste.
    }
}
