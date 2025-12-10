#include<bits/stdc++.h>
using namespace std;

class SingleTon{
protected:
    static SingleTon* pointer;
public:
    /* 
        Static is used to Access function without obj
        Here we are creating object
    */
    // Thread safe
    static SingleTon* getInstance(){
        return pointer;
    }
};

// Declare static syntax 
SingleTon* SingleTon::pointer = new SingleTon(); // Can be heavy

int main(){
    SingleTon* a = SingleTon::getInstance();
    SingleTon* b = SingleTon::getInstance();

    cout<<(a==b)<<endl;
}