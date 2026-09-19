#include<bits/stdc++.h>
using namespace std;

class NPC{
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

    void describe() {
        cout << "  NPC: " << name << " | HP=" << health << " ATK=" << attack
             << " DEF=" << defense << "\n";
    }    
};

int main() {
    NPC* alien = new NPC("Alien", 100, 100, 100);
    alien->describe();

    NPC* alien2 = new NPC("Powerfull-Alien", 200, 200, 200);
    alien2->describe();
}