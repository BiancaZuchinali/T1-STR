#include<iostream>

using namespace std;

struct PeriodicTask {
  unsigned c, p, d;
  unsigned ex, wt;
};

struct AperiodicTask {
  unsigned a, c;
  unsigned ex, wt;
};


void read(int &t, int &tp, int &ta, int &maior_p){
    int trash;
    cout<<"Insira o conjunto de teste:"<<endl;
    cin << t << tp <<ta;
    cin << trash << trash << trash;
    PeriodicTask[tp]; //conferir sixtaxe dessa porra
    AperiodicTask[ta]; //rt
    for(int i = ; i < tp; i++){
        cin << PeriodicTask[i].c <<PeriodicTask[i].p << PeriodicTask[i].d;
        if(PeriodicTask[i].p > maior_p){
            maior_p = PeriodicTask[i].p;
        }
    }
    for(int j = 0; j <ta; i++){
        cin << AperiodicTask[j].a << AperiodicTask[j].c;
    }
}

void BS_Execution(int t){
    int countdown = 0;
    while(countdown < t){

    }

}

int main(){
    int t,tp,ta;
    int maior_p = 0;
    while(t!= 0 && tp != 0 && ta!=0){
        read(t,tp,ta,maior_p);

    }
// fazer uma lista com as tarefas mais prioritárias? tipo em ordem de prioridade?
//como faço pra saber tipo é essa tarefa agr?
}
