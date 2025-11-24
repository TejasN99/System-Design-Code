#include<bits/stdc++.h>
using namespace std;

class Walkablerobot{
public:
    virtual void walk() = 0;
    virtual ~Walkablerobot(){}
};

class NormalWalk : public Walkablerobot{
public:
    void walk() override {
        cout<<"Robot Normal Walk behaviour"<<endl;
    }
};

class NoWalk : public Walkablerobot{
public:
    void walk() override {
        cout<<"Robot No Walk behaviour"<<endl;
    }
};

class Talkablerobot{
public:
    virtual void talk() = 0;
    virtual ~Talkablerobot(){};
};

class TalkRobot : public Talkablerobot{
public:
    void talk() override {
        cout<<"Talk behaviour"<<endl;
    }
};

class NoTalkRobot : public Talkablerobot{
public:
    void talk() override {
        cout<<"No Talk behaviour"<<endl;
    }
};

class FlyableRobot {
public:
    virtual void fly() = 0;
    virtual ~FlyableRobot() {}
};

class NormalFly : public FlyableRobot {
public:
    void fly() override { 
        cout << "Flying normally..." << endl; 
    }
};

class NoFly : public FlyableRobot {
public:
    void fly() override { 
        cout << "Cannot fly." << endl; 
    }
};

class Robot{
private:
    Walkablerobot *walkBehavior;
    Talkablerobot *talkBehavior;
    FlyableRobot *flyBehavior;
public:
    Robot(Walkablerobot* w, Talkablerobot* t, FlyableRobot* f){
        this->walkBehavior = w;
        this->talkBehavior = t;
        this->flyBehavior = f;
    }  
    
    void walk(){
        walkBehavior->walk();
    }
    void talk(){
        talkBehavior->talk();
    }
    void fly(){
        flyBehavior->fly();
    }

    virtual void projection() = 0;
};

class CompanionRobot : public Robot{
public:
    CompanionRobot(Walkablerobot* w, Talkablerobot* t, FlyableRobot* f) : Robot(w, t, f){};

    void projection() override {
        cout << "Displaying friendly companion features..." << endl;
    }
};

class WorkerRobot : public Robot {
public:
    WorkerRobot(Walkablerobot* w, Talkablerobot* t, FlyableRobot* f)
        : Robot(w, t, f) {}

    void projection() override {
        cout << "Displaying worker efficiency stats..." << endl;
    }
};


int main(){
    Robot *robot1 = new CompanionRobot(new NormalWalk(), new TalkRobot(), new NormalFly());
    robot1->walk();
    robot1->talk();
    robot1->fly();
    robot1->projection();

    cout << "--------------------" << endl;

    Robot *robot2 = new WorkerRobot(new NoWalk(), new NoTalkRobot(), new NormalFly());
    robot2->walk();
    robot2->talk();
    robot2->fly();
    robot2->projection();
}