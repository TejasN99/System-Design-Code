#include<bits/stdc++.h>
using namespace std;

class INotification{
public:
    virtual string getContent() = 0;
};

class SimpleNotification : public INotification{
private:
    string notifyText;
public:
    SimpleNotification(const string &text){
        this->notifyText = text;
    }
    string getContent() override {
        return notifyText;
    }
};

class INotificationDecorator : public INotification{
protected:
    INotification * notification;
public:
    INotificationDecorator(INotification* n){
        this->notification = n;
    }
    virtual ~INotificationDecorator() {
        delete notification;
    }
};

class TimeStampDecorator : public INotificationDecorator{
public:
    TimeStampDecorator(INotification* n) : INotificationDecorator(n) { }
    string getContent() override{
        return "[2025-04-13 14:22:00] " + notification->getContent();
    }
};

class SignatureDecorator : public INotificationDecorator{
private:
    string sig;
public:
    SignatureDecorator(INotification* n, const string &sig) : INotificationDecorator(n) { 
        this->sig = sig;
    }
    string getContent() override {
        return notification->getContent() + "-- " + sig + "\n\n";
    }
};

class INotificationStrategy{
public:
    virtual void sendNotification(const string &s) = 0;
};

class SMSNotification : public INotificationStrategy{
private:
    string mobileNumber;
public:
    SMSNotification(string mobileNumber) {
        this->mobileNumber = mobileNumber;
    }

    void sendNotification(const string &s) override{
        cout<< "Sending SMS " << s;
    }
};

class EmailNotification : public INotificationStrategy{
private:
    string emailId;
public:
    EmailNotification(string emailId) {
        this->emailId = emailId;
    }
    void sendNotification(const string &s) override{
        cout<< "Sending Email " << s;
    }
};

class PopNotification : public INotificationStrategy{
public:
    void sendNotification(const string &s) override{
        cout<< "Sending Pop " << s;
    }
};

class Observer{
public:
    virtual void update() = 0;
};

class IObservable{
protected:
    vector<Observer*> ob;
public:
    virtual void addObserver(Observer* obs) = 0;
    virtual void removeObserver(Observer* obs) = 0;
    virtual void notify() = 0;
};

class NotificationObservable : public IObservable{
private:
    INotification* noti;
public:
    void setNotification(INotification* n){
        this->noti = n;
        notify();
    }

    string getNotification(){
        return this->noti->getContent();
    }

    void addObserver(Observer* obs) override{
        auto itr = find(ob.begin(), ob.end(), obs);
        if(itr == ob.end())
            ob.push_back(obs);
    }

    void removeObserver(Observer* obs) override{
        ob.erase(remove(ob.begin(), ob.end(), obs), ob.end());
    }

    void notify() override {
        for(auto &observer: ob){
            observer->update();
        }
    }
};

class Logger: public Observer{
private: 
    NotificationObservable* noti;
public:
    Logger(NotificationObservable* observable) {
        this->noti = observable;
    }

    void update() override{
        cout<<  "Logging New Notification : " << noti->getNotification();
    }
};

class NotificationEngine: public Observer{
private: 
    NotificationObservable* noti;
    vector<INotificationStrategy*> notificationStrategies;
public:
    NotificationEngine(NotificationObservable* n){
        this->noti = n;
    }

    void addNotificationStrategy(INotificationStrategy* st){
        auto temp = find(notificationStrategies.begin(), notificationStrategies.end(), st);
        if(temp == notificationStrategies.end())
            notificationStrategies.push_back(st);
    }

    void update() override{
        string notificationContent = noti->getNotification();
        for(auto &it: notificationStrategies){
            it->sendNotification(notificationContent);
        }
    }
};


class NotificationService{
private:
    NotificationObservable* observable;
    static NotificationService* instance;
    vector<INotification*> notification;
    
    NotificationService(){
        observable = new NotificationObservable();
    }
public:
    static NotificationService* getInstance() {
        if(instance == nullptr)
            return new NotificationService();
        return instance;
    }

    NotificationObservable* getObservable() {
        return observable;
    }

    void sendNotification(INotification* noti){
        notification.push_back(noti);
        observable->setNotification(noti);
    }
};

NotificationService* NotificationService::instance = nullptr;

int main(){
    NotificationService* notificationService = NotificationService::getInstance();
    NotificationObservable* notificationObservable = notificationService->getObservable();
    Logger* logger = new Logger(notificationObservable);
    NotificationEngine* notificationEngine = new NotificationEngine(notificationObservable);

    notificationEngine->addNotificationStrategy(new EmailNotification("random.person@gmail.com"));
    notificationEngine->addNotificationStrategy(new SMSNotification("+91 9876543210"));
    notificationEngine->addNotificationStrategy(new PopNotification());

    notificationObservable->addObserver(logger);
    notificationObservable->addObserver(notificationEngine);

    INotification* notification = new SimpleNotification("Your Order is Shipped");
    notification = new TimeStampDecorator(notification);
    notification = new SignatureDecorator(notification, "Monk");
    notificationService->sendNotification(notification);
}