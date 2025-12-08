#include<bits/stdc++.h>
using namespace std;

class Isubscriber{
public:
    virtual void update() = 0;
    virtual ~Isubscriber() {};
};

class Ichannel{
public:
    virtual void subsribe(Isubscriber * subs) = 0;
    virtual void unsubsribe(Isubscriber * subs) = 0;
    virtual void notify() = 0;
};

class Channel : public Ichannel{
private:
    string channel_name;
    vector<Isubscriber*> subs;
    string latestVideoName;
public:
    Channel(const string &name){
        this->channel_name = name;
    }

    void subsribe(Isubscriber * subscriber) override{
        if(find(subs.begin(), subs.end(), subscriber) == subs.end())
            subs.push_back(subscriber);
    }

    void unsubsribe(Isubscriber * subscriber) override{
        auto it = find(subs.begin(), subs.end(), subscriber);
        if(it != subs.end()){
            subs.erase(it);
        }
    }

    void notify() override{
        for(auto &it: subs){
            it->update();
        }
    }

    void uploadVideo(const string &title){
        latestVideoName = title;
        cout<<"Latest Video Name is "<< title << endl;
        notify();
    }

    string getVideoData(){
        return latestVideoName;
    }
};

class Subcriber: public Isubscriber{
private:
    string sub_name;
    Channel *ch;
public:
    Subcriber(const string &sub_name, Channel* ch){
        this->sub_name = sub_name;
        this->ch = ch;
    }

    void update() override{
        cout<<"New Video for "<<sub_name<< " "<<ch->getVideoData()<<endl;
    }
};

int main(){
    Channel* tejasLeet = new Channel("TejasLeet");
    Subcriber *sub1 = new Subcriber("Tejas", tejasLeet);
    Subcriber *sub2 = new Subcriber("Aditi", tejasLeet);

    tejasLeet->subsribe(sub1);
    tejasLeet->subsribe(sub2);

    tejasLeet->uploadVideo("Two sum");
    cout<<endl;
    
    tejasLeet->unsubsribe(sub2);
    tejasLeet->uploadVideo("Three sum");
}