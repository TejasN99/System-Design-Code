#include<bits/stdc++.h>
using namespace std;

class ICommand{
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual ~ICommand() = default;
    /*
    This is written here the destructor because of this
    Icommand * command = new FanCommand(fan) so when remote control destroys FAncommand 
    it should destroy the Icommand

    so actual flow is 
    delete command
    fancommand is deleted 
    icommand is deleted
    release memory

    if we skip this default
    1. Derived destructor doesn't run
    2. Resources can remain unreleased

    so this process is not followed 

    so when we use delete through base class we should have virtual destructor
    */
};

class Fan{
public:
    void on(){  cout<<"Fan is on"<<endl;    }
    void off(){ cout<<"Fan is off"<<endl;   }
};

class Light{
public:
    void on(){  cout<<"Light is on"<<endl;  }
    void off(){ cout<<"Light is off"<<endl; }
};

class FanCommand : public ICommand{
private:
    Fan* fan;
public:
    FanCommand(Fan* f){
        this->fan = f;
    }
    void execute() override{
        fan->on();
    }
    void undo() override{
        fan->off();
    }
};

class LightCommand : public ICommand{
private:
    Light* light;
public:
    LightCommand(Light *l){
        this->light = l;
    }
    void execute() override{
        light->on();
    }
    void undo() override{
        light->off();
    }
};


class RemoteControl{
private:
    vector<ICommand*> command;
    vector<bool> pressedButtons;
public:
    RemoteControl(){
        for(int i=0; i<command.size(); i++){
            command[i] = nullptr;
            pressedButtons[i] = false;
        }
    }

    void addCommand(ICommand* cmd){
        command.push_back(cmd);
        pressedButtons.push_back(false);
    }

    void pressButton(int index){
        if(index >= command.size()){
            cout<<"No such Button exist"<<endl;
            return;
        }

        if(pressedButtons[index]){
            command[index]->undo();
            pressedButtons[index] = false;
        }
        else{
            command[index]->execute();
            pressedButtons[index] = true;
        }
        
    }

    ~RemoteControl(){
    for (auto cmd : command) {
        delete cmd;
    }
}
};

int main(){
    RemoteControl* remote = new RemoteControl();
    Fan* fan = new Fan(); // Dont use uninitialized pointers
    Light* light = new Light();
    remote->addCommand(new FanCommand(fan)); // 0
    remote->addCommand(new LightCommand(light)); // 1

    remote->pressButton(0);
    remote->pressButton(0);
    remote->pressButton(1);
    remote->pressButton(1);
}