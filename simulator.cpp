#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

struct PeriodicTask {
    unsigned c, p, d; // tempo de computação, período, deadline
    unsigned ex = 0;  // tempo total executado
    unsigned wt = 0;  // tempo de espera
    unsigned lastExecutionTime = 0; // Tempo da última execução
};

struct AperiodicTask {
    unsigned a, c; // tempo de chegada, tempo de computação
    unsigned ex = 0;  // tempo total executado
    unsigned wt = 0;  // tempo de espera
};

void read(int &t, int &tp, int &ta, int &maior_p, vector<PeriodicTask> &periodicTasks, vector<AperiodicTask> &aperiodicTasks) {
    int trash;
    cout << "Insira o conjunto de teste:" << endl;
    cin >> t >> tp >> ta; 
    cin >> trash >> trash >> trash;

    periodicTasks.resize(tp); 
    aperiodicTasks.resize(ta); 

    for (int i = 0; i < tp; i++) { // guarda as infos das tarefas periódicas
        cin >> periodicTasks[i].c >> periodicTasks[i].p >> periodicTasks[i].d;
        if (periodicTasks[i].p > maior_p) {
            maior_p = periodicTasks[i].p;
        }
    }
    for (int j = 0; j < ta; j++) { // guarda as infos das tarefas aperiódicas
        cin >> aperiodicTasks[j].a >> aperiodicTasks[j].c;
    }
}

