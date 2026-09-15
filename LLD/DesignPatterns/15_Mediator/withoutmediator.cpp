#include<bits/stdc++.h>
using namespace std;

class User{
private:
    string name;
    vector<User*> peers;
    vector<string> mutedUsers;
public:
    User(const string &n) : name(n) {}
    
    void addPeer(User *usr){
        peers.push_back(usr);
    }

    void mute(const string &userToMute){
        mutedUsers.push_back(userToMute);
    }

    void send(const string &msg){
        cout << "[" << name << " broadcasts]: " << msg << endl;
        for(auto &usr: peers){
            if(!usr->isMuted(name)){
                usr->receive(name, msg);
            }
        }
    }

    bool isMuted(const string& userName) {
        for(auto name : mutedUsers) {
            if(name == userName) {
                return true;
            }
        }
        return false;
    }

    void sendTo(User* target, const string& msg) {
        cout << "[" << name << "→" << target->name << "]: " << msg << endl;
        if(!target->isMuted(name)) {
            target->receive(name, msg);
        }
    }

    void receive(const string& from, const string& msg) {
        cout << "    " << name << " got from " << from << ": " << msg << endl;
    }
};

int main() {
    // create users
    User* user1 = new User("Rohan");
    User* user2 = new User("Neha");
    User* user3 = new User("Mohan");

    // wire up peers (each knows each other) → n*(n-1)/2 connections
    user1->addPeer(user2);   
    user2->addPeer(user1);

    user1->addPeer(user3);   
    user3->addPeer(user1);

    user2->addPeer(user3); 
    user3->addPeer(user2);

    // mute example: Mohan mutes Rohan (Hence Rohan add Mohan to its muted list).
    user1->mute("Mohan");

    // broadcast
    user1->send("Hello everyone!");

    // private
    user3->sendTo(user2, "Hey Neha!");

    // cleanup
    delete user1;
    delete user2;
    delete user3;
    return 0;
}