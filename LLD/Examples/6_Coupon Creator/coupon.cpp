#include<bits/stdc++.h>
using namespace std;

// Discount Startegy

class DiscountStartegy{
public:
    virtual double calculate(double amt) = 0;
    virtual ~DiscountStartegy() {};
};

class FlatDiscountStrategy : public DiscountStartegy{
private:
    double amt;
public:
    FlatDiscountStrategy(double amt){
        this->amt = amt;
    }

    double calculate(double baseAmt) override{
        return min(this->amt, baseAmt);
    }
};

class PercentageDiscountStrategy : public DiscountStartegy{
private:
    double percent;
public:
    PercentageDiscountStrategy(double per){
        this->percent = per;
    }

    double calculate(double baseAmt) override{
        return (percent / 100) * baseAmt;
    }
};

class PercentageDiscountCapStrategy : public DiscountStartegy{
private:
    double percent;
    double cap;
public:
    PercentageDiscountCapStrategy(double per, double cap){
        this->percent = per;
        this->cap = cap;
    }

    double calculate(double baseAmt) override{
        return min((percent / 100) * baseAmt, cap);
    }
};

enum StrategyType {
    FLAT,
    PERCENT,
    PERCENT_WITH_CAP
};

class DiscountStrategyManager{
private:
    static DiscountStrategyManager* instance;
public:
    static DiscountStrategyManager* getInstance(){
        if(!instance)
            instance = new DiscountStrategyManager();
        return instance;
    }

    DiscountStartegy* getStrategy(StrategyType type, double param1, double param2=0) const{
        if(type == StrategyType::FLAT){
            return new FlatDiscountStrategy(param1);
        }
        else if(type == StrategyType::PERCENT){
            return new PercentageDiscountStrategy(param1);
        }
        else if(type == StrategyType::PERCENT_WITH_CAP){
            return new PercentageDiscountCapStrategy(param1, param2);
        }
        else{
            return nullptr;
        }
    }
};

DiscountStrategyManager* DiscountStrategyManager::instance = nullptr;


// Product Heirarchy

class Product{
private:
    string name;
    string category;
    double price;
public:
    Product(string name, string category, double price){
        this->name = name;
        this->category = category;
        this->price = price;
    }

    string getName(){
        return this->name;
    }

    string getCategory() const {
        return category;
    }
    double getPrice() {
        return price;
    }
};

class CartItem{
private:
    Product* product;
    int qty;
public:
    CartItem(Product* p, int qty){
        this->product = p;
        this->qty = qty;
    }

    double itemTotal(){
        return this->product->getPrice() * qty;
    }
    
    Product* getProduct(){
        return this->product;
    }
};


class Cart{
private:
    vector<CartItem*> items;
    double originalTotal;
    double currentTotal;
    bool loyaltyMember;
    string paymentBank;
public:
    Cart() {
        originalTotal = 0.0;
        currentTotal = 0.0;
        loyaltyMember = false;
        paymentBank = "";
    }

    void addProduct(Product* prod, int qty = 1) {
        CartItem* item = new CartItem(prod, qty);
        items.push_back(item);
        originalTotal += item->itemTotal();
        currentTotal  += item->itemTotal();
    }

    double getOriginalTotal() {
        return originalTotal;
    }

    double getCurrentTotal() {
        return currentTotal;
    }

    void setLoyaltyMember(bool member) {
        loyaltyMember = member;
    }

    bool isLoyaltyMember() {
        return loyaltyMember;
    }

    void setPaymentBank(string bank) {
        paymentBank = bank;
    }

    string getPaymentBank() {
        return paymentBank;
    }

    vector<CartItem*> getItems() {
        return items;
    }

    void applyDiscount(double d) {
        currentTotal -= d;
        if (currentTotal < 0) {
            currentTotal = 0;
        }
    }
};


// Coupon Heirarchy

class Coupon{
private:
    Coupon* next;
public:
    Coupon(){
        this->next = nullptr;
    }

    virtual ~Coupon(){
        if(next)
            delete next;
    }

    void setNext(Coupon* nxt) {
        next = nxt;
    }
    Coupon* getNext() {
        return next;
    }

    virtual bool isApplicable(Cart* cart) = 0;
    virtual double getDiscount(Cart* cart) = 0;
    virtual bool isCombinable() {
        return true;
    }
    virtual string name() = 0;

    void applyDiscount(Cart* cart){
        if(isApplicable(cart)){
            double discount = getDiscount(cart);
            cart->applyDiscount(discount);
            cout<< name() << "Applied: " << discount<<endl;
            if(!isCombinable())
                return ;
        }
        if(this->next){
            next->applyDiscount(cart);
        }
    }
};

class SeasonalOffer : public Coupon {
private:
    double percent;
    string category;
    DiscountStartegy* strat;
public:
    SeasonalOffer(double pct, string cat) {
        this->percent = pct;
        this->category = cat;
        this->strat = DiscountStrategyManager::getInstance()->getStrategy(StrategyType::PERCENT, percent);
    }
    
    ~SeasonalOffer(){
        delete strat;
    }

    bool isApplicable(Cart* cart) override{
        for(auto &it: cart->getItems()){
            if(it->getProduct()->getCategory() == this->category)
                return true;
        }
        return false;
    }

    bool isCombinable() override {
        return true;
    }