void BS_Execution(int t, vector<PeriodicTask> &periodicTasks, vector<AperiodicTask> &aperiodicTasks) {
    vector<char> ganttDiagram(t + 1, '.'); // Diagrama de Gantt
    int preemptions = 0;                     // Contador de preempções
    int contextSwitches = 0;                 // Contador de trocas de contexto
    int currentTime = 0;

    //PREPARAÇÃO PARA A EXECUÇÃO-----------------------------------------------------------

    //para tarefas periódicas
    vector<int> remainingComputation(periodicTasks.size(), 0);
    vector<int> nextExecutionTime(periodicTasks.size(), 0);
    // Vetor para rastrear se a tarefa já foi executada
    vector<bool> executed(periodicTasks.size(), false);

    //para tarefas aperiódicas
    vector<int> remainingComputationAperiodic(aperiodicTasks.size(),0);
    vector<int> inicialExecutionTime(aperiodicTasks.size(),0); // quando elas chegam
    // Vetor para rastrear se a tarefa já foi executada
    vector<bool> executedAperiodic(aperiodicTasks.size(), false);

    for (int i = 0; i < periodicTasks.size(); i++) {
        nextExecutionTime[i] = periodicTasks[i].p; // Próximo tempo de execução
        //cout << " prox temp exec tarefa: " << i << " é: " << nextExecutionTime[i] << endl;
        remainingComputation[i] = periodicTasks[i].c; // Tempo restante inicial
        //cout << " tempo rest inicial da tarefa: " << i << " é: " << remainingComputation[i] << endl;
    }

    for (int j = 0; j < aperiodicTasks.size(); j++){
        remainingComputationAperiodic[j] = aperiodicTasks[j].c;
        //cout << " tempo rest inicial da tarefa: " << j << " é: " << remainingComputationAperiodic[j]<< endl;
        inicialExecutionTime[j] = aperiodicTasks[j].a;
        //cout << " o tempo em q a tarefa:" << j << " fica disponível é: " << inicialExecutionTime[j] << endl;
    }

    //filas de execução
    deque<PeriodicTask> periodicDeque(periodicTasks.begin(), periodicTasks.end());
    deque<AperiodicTask> aperiodicDeque(aperiodicTasks.begin(),aperiodicTasks.end());

    // Ordenando a deque do maior para o menor período
    sort(periodicDeque.begin(), periodicDeque.end(), [](const PeriodicTask& a, const PeriodicTask& b) {
        return a.p > b.p; // Ordena por período em ordem decrescente
    });

    // Exibindo a deque após a ordenação
    /*cout << "Deque de Tarefas Periódicas (ordenada do maior para o menor período):" << endl;
    for (const auto& task : periodicDeque) {
        cout << "C: " << task.c << ", P: " << task.p << ", D: " << task.d << endl;
    }*/
 
    // Ordenando as tarefas aperiódicas pela ordem de chegada (campo `a`)
    sort(aperiodicDeque.begin(), aperiodicDeque.end(), [](const AperiodicTask& a, const AperiodicTask& b) {
        return a.a < b.a; // Ordena por tempo de chegada em ordem crescente
    });

    // Exibindo a deque após a ordenação das tarefas aperiódicas
    /*cout << "Deque de Tarefas Aperiódicas (ordenada pela ordem de chegada):" << endl;
    for (const auto& task : aperiodicDeque) {
        cout << "A: " << task.a << ", C: " << task.c << endl;
    }*/
    
    //EXECUÇÃO DE FATO -----------------------------------------------------------------------------
    // Loop enquanto houver tarefas aperiódicas restantes
    while (true) {
        bool allZero = true;

        // Verifica se todas as tarefas aperiódicas estão zeradas
        for (size_t i = 0; i < remainingComputationAperiodic.size(); i++) {
            if (remainingComputationAperiodic[i] > 0) {
                allZero = false; // Se encontrar um elemento não zero, continua o loop
                break; // Sai do loop se encontrar um elemento não zero
            }
        }
        if (allZero) {
            break; // Sai do loop se todos os elementos forem zero
        }

        // EXECUTANDO TAREFAS PERIÓDICAS
        for (int i = 0; i < periodicDeque.size(); i++) {  // passa por todas tarefas periódicas
            char taskIdentifier = 'A' + i; // 'A' é a letra correspondente à primeira tarefa
            
            while (remainingComputation[i] > 0) { // roda até que as computações não tenham sido terminadas
                /*for(int j=0; j < periodicTasks.size(); j++){// comparar preempções n sei bem ainda
                    if(){ /// ferificar aqui a questão das preempções

                    }
                }*/
                remainingComputation[i] -= 1;  // Executa todas as computações das tarefas periódicas em ordem
                currentTime++;
                //cout << "Executando tarefa " << taskIdentifier << " (C: " << periodicDeque[i].c << ", P: " << periodicDeque[i].p << ") - Tempo atual: " << currentTime << endl;

                for (int j = 0; j < periodicTasks.size(); j++) { 
                    if (j != i && !executed[j]) { 
                        periodicTasks[j].wt += 1;
                        //cout << "Atualizando WT da tarefa " << char('A' + j) << ": Novo WT = " << periodicTasks[j].wt << endl;
                    }
                }
                executed[i] = true; // Marca a tarefa atual como executada
            }
            //cout << "Tarefa " << taskIdentifier << " concluída." << endl;
            // talvez teria que colocar ele denovo na fila, acho q sim, mas ainda n tenho certeza de qual posição e de como tratar disso depois
        }

        cout << "COMEÇANDO A EXECUTAR TAREFAS APERIÓDICAS:" << endl;
        
        for (int i = 0; i < aperiodicDeque.size(); i++) { 
            if (aperiodicTasks[i].a <= currentTime) { // Verifica se a tarefa está pronta para execução
                char taskIdentifier = 'A' + periodicTasks.size() + i; // Continua a sequência alfabética

                while (remainingComputationAperiodic[i] > 0) {
                    remainingComputationAperiodic[i] -= 1;  
                    currentTime++;

                    cout << "Executando tarefa " << taskIdentifier 
                         << " (C: " << aperiodicDeque[i].c 
                         << ", A: " << aperiodicDeque[i].a 
                         << ") - Tempo atual: " << currentTime << endl;

                    for (int j = 0; j < aperiodicTasks.size(); j++) { 
                        if (j != i && !executedAperiodic[j]) { 
                            aperiodicTasks[j].wt += 1;
                            cout << "Atualizando WT da tarefa " 
                                 << char('A' + periodicTasks.size() + j) 
                                 << ": Novo WT = " << aperiodicTasks[j].wt << endl;
                        }
                    }
                }
                executedAperiodic[i] = true; // Marca a tarefa atual como executada
                cout << "Tarefa " << taskIdentifier << " concluída." << endl;
            }
        }
    }

    cout << "Todas as tarefas periódicas e aperiódicas foram processadas." << endl;
}


int main() {
    int t, tp, ta;
    
    while (true) {
        int maior_p = 0;
        vector<PeriodicTask> periodicTasks;
        vector<AperiodicTask> aperiodicTasks;

        read(t, tp, ta, maior_p, periodicTasks, aperiodicTasks);
        
        if (t == 0 && tp == 0 && ta == 0){
            break;  
        }  
        
        BS_Execution(t, periodicTasks, aperiodicTasks); 

        cout << endl; // Linha em branco entre os conjuntos de teste.
    }
}
