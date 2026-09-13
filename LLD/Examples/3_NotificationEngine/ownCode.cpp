#include<bits/stdc++.h>
using namespace std;

class INotification{
public:
    virtual string getContent() = 0;
    virtual ~INotification() = default;
    // Always add destructor in the abstract classes
};

class SimpleNotification : public INotification {
private:
    string text;
public:
    SimpleNotification(const string &content) : text(content) {}
    
    string getContent() override {
        return text;    
    }
};

class INotificationDecorator : public INotification{
protected:
    INotification * noti;
public:
    INotificationDecorator(INotification* n) : noti(n) {}
    virtual ~INotificationDecorator() {
        delete noti;
    }
};

class TimeStampDecorator : public INotificationDecorator{
public: 
    TimeStampDecorator(INotification *n) : INotificationDecorator(n) {}
    string getContent() override {
        return noti->getContent() + " with TimeStamp";
    }
};

class SignatureDecorator : public INotificationDecorator{
public:
    SignatureDecorator(INotification *n) : INotificationDecorator(n) {}
    string getContent() override {
        return noti->getContent() + " with Signature";
    }
};

class Iobserverable;

class InotificationStrategy{
public:
    virtual void sendNotification(const string& content) = 0;
    virtual ~InotificationStrategy() = default;
};

class EmailStrategy : public InotificationStrategy{
private:
    string emailId;
public:
    EmailStrategy(const string &email) : emailId(email) {}
    void sendNotification(const string & content) override{
        cout << content << " via Email"<< endl;
    }
};

class SMSStrategy : public InotificationStrategy{
private:
    string phone;
public:
    SMSStrategy(const string &phone) : phone(phone) {}
    void sendNotification(const string & content) override{
        cout << content << " via SMS"<< endl;
    }
};

class IObserver{
public:
    virtual void update() = 0;
    virtual ~IObserver () = default;
};

class Iobserverable{
protected:
    vector<IObserver*> obs;
public:
    virtual ~Iobserverable() = default;

    void addObserver(IObserver* observer){
        if(find(obs.begin(), obs.end(), observer) == obs.end()){
            obs.push_back(observer);
        }
    }

    void removeObserver(IObserver* observer) {
        auto it = find(obs.begin(), obs.end(), observer);
        if (it != obs.end()) {
            obs.erase(it);
        }
    }

    virtual void notify() = 0;
};

class NotificationObservable : public Iobserverable{
private:
    INotification* noti;
public:
    void notify() override{
        for(auto &observer: obs){
            observer->update();
        }
    }

    void setNotification(INotification* n){
        noti = n;
        notify();
    }

    INotification* getNotification() const {
        return noti;
    }
};

class NotificationService{
private:
    vector<INotification*> notifications;
    NotificationObservable* obs;
    NotificationService() {
        obs = new NotificationObservable();
    }
public:
    ~NotificationService(){
        delete obs;
    }

    // One who is the owner will destroy the object
    static NotificationService& getInstance(){
        static NotificationService instance;
        return instance;
    }


    vector<INotification*> getNotification() const {
        return notifications;
    }

    NotificationObservable* getObservable() {
        return obs;
    }

    void sendNotification(INotification* noti){
        obs->setNotification(noti);
    }
};

class Logger : public IObserver{
private:
    NotificationObservable *observable;
public:
    Logger(){
        observable = NotificationService::getInstance().getObservable();
        observable->addObserver(this);
    }
    // we add ourselves then in the end remove ourselves
    ~Logger(){
        if(observable)  observable->removeObserver(this);
    }

    void update() override {
        cout << observable->getNotification()->getContent() << endl;
    }
};

class NotificationEngine : public IObserver{
private:
    NotificationObservable *observable;
    vector<InotificationStrategy*> ns;
public:
    NotificationEngine(){
        observable = NotificationService::getInstance().getObservable();
        observable->addObserver(this);
    }

    ~NotificationEngine(){
        if(observable)  observable->removeObserver(this);
        for(auto &it: ns){
            delete it;
        }
    }

    void update() override {
        string content = observable->getNotification()->getContent();
        for(auto &strategy: ns){
            strategy->sendNotification(content);
        }
    }

    void addNotificationStrategy(InotificationStrategy* s) {
        if (find(ns.begin(), ns.end(), s) == ns.end())
            ns.push_back(s);
    }
};


int main(){
    /*
    & is important as then the notification::instance will try to create a new object
    and then when int main exits then in that case frees the pointer
    and when service goes out of scope it tries to free 
    double free
    */ 

    NotificationService& service = NotificationService::getInstance();

    Logger logger;
    NotificationEngine engine;

    engine.addNotificationStrategy(new EmailStrategy("monk99@gmail.com"));
    engine.addNotificationStrategy(new SMSStrategy("888888888"));

    INotification* notification = new SimpleNotification("Your order is shipped");
    notification = new SignatureDecorator(notification);
    notification = new TimeStampDecorator(notification);

    service.sendNotification(notification);
    delete notification;
}
