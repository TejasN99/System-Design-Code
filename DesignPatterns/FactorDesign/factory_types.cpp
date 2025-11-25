#include<bits/stdc++.h>
using namespace std;

// Extending Factory

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

class BasicBurgerWheat : public Burger{
public:
    void prepare() override {
        cout<<"Basic Burger Wheat is printed"<<endl;
    }
};

class PremiumBurgerWheat : public Burger{
public:
    void prepare() override {
        cout<<"Premium Burger Wheat is printed"<<endl;
    }
};

class GourmeBurgerWheat : public Burger{
public:
    void prepare() override {
        cout<<"Gourme Burger Wheat is printed"<<endl;
    }
};

class BurgerFactor{
public:
    virtual Burger* takeOrder(string &type) = 0;
};

class SinghBurger : public BurgerFactor{
public:
    Burger* takeOrder(string &type) override{
        if(type == "basic")
            return new BasicBurger();
        else if(type == "premium")
            return new PremiumBurger();
        else if(type == "gourme")
            return new GourmeBurger();
        return nullptr;
    }
};

class KingBurger : public BurgerFactor{
public:
    Burger* takeOrder(string &type) override{
        if(type == "basic")
            return new BasicBurgerWheat();
        else if(type == "premium")
            return new PremiumBurgerWheat();
        else if(type == "gourme")
            return new GourmeBurgerWheat();
        return nullptr;
    }
};

int main(){
    string type = "premium";
    BurgerFactor* fact = new KingBurger();
    Burger* burg = fact->takeOrder(type);
    burg->prepare();
}