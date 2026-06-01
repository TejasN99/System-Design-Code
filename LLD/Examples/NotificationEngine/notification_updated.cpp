#include <bits/stdc++.h>
using namespace std;

/* ================= FORWARD DECLARATIONS ================= */
class NotificationService;
class NotificationObservable;

/* ================= NOTIFICATION INTERFACE ================= */
class INotification {
public:
    virtual string getContent() = 0;
    virtual ~INotification() = default;
};

class SimpleNotification : public INotification {
private:
    string notifyText;
public:
    SimpleNotification(const string& text) : notifyText(text) {}

    string getContent() override {
        return notifyText;
    }
};

/* ================= DECORATOR ================= */
class INotificationDecorator : public INotification {
protected:
    INotification* notification;
public:
    INotificationDecorator(INotification* n) : notification(n) {}

    virtual ~INotificationDecorator() {
        delete notification;
    }
};

class TimeStampDecorator : public INotificationDecorator {
public:
    TimeStampDecorator(INotification* n) : INotificationDecorator(n) {}

    string getContent() override {
        return "[2025-04-13 14:22:00] " + notification->getContent();
    }
};

class SignatureDecorator : public INotificationDecorator {
private:
    string sig;
public:
    SignatureDecorator(INotification* n, const string& sig)
        : INotificationDecorator(n), sig(sig) {}

    string getContent() override {
        return notification->getContent() + " -- " + sig + "\n";
    }
};

/* ================= STRATEGY ================= */
class INotificationStrategy {
public:
    virtual void sendNotification(const string& s) = 0;
    virtual ~INotificationStrategy() = default;
};

class SMSNotification : public INotificationStrategy {
private:
    string mobileNumber;
public:
    SMSNotification(const string& mobileNumber) : mobileNumber(mobileNumber) {}

    void sendNotification(const string& s) override {
        cout << "Sending SMS to " << mobileNumber << " : " << s;
    }
};

class EmailNotification : public INotificationStrategy {
private:
    string emailId;
public:
    EmailNotification(const string& emailId) : emailId(emailId) {}

    void sendNotification(const string& s) override {
        cout << "Sending Email to " << emailId << " : " << s;
    }
};

class PopNotification : public INotificationStrategy {
public:
    void sendNotification(const string& s) override {
        cout << "Sending Pop Notification : " << s;
    }
};

/* ================= OBSERVER ================= */
class Observer {
public:
    virtual void update() = 0;
    virtual ~Observer() = default;
};

class IObservable {
protected:
    vector<Observer*> observers;
public:
    virtual void addObserver(Observer* obs) = 0;
    virtual void removeObserver(Observer* obs) = 0;
    virtual void notify() = 0;
    virtual ~IObservable() = default;
};

/* ================= OBSERVABLE ================= */
class NotificationObservable : public IObservable {
private:
    INotification* noti = nullptr;
public:
    void setNotification(INotification* n) {
        noti = n;
        notify();
    }

    string getNotification() {
        return noti ? noti->getContent() : "";
    }

    void addObserver(Observer* obs) override {
        if (find(observers.begin(), observers.end(), obs) == observers.end())
            observers.push_back(obs);
    }

    void removeObserver(Observer* obs) override {
        observers.erase(remove(observers.begin(), observers.end(), obs),
                        observers.end());
    }

    void notify() override {
        for (auto& obs : observers)
            obs->update();
    }
};

/* ================= SINGLETON SERVICE ================= */
class NotificationService {
private:
    NotificationObservable* observable;
    static NotificationService* instance;

    NotificationService() {
        observable = new NotificationObservable();
    }

public:
    static NotificationService* getInstance() {
        if (!instance)
            instance = new NotificationService();
        return instance;
    }

    NotificationObservable* getObservable() {
        return observable;
    }

    void sendNotification(INotification* noti) {
        observable->setNotification(noti);
    }
};

NotificationService* NotificationService::instance = nullptr;

/* ================= LOGGER (OBSERVER) ================= */
class Logger : public Observer {
private:
    NotificationObservable* observable;
public:
    Logger() {
        observable = NotificationService::getInstance()->getObservable();
        observable->addObserver(this);
    }

    void update() override {
        cout << "LOG: " << observable->getNotification();
    }
};

/* ================= NOTIFICATION ENGINE ================= */
class NotificationEngine : public Observer {
private:
    NotificationObservable* observable;
    vector<INotificationStrategy*> strategies;
public:
    NotificationEngine() {
        observable = NotificationService::getInstance()->getObservable();
        observable->addObserver(this);
    }

    void addNotificationStrategy(INotificationStrategy* s) {
        if (find(strategies.begin(), strategies.end(), s) == strategies.end())
            strategies.push_back(s);
    }

    void update() override {
        string content = observable->getNotification();
        for (auto& s : strategies)
            s->sendNotification(content);
    }
};

/* ================= MAIN ================= */
int main() {
    NotificationService* service = NotificationService::getInstance();

    Logger logger;
    NotificationEngine engine;

    engine.addNotificationStrategy(new EmailNotification("random.person@gmail.com"));
    engine.addNotificationStrategy(new SMSNotification("+91 9876543210"));
    engine.addNotificationStrategy(new PopNotification());

    INotification* notification = new SimpleNotification("Your Order is Shipped");
    notification = new TimeStampDecorator(notification);
    notification = new SignatureDecorator(notification, "Monk");

    service->sendNotification(notification);

    return 0;
}
