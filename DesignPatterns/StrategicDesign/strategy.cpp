#include<bits/stdc++.h>
using namespace std;

class Walkablerobot{
public:
    virtual void walk() = 0;
    virtual ~Walkablerobot(){}
};

class NormalWalk : public Walkablerobot{
public:
    void walk override {
        cout<<"Robot Normal Walk behaviour";
    }
};

class Robot{
private:
    Walkablerobot *w;
    Talkablerobot *t;
    flyablerobot *f;
public:

};