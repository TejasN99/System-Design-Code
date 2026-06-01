#include<bits/stdc++.h>
using namespace std;

class Burger{
public:
    virtual void prepare() = 0; 
    virtual ~Burger() {};
};

class BasicBurger : public Burger{
public:
    void prepare() override {
        cout<<"Basic Burger is printed"<<endl;
    }
};

class PremiumBurger : public Burger{
public:
    void prepare() override {
        cout<<"Premium Burger is printed"<<endl;
    }
};

class GourmeBurger : public Burger{
public:
    void prepare() override {
        cout<<"Gourme Burger is printed"<<endl;
    }
};

class BurgerFactor{
public:
    Burger* takeOrder(string &type){
        if(type == "basic")
            return new BasicBurger();
        else if(type == "premium")
            return new PremiumBurger();
        else if(type == "gourme")
            return new GourmeBurger();
        return nullptr;
    }
};

int main(){
    string type = "premium";
    BurgerFactor* fact = new BurgerFactor();
    Burger* burg = fact->takeOrder(type);
    burg->prepare();
}