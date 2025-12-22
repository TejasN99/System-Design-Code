#include<bits/stdc++.h>
using namespace std;

class CPU{
public:
    void startCpu(){
        cout<<"CPU is started"<<endl;
    }
};

class OtherComponents{
public:
    void startComponents(){
        cout<<"Components are started"<<endl;
    }
};

class Facade{
private:
    CPU* cpu;
    OtherComponents* other;
public:
    void startComputer(){
        cpu->startCpu();
        other->startComponents();
    }
};

int main(){
    Facade* fac = new Facade();
    fac->startComputer();
}