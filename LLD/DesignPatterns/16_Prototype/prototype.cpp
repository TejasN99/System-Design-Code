#include<bits/stdc++.h>
using namespace std;


// This is called marker class, just to mark this is clonable
// class Cloneable {
// public:
//     virtual Cloneable* clone() const = 0;
//     virtual ~Cloneable() {}
// };

// you can mark with this

class NPC {
public:
    string name;
    int health;
    int attack;
    int defense;

    NPC(const string& name, int health, int attack, int defense){
        this->name = name;
        this->health = health;
        this->attack = attack;
        this->defense = defense;

        cout << "Creating NPC '" << name << "' [HP:" << health << ", ATK:" 
             << attack << ", DEF:" << defense << "]\n";        
    }

    NPC(NPC *npc){
        this->name = npc->name;
        this->health = npc->health;
        this->attack = npc->attack;
        this->defense = npc->defense;

        cout << "Creating NPC '" << name << "' [HP:" << health << ", ATK:" 
             << attack << ", DEF:" << defense << "]\n";  
        cout<< "Created by copy constructor " <<endl;
    }

    void describe() {
        cout << "  NPC: " << name << " | HP=" << health << " ATK=" << attack
             << " DEF=" << defense << "\n";
    }    


};

int main() {
    NPC* alien = new NPC("alien", 100, 100, 100);
    alien->describe();

    NPC* powerAlien = new NPC(alien);
    powerAlien->attack = 200;
    powerAlien->health = 200;
    powerAlien->defense = 200;

    powerAlien->describe();

}