    double getDiscount(Cart* cart) override {
        double amt = 0;
        for(auto &it: cart->getItems()){
            if(it->getProduct()->getCategory() == this->category){
                amt += it->itemTotal();
            }
        }
        return strat->calculate(amt);
    }
    string name() override {
        return "Seasonal Offer " + to_string((int)percent) + " % off " + category;
    }
};

class LoyaltyDiscount : public Coupon {
private:
    double percent;
    DiscountStartegy* strat;
public:
    LoyaltyDiscount(double pct) {
        percent = pct;
        strat = DiscountStrategyManager::getInstance()->getStrategy(StrategyType::PERCENT, percent);
    }

    ~LoyaltyDiscount() {
        delete strat;
    }

    bool isApplicable(Cart* cart) override {
        return cart->isLoyaltyMember();
    }

    double getDiscount(Cart* cart) override{
        return strat->calculate(cart->getCurrentTotal());
    }

    string name() override {
        return "Loyalty Discount " + to_string((int)percent) + "% off";
    }
};

class BulkPurchaseDiscount : public Coupon {
private:
    double threshold;
    double flatOff;
    DiscountStartegy* strat;
public:
    BulkPurchaseDiscount(double thr, double off) {
        threshold = thr;
        flatOff = off;
        strat = DiscountStrategyManager::getInstance()->getStrategy(StrategyType::FLAT, flatOff);
    }
    ~BulkPurchaseDiscount() {
        delete strat;
    }

    bool isApplicable(Cart* cart) override {
        return cart->getOriginalTotal() >= threshold;
    }
    double getDiscount(Cart* cart) override {
        return strat->calculate(cart->getCurrentTotal());
    }

    string name() override {
        return "Bulk Purchase Rs " + to_string((int)flatOff) + " off over "
             + to_string((int)threshold);
    }
};

class BankingCoupon : public Coupon {
private:
    string bank;
    double minSpend;
    double percent;
    double offCap;
    DiscountStartegy* strat;
public:
    BankingCoupon(const string& b, double ms, double percent, double offCap) {
        bank = b;
        minSpend = ms;
        this->percent = percent;
        this->offCap = percent;
        strat = DiscountStrategyManager::getInstance()->getStrategy(StrategyType::PERCENT_WITH_CAP, percent, offCap);
    }
    ~BankingCoupon() {
        delete strat;
    }

    bool isApplicable(Cart* cart) override {
        return cart->getPaymentBank() == bank
            && cart->getOriginalTotal() >= minSpend;
    }
    double getDiscount(Cart* cart) override {
        return strat->calculate(cart->getCurrentTotal());
    }
    string name() override {
        return bank + " Bank Rs " + to_string((int)percent) + " off upto " + to_string((int) offCap);
    }
};

// CouponManager (Singleton)

class CouponManager{
private:
    static CouponManager* instance;
    Coupon* head;
    mutable mutex mtx;
    CouponManager() {
        head = nullptr;
    }
public:
    static CouponManager* getInstance() {
        if (!instance) {
            instance = new CouponManager();
        }
        return instance;
    }

    void registerCoupon(Coupon* coupon) {
        lock_guard<mutex> lock(mtx);
        if (!head) {
            head = coupon;
        } else {
            Coupon* cur = head;
            while (cur->getNext()) {
                cur = cur->getNext();
            }
            cur->setNext(coupon);
        }
    }

    vector<string> getApplicable(Cart* cart) const {
        lock_guard<mutex> lock(mtx);
        vector<string> res;
        Coupon* cur = head;
        while (cur) {
            if (cur->isApplicable(cart)) {
                res.push_back(cur->name());
            }
            cur = cur->getNext();
        }
        return res;
    }

    double applyAll(Cart* cart) {
        lock_guard<mutex> lock(mtx);
        if (head) {
            head->applyDiscount(cart);
        }
        return cart->getCurrentTotal();
    }
};

CouponManager* CouponManager::instance = nullptr;

int main() {
    CouponManager* mgr = CouponManager::getInstance();
    mgr->registerCoupon(new SeasonalOffer(10, "Clothing"));
    mgr->registerCoupon(new LoyaltyDiscount(5));
    mgr->registerCoupon(new BulkPurchaseDiscount(1000, 100));
    mgr->registerCoupon(new BankingCoupon("ABC", 2000, 15, 500));

    Product* p1 = new Product("Winter Jacket", "Clothing", 1000);
    Product* p2 = new Product("Smartphone", "Electronics", 20000);
    Product* p3 = new Product("Jeans", "Clothing", 1000);
    Product* p4 = new Product("Headphones", "Electronics", 2000);

    Cart* cart = new Cart();
    cart->addProduct(p1, 1);
    cart->addProduct(p2, 1);
    cart->addProduct(p3, 2);
    cart->addProduct(p4, 1);

    cart->setLoyaltyMember(true);
    cart->setPaymentBank("ABC");

    cout << "Original Cart Total: " << cart->getOriginalTotal() << " Rs" << endl;
    vector<string> applicable = mgr->getApplicable(cart);
    cout << "Applicable Coupons:" << endl;
    for (string name : applicable) {
        cout << " - " << name << endl;
    }

    double finalTotal = mgr->applyAll(cart);
    cout << "Final Cart Total after discounts: " << finalTotal << " Rs" << endl;

    // Cleanup code
    delete p1;
    delete p2;
    delete p3;
    delete p4;
    delete cart;
}