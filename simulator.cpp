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

void read(int &t, int &tp, int &ta, queue<Task>& tasks) {
    int trash;
    cout << "Insira o conjunto de teste:" << endl;
    cin >> t >> tp >> ta; 
    cin >> trash >> trash >> trash;

    char letter = 'A';  // Inicia o contador de letras

    // Lê as tarefas periódicas
    for (int i = 0; i < tp; i++) {
        Task task; 
        cin >> task.c >> task.p >> task.d;
        task.isPeriodic = true;
        task.id = i;
        task.a = 0;
        task.remainingTime = task.c;
        task.diagram = letter++;  // Atribui uma letra e incrementa o contador
        tasks.push(task);
    }
    
    // Lê as tarefas aperiódicas
    for (int j = tp; j < (tp + ta); j++) {
        Task task; 
        cin >> task.a >> task.c;
        task.isPeriodic = false;
        task.id = (j + 2) * 5; // Para ter certeza que não vai dar conflito de id
        task.p = 0; 
        task.d = 0; 
        task.remainingTime = task.c;
        task.diagram = letter++;  // Atribui uma letra e incrementa o contador
        tasks.push(task);
    }
}


void order(queue<Task>& tasks) {
    vector<Task> periodicTasks;
    vector<Task> aperiodicTasks;

    // Transfere tarefas da fila para vetores temporários
    while (!tasks.empty()) {
        Task currentTask = tasks.front();
        tasks.pop();
        
        if (currentTask.isPeriodic) {
            periodicTasks.push_back(currentTask);
        } else {
            aperiodicTasks.push_back(currentTask);
        }
    }

    // Ordena as tarefas periódicas por período (crescente)
    sort(periodicTasks.begin(), periodicTasks.end(), [](const Task& a, const Task& b) {
        return a.p < b.p; // Ordena em ordem crescente de período
    });

    // Ordena as tarefas aperiódicas por tempo de chegada (crescente)
    sort(aperiodicTasks.begin(), aperiodicTasks.end(), [](const Task& a, const Task& b) {
        return a.a < b.a; // Ordena em ordem crescente de chegada
    });

    // Recoloca as tarefas ordenadas na fila
    for (const auto& task : periodicTasks) {
        tasks.push(task);
    }
    
    for (const auto& task : aperiodicTasks) {
        tasks.push(task);
    }
}

void printTasks(queue<Task> tasks) { // cópia para garantir que nada seja mudado na original
    cout << "Tarefas:" << endl;
    
    while (!tasks.empty()) {
        Task task = tasks.front();
        tasks.pop();
        
        cout << "ID: " << task.id 
             << ", Computação: " << task.c 
             << ", Período: " << task.p 
             << ", Deadline: " << task.d 
             << ", Arrival: " << task.a 
             << ", Diagrama: " << task.diagram  // Exibe a letra associada à tarefa
             << ", Periódica: " << (task.isPeriodic ? "Sim" : "Não") 
             << endl;
    }
}


void BS_Execution(int &t, queue<Task>& tasks) {
    vector<char> ganttDiagram(t + 1); // Diagrama de Gantt
    int currentTime = 0;
    int last_id = tasks.front().id;
    int preempcoes = 0;
    int context_change = 0;

    vector<Task> periodicTasks; // Vetor para armazenar apenas as tarefas periódicas, nunca serão alteradas aqui, esse serve só para gerar eventos e novas tarefas

    while (!tasks.empty()) {
        Task currentTask = tasks.front();
        tasks.pop();
        
        if (currentTask.isPeriodic) {
            periodicTasks.push_back(currentTask);
        }
    }

    while (currentTime < t) {
        // verificar qual tarefa tem que executar, se é a primeira mesmo. 
        Task currentTask = tasks.front(); // Pega a tarefa mais antiga
        tasks.pop();
          
        if(last_id != currentTask.id && currentTime > 0 ){
            context_change++;
        }


        currentTask.remainingTime -= 1;
        // adiciona a tarefa no diagrama final
        ganttDiagram[currentTime] = currentTask.diagram;

        //lógica preempção
        //ver pro processador em idle
        int count_tasks = 0;
        for(int i = 0; i < tasks.size() ; i++){
           if(task[i].remainingTime == 0) count_tasks++;
        }
        if ( count_tasks == tasks.size()) ganttDiagram[currentTime] = '.';

        if (currentTask.remainingTime > 0) { // também tenho que criar uma tarefa igual a essa na fila
            Task newTask = currentTask; 
            newTask.remainingTime = currentTask.remainingTime; // COpia o tempo restante
            newTask.c = currentTask.c;
            newTask.a = currentTask.a;
            newTask.p = currentTask.p;
            newTask.ex = currentTask.ex;
            newTask.wt = currentTask.wt;
            newTask.isPeriodic = currentTask.isPeriodic;
            newTask.diagram = currentTask.diagrem;
            srand(static_cast<unsigned int>(time(0)));
            newTask.id =  periodicTasks.size() + rand() % (300 - periodicTasks.size() + 1); // gera um id aleatório para essa tarefa
            tasks.push(newTask);   // Adiciona nova tarefa
        } else {
            cout << "Tarefa "<< currentTask.diagram << "ID " << currentTask.id << " concluída." << endl;
            last_id = currentTask.id;
        }
        for(int i = 0; i < periodicTasks.size(); i++){
            if(currentTime % periodicTasks[i].p == 0){
            // Criando nova instância da tarefa com os mesmos parâmetros
            Task newTask = periodicTasks[i]; 
            newTask.remainingTime = periodicTasks[i].c; // Reinicia o tempo restante
            newTask.c = periodicTasks[i].c;
            newTask.a = periodicTasks[i].a;
            newTask.p = periodicTasks[i].p;
            newTask.ex = periodicTasks[i].ex;
            newTask.wt = periodicTasks[i].wt;
            newTask.diagram = periodicTasks[i].diagram;
            newTask.isPeriodic = periodicTasks[i].isPeriodic;
            srand(static_cast<unsigned int>(time(0)));
            newTask.id =  periodicTasks.size() + rand() % (300 - periodicTasks.size() + 1); // gera um id aleatório para essa tarefa
            tasks.push(newTask);   // Adiciona nova tarefa
            }
        }
        
        currentTime++;
        order(tasks); // Reordena as tarefas na fila após cada execução se necessário.      
    }

    cout << "Diagrama de Gantt: ";
    for (char c : ganttDiagram) {
        cout << c << ' '; // Adicionando espaço entre os caracteres do diagrama
    }
    cout << endl;
}

int main() {
    int t, tp ,ta;
    
    while (true) {
        queue<Task> tasks;         
        read(t, tp, ta, tasks);
        
        if (t == 0 && tp == 0 && ta == 0) {
            break;  
        } 
        
        BS_Execution(t, tasks); 
        printTasks(tasks); 

        cout << endl; // Linha em branco entre os conjuntos de teste.
    }
}
