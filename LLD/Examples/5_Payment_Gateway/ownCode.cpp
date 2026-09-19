#include<bits/stdc++.h>
using namespace std;

class PaymentRequest{
public:
    string sender;
    string reciever;
    double amt;
    string currency;

    PaymentRequest(const string& sender, const string& rec, double amt, const string& currency){
        this->sender = sender;
        this->reciever = rec;
        this->amt = amt;
        this->currency = currency;
    }
};

class BankingSystem {
public:
    virtual bool processPayment(double amt) = 0;
    virtual ~BankingSystem() = default;
};

class PaytmBankingSystem : public BankingSystem {
public:
    bool processPayment(double amt) override {
        int r = rand() % 100;
        return r < 80;
    }
};

class RazorpayBankingSystem : public BankingSystem {
public:
    bool processPayment(double amt) override {
        int r = rand() % 100;
        return r < 90;
    }
};

class PaymentGateway{
protected:
    BankingSystem* bankingSystem;
public:
    PaymentGateway() {
        bankingSystem = nullptr;
    }

    virtual ~PaymentGateway() {
        delete bankingSystem;
    }

    // Template method defining the standard payment flow
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

    // Steps to be implemented by concrete gateways
    virtual bool validatePayment(const PaymentRequest* request) = 0;
    virtual bool initiatePayment(const PaymentRequest* request) = 0;
    virtual bool confirmPayment(const PaymentRequest* request) = 0;
};

class PaytmGateway : public PaymentGateway {
public:
    PaytmGateway() {
        bankingSystem = new PaytmBankingSystem();
    }

    bool validatePayment(const PaymentRequest* request) override {
        cout << "[Paytm] Validating payment for " << request->sender << ".\n";

        if (request->amt <= 0 || request->currency != "INR") {
            return false;
        }
        return true;
    }

    bool initiatePayment(const PaymentRequest* request) override {
        cout << "[Paytm] Initiating payment of " << request->amt 
                  << " " << request->currency << " for " << request->sender << ".\n";

        return bankingSystem->processPayment(request->amt);
    }

    bool confirmPayment(const PaymentRequest* request) override {
        cout << "[Paytm] Confirming payment for " << request->sender << ".\n";
        return true;
    }
};

class RazorpayGateway : public PaymentGateway {
public:
    RazorpayGateway() {
        bankingSystem = new RazorpayBankingSystem();
    }
    bool validatePayment(const PaymentRequest* request) override {
        cout << "[Razorpay] Validating payment for " << request->sender << ".\n";

        if (request->amt <= 0) {
            return false;
        }
        return true;
    }
    bool initiatePayment(const PaymentRequest* request) override {
        cout << "[Razorpay] Initiating payment of " << request->amt 
                  << " " << request->currency << " for " << request->sender << ".\n";

        return bankingSystem->processPayment(request->amt);
       
    }
    bool confirmPayment(const PaymentRequest* request) override {
        cout << "[Razorpay] Confirming payment for " << request->sender << ".\n";

        // Confirmation always succeeds in this simulation
        return true;
    }
};

class PaymentGatewayProxy : public PaymentGateway {
private:
    PaymentGateway* realGateway;
    int retries;
public:
    PaymentGatewayProxy(PaymentGateway* gateway, int maxretries) : 
        realGateway(gateway), retries(maxretries) {}

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

    bool validatePayment(const PaymentRequest* request) override {
        return realGateway->validatePayment(request);
    }
    bool initiatePayment(const PaymentRequest* request) override {
        return realGateway->initiatePayment(request);
    }
    bool confirmPayment(const PaymentRequest* request) override {
        return realGateway->confirmPayment(request);
    }
};

enum class GatewayType { 
    PAYTM, 
    RAZORPAY
};

class GatewayFactory {
private:
    // Private constructor and delete copy/assignment to ensure no one can clone or reassign your singleton.
    GatewayFactory() {}
    GatewayFactory(const GatewayFactory&) = delete;
    GatewayFactory& operator=(const GatewayFactory&) = delete;
    
public:
    static GatewayFactory& getInstance() {
        static GatewayFactory instance;
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

class PaymentService{
private:    
    PaymentGateway* gateway;
    PaymentService() { 
        gateway = nullptr; 
    }
    ~PaymentService() { 
        delete gateway; 
    }
public:
    static PaymentService& getInstance() {
        static PaymentService instance;
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

class PaymentController {
private:
    PaymentController() {}
    PaymentController(const PaymentController&) = delete;
    PaymentController& operator=(const PaymentController&) = delete;
public:
    static PaymentController& getInstance() {
        static PaymentController instance;
        return instance;
    }
    bool handlePayment(GatewayType type, PaymentRequest* req) {
        PaymentGateway* paymentGateway = GatewayFactory::getInstance().getGateway(type);
        PaymentService::getInstance().setGateway(paymentGateway);
        return PaymentService::getInstance().processPayment(req);
    }
};

int main() {
    srand((unsigned)time(nullptr));

    PaymentRequest req("Tejas", "Darshan", 1000, "INR");
    cout << "Processing via Paytm\n";
    cout << "------------------------------\n";

    bool res1 = PaymentController::getInstance().handlePayment(GatewayType::PAYTM, &req);
    cout << "Result: " << (res1 ? "SUCCESS" : "FAIL") << "\n";
    cout << "------------------------------\n\n";

    PaymentRequest req2("Darshan", "Vishal", 500.0, "USD");

    cout << "Processing via Razorpay\n";
    cout << "------------------------------\n";
    bool res2 = PaymentController::getInstance().handlePayment(GatewayType::RAZORPAY, &req2);
    cout << "Result: " << (res2 ? "SUCCESS" : "FAIL") << "\n";
    cout << "------------------------------\n";    
}