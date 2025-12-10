#include<bits/stdc++.h>
using namespace std;

class Character{
public:
    virtual string getAbilities() = 0;
};

class Mario : public Character{
public:
    string getAbilities() override{
        return "Mario";
    }
};

class Decorater : public Character{
protected:
    Character* character;
public:
    Decorater(Character* ch){
        this->character = ch;
    }
};

class HeightUp : public Decorater{
public:
    HeightUp(Character* ch) : Decorater(ch) {};
    string getAbilities() override{
        return character->getAbilities() +  " with HeightUp";
    }
};

class GunUp : public Decorater{
public:
    GunUp(Character* ch) : Decorater(ch) {};
    string getAbilities() override{
        return character->getAbilities() + " with GunUp";
    }
};

class StarUp : public Decorater{
public:
    StarUp(Character* ch) : Decorater(ch) {};
    string getAbilities() override{
        return character->getAbilities() + " with StarUp";
    }
};

int main(){
    Character *mario = new Mario();
    cout<<mario->getAbilities()<<endl;

    mario = new HeightUp(mario);
    cout<<mario->getAbilities()<<endl;

    mario = new GunUp(mario);
    cout<<mario->getAbilities()<<endl;

    mario = new StarUp(mario);
    cout<<mario->getAbilities()<<endl;

    // Similar to new GunUp(new HeightUp( new Mario())); ALL  are of type character
}