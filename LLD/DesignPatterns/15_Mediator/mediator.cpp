#include <bits/stdc++.h>
using namespace std;

class IMediator;

class IColleague {
public:
    virtual void setMediator(IMediator* med) = 0;
    virtual void sendMsg(const string& msg) = 0;
    virtual void receive(const string& msg) = 0;

    virtual void muteUser(const string& userName) = 0;
    virtual void unmuteUser(const string& userName) = 0;
    virtual bool isMuted(const string& userName) = 0;

    virtual string getName() = 0;

    virtual ~IColleague() = default;
};


class IMediator {
public:
    virtual void addUser(IColleague* usr) = 0;
    virtual void removeUser(IColleague* usr) = 0;
    virtual void sendMsg(IColleague* sender, const string& msg) = 0;

    virtual ~IMediator() = default;
};


class ConcreteColleague : public IColleague {
private:
    string name;
    IMediator* med;
    unordered_set<string> mutedUsers;

public:
    ConcreteColleague(const string& name, IMediator* medi)
        : name(name), med(medi) {}

    void setMediator(IMediator* med) override {
        this->med = med;
    }

    void sendMsg(const string& msg) override {
        med->sendMsg(this, msg);
    }

    void receive(const string& msg) override {
        cout << name << ": " << msg << endl;
    }

    void muteUser(const string& userName) override {
        mutedUsers.insert(userName);
    }

    void unmuteUser(const string& userName) override {
        mutedUsers.erase(userName);
    }

    bool isMuted(const string& userName) override {
        return mutedUsers.find(userName) != mutedUsers.end();
    }

    string getName() override {
        return name;
    }
};


class ConcreteMediator : public IMediator {
private:
    vector<IColleague*> list;

public:
    void addUser(IColleague* usr) override {
        list.push_back(usr);
    }

    void removeUser(IColleague* usr) override {
        auto itr = find(list.begin(), list.end(), usr);

        if (itr != list.end()) {
            list.erase(itr);
        }
    }

    void sendMsg(IColleague* sender, const string& msg) override {
        for (auto& usr : list) {

            // Don't send to the sender
            if (usr == sender) {
                continue;
            }

            // Don't send if recipient has muted sender
            if (usr->isMuted(sender->getName())) {
                continue;
            }

            usr->receive(sender->getName() + ": " + msg);
        }
    }
};


int main() {
    IMediator* medi = new ConcreteMediator();

    IColleague* user1 = new ConcreteColleague("Tejas", medi);
    IColleague* user2 = new ConcreteColleague("Tru", medi);
    IColleague* user3 = new ConcreteColleague("Dars", medi);
    IColleague* user4 = new ConcreteColleague("Vish", medi);

    medi->addUser(user1);
    medi->addUser(user2);
    medi->addUser(user3);
    medi->addUser(user4);

    // Tejas mutes Tru
    user1->muteUser("Tru");

    // Tru sends a message
    user2->sendMsg("Hello everyone!");

    delete user1;
    delete user2;
    delete user3;
    delete user4;
    delete medi;
}