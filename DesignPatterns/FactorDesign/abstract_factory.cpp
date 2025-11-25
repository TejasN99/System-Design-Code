#include<bits/stdc++.h>
using namespace std;

// THis is making more factories

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

class Garlic{
public:
    virtual void prepare() = 0; 
    virtual ~Garlic() {};
};

class BasicGarlic : public Garlic{
public:
    void prepare() override {
        cout<<"Basic Garlic is printed"<<endl;
    }
};

class PremiumGarlic : public Garlic{
public:
    void prepare() override {
        cout<<"Premium Garlic is printed"<<endl;
    }
};

class GourmeGarlic : public Garlic{
public:
    void prepare() override {
        cout<<"Gourme Garlic is printed"<<endl;
    }
};

class BasicGarlicWheat : public Garlic{
public:
    void prepare() override {
        cout<<"Basic Garlic Wheat is printed"<<endl;
    }
};

class PremiumGarlicWheat : public Garlic{
public:
    void prepare() override {
        cout<<"Premium Garlic Wheat is printed"<<endl;
    }
};

class GourmeGarlicWheat : public Garlic{
public:
    void prepare() override {
        cout<<"Gourme Garlic Wheat is printed"<<endl;
    }
};

class MealFactor{
public:
    virtual Burger* takeOrder(string &type) = 0;
    virtual Garlic* takeOrderGarlic(string &type) = 0;
};

class SinghBurger : public MealFactor{
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

    Garlic* takeOrderGarlic(string &type) override{
        if(type == "basic")
            return new BasicGarlic();
        else if(type == "premium")
            return new PremiumGarlic();
        else if(type == "gourme")
            return new GourmeGarlic();
        return nullptr;
    }
};

class KingBurger : public MealFactor{
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

    Garlic* takeOrderGarlic(string &type) override{
        if(type == "basic")
            return new BasicGarlicWheat();
        else if(type == "premium")
            return new PremiumGarlicWheat();
        else if(type == "gourme")
            return new GourmeGarlicWheat();
        return nullptr;
    }
};

int main(){
    string type = "premium";
    MealFactor* fact = new KingBurger();
    Burger* burg = fact->takeOrder(type);
    Garlic* gar = fact->takeOrderGarlic(type);
    burg->prepare();
    gar->prepare();
}