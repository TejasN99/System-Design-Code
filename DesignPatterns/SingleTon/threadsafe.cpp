#include<bits/stdc++.h>
using namespace std;

class SingleTon{
protected:
    static SingleTon* pointer;
    static mutex mtx;
public:
    /* 
        Static is used to Access function without obj
        Here we are creating object
    */
    // Thread safe
    static SingleTon* getInstance(){
        if(pointer == nullptr){
            lock_guard<mutex> lock(mtx);
            if(pointer == nullptr)
                pointer = new SingleTon();
        }
        return pointer;
    }
};

// Declare static syntax
SingleTon* SingleTon::pointer = nullptr;
mutex SingleTon::mtx;

int main(){
    SingleTon* a = SingleTon::getInstance();
    SingleTon* b = SingleTon::getInstance();

    cout<<(a==b)<<endl;
}