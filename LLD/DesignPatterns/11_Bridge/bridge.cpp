#include<bits/stdc++.h>
using namespace std;

// Implementation Heirarhy : Engine Interface

class Engine{
public:
    virtual void start() = 0;
    virtual ~Engine() {};
};

// Concrete Implementors (LLL)
class PetrolEngine : public Engine {
public:
    void start() override {
        cout << "Petrol engine starting with ignition!" << endl;
    }
};

class DieselEngine : public Engine {
public:
    void start() override {
        cout << "Diesel engine roaring to life!" << endl;
    }
};

class ElectricEngine : public Engine {
public:
    void start() override {
        cout << "Electric engine powering up silently!" << endl;
    }
};


// Abstraction Hierarchy: Car (HLL)

class Car{
protected:
    Engine* engine;
public:
    Car(Engine* eng){
        this->engine = eng;
    }
    virtual void drive() = 0;
};

class Sedan : public Car{
public:
    Sedan(Engine* e) : Car(e) {}
    void drive() override{
        engine->start();
        cout << "Driving a Sedan on the highway." << endl;
    }
};

// Refined Abstraction: SUV
class SUV : public Car {
public:
    SUV(Engine* e) : Car(e) {}

    void drive() override {
        engine->start();
        cout << "Driving an SUV off-road." << endl;
    }
};

int main(){
    Car* hondaCity = new Sedan(new PetrolEngine());
    hondaCity->drive();
    
    Car* defender = new SUV(new DieselEngine());
    defender->drive();
}