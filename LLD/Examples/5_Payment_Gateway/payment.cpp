#include<bits/stdc++.h>
using namespace std;

class PaymentRequest {
public:
    string sender;
    string rec;
    double amount;
    string currency;
    
    PaymentRequest(const string &sender, const string &rec, double amt, const string &cur){
        this->sender = sender;
        this->rec = rec;
        this->amount = amt;
        this->currency = cur;
    }
};

class BankingSystem{
public:
    virtual bool processPayment(double amt) = 0;
    virtual ~BankingSystem() {};
};

class PaytmBanking : public BankingSystem{
public:
    bool processPayment(double amt) override {
        int r = rand() % 100;
        return r < 80;
    }
};

class RazorBanking : public BankingSystem{
public:
    bool processPayment(double amt) override {
        int r = rand() % 100;
        return r < 90;
    }
};

class PaymentGateway {
protected:
    BankingSystem* bankingSystem;
public:
    PaymentGateway(){
        this->bankingSystem = nullptr;
    }
    virtual ~PaymentGateway(){
        delete bankingSystem;
    }

    virtual bool processPayment(PaymentRequest* request) {
        if (!validatePayment(request)) {
            cout << "[PaymentGateway] Validation failed for " << request->sender << ".\n";
            return false;
        }
        if (!initiatePayment(request)) {
            cout << "[PaymentGateway] Initiation failed for " << request->sender << ".\n";
            return false;
        }
        if (!confirmPayment(request)) {
            cout << "[PaymentGateway] Confirmation failed for " << request->sender << ".\n";
            return false;
        }
        return true;
    }

    virtual bool validatePayment(PaymentRequest* request) = 0;
    virtual bool initiatePayment(PaymentRequest* request) = 0;
    virtual bool confirmPayment(PaymentRequest* request) = 0;
};

class PaytmGateway : public PaymentGateway {
public:
    PaytmGateway(){
        this->bankingSystem = new PaytmBanking();
    }
    
    bool validatePayment(PaymentRequest* request) override {
        cout << "[Paytm] Validating payment for " << request->sender << ".\n";
        if (request->amount <= 0 || request->currency != "INR") {
            return false;
        }
        return true;
    }
    
    bool initiatePayment(PaymentRequest* request) override {
        cout << "[Paytm] Initiating payment of " << request->amount 
                  << " " << request->currency << " for " << request->sender << ".\n";
        return bankingSystem->processPayment(request->amount);
    }
    
    bool confirmPayment(PaymentRequest* request) override {
        cout << "[Paytm] Confirming payment for " << request->sender << ".\n";
        return true;
    }
};


class RazorpayGateway : public PaymentGateway {
public:
    RazorpayGateway() {
        bankingSystem = new RazorBanking();
    }
    bool validatePayment(PaymentRequest* request) override {
        cout << "[Razorpay] Validating payment for " << request->sender << ".\n";
        if (request->amount <= 0) {
            return false;
        }
        return true;
    }
    bool initiatePayment(PaymentRequest* request) override {
        cout << "[Razorpay] Initiating payment of " << request->amount 
                  << " " << request->currency << " for " << request->sender << ".\n";
        return bankingSystem->processPayment(request->amount);
       
    }
    bool confirmPayment(PaymentRequest* request) override {
        cout << "[Razorpay] Confirming payment for " << request->sender << ".\n";
        return true;
    }
};

class PaymentGatewayProxy : public PaymentGateway {
private:
    PaymentGateway* realGateway;
    int retries;
public:
    PaymentGatewayProxy(PaymentGateway* gateway, int maxRetries) {
        realGateway = gateway;
        retries = maxRetries;
    }
    ~PaymentGatewayProxy() {
        delete realGateway;
    }

    bool processPayment(PaymentRequest* request) override {
        bool result = false;
        for (int attempt = 0; attempt < retries; ++attempt) {
            if (attempt > 0) {
                cout << "[Proxy] Retrying payment (attempt " << (attempt+1)
                          << ") for " << request->sender << ".\n";
            }
            result = realGateway->processPayment(request);
            if (result) break;
        }
        if (!result) {
            cout << "[Proxy] Payment failed after " << (retries)
                      << " attempts for " << request->sender << ".\n";
        }
        return result;
    }

    bool validatePayment(PaymentRequest* request) override {
        return realGateway->validatePayment(request);
    }
    bool initiatePayment(PaymentRequest* request) override {
        return realGateway->initiatePayment(request);
    }
    bool confirmPayment(PaymentRequest* request) override {
        return realGateway->confirmPayment(request);
    }
};

enum class GatewayType { 
    PAYTM, 
    RAZORPAY
};

class GatewayFactory {
private:
    static GatewayFactory* instance;
public:
    static GatewayFactory* getInstance() {
        if(!instance)
            instance = new GatewayFactory();
        return instance;
    }
    PaymentGateway* getGateway(GatewayType type) {
        if (type == GatewayType::PAYTM) {
            PaymentGateway* paymentGateway = new PaytmGateway();
            return new PaymentGatewayProxy(paymentGateway, 3);
        } else {
            PaymentGateway* paymentGateway = new RazorpayGateway();
            return new PaymentGatewayProxy(paymentGateway, 1);
        }
    }
};

GatewayFactory* GatewayFactory::instance = nullptr;

class PaymentService {
private:
    static PaymentService* instance;
    PaymentGateway* gateway;

    PaymentService() { 
        gateway = nullptr; 
    }
    ~PaymentService() { 
        delete gateway; 
    }
public:
public:
    static PaymentService* getInstance() {
        if(!instance)
            instance = new PaymentService();
        return instance;
    }
    void setGateway(PaymentGateway* g) {
        if (gateway) delete gateway;
        gateway = g;
    }
    bool processPayment(PaymentRequest* request) {
        if (!gateway) {
            cout << "[PaymentService] No payment gateway selected.\n";
            return false;
        }
        return gateway->processPayment(request);
    }
};

PaymentService* PaymentService::instance = nullptr;

class PaymentController {
private:
    static PaymentController* instance;
    PaymentController() {}
public:
    static PaymentController* getInstance() {
        if(!instance)
            instance = new PaymentController();
        return instance;
    }
    bool handlePayment(GatewayType type, PaymentRequest* req) {
        PaymentGateway* paymentGateway = GatewayFactory::getInstance()->getGateway(type);
        PaymentService::getInstance()->setGateway(paymentGateway);
        return PaymentService::getInstance()->processPayment(req);
    }
};

PaymentController* PaymentController::instance = nullptr;

int main() {

    srand(static_cast<unsigned>(time(nullptr)));

    PaymentRequest* req1 = new PaymentRequest("Aditya", "Shubham", 1000.0, "INR");

    cout << "Processing via Paytm\n";
    cout << "------------------------------\n";
    bool res1 = PaymentController::getInstance()->handlePayment(GatewayType::PAYTM, req1);
    cout << "Result: " << (res1 ? "SUCCESS" : "FAIL") << "\n";
    cout << "------------------------------\n\n";

    PaymentRequest* req2 = new PaymentRequest("Shubham", "Aditya", 500.0, "USD");

    cout << "Processing via Razorpay\n";
    cout << "------------------------------\n";
    bool res2 = PaymentController::getInstance()->handlePayment(GatewayType::RAZORPAY, req2);
    cout << "Result: " << (res2 ? "SUCCESS" : "FAIL") << "\n";
    cout << "------------------------------\n";

    return 0;
}