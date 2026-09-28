#include<bits/stdc++.h>
using namespace std;

class Product{
private:
    string name;
    string category;
    double price;
public:
    Product(const string& name, const string& category, double price)
        : name(name), category(category), price(price) {}

    string getProductName() const {
        return name;
    }

    void setProductName(const string& newName) {
        name = newName;
    }

    string getProductCategory() const {
        return category;
    }

    void setProductCategory(const string& newCat) {
        category = newCat;
    }

    double getProductPrice() const {
        return price;
    }

    void setProductPrice(double newPrice) {
        price = newPrice;
    }
};

class CartItem{
private:
    Product* prod;
    int qty;
public:
    CartItem(Product* p, int q) : prod(p), qty(q) {}
    
    double getPrice(){
        return prod->getProductPrice() * qty;
    }

    void setQty(int n) {
        qty = n;
    }

    Product* getProduct() const {
        return prod;
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
        originalTotal += item->getPrice();
        currentTotal  += item->getPrice();
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

    vector<CartItem*> getItems() const{
        return items;
    }

    void applyDiscount(double d) {
        currentTotal -= d;
        if (currentTotal < 0) {
            currentTotal = 0;
        }
    }
};

class DiscountStrategy {
public:
    virtual double calculate(double amt) = 0;
    virtual ~DiscountStrategy() = default;
};

class FlatDiscountStrategy : public DiscountStrategy {
private:
    double flatAmt;
public:
    FlatDiscountStrategy(double amt) : flatAmt(amt) {}
    double calculate(double amt) override {
        return min(flatAmt, amt);
    }
};

class PercentDiscountStrategy : public DiscountStrategy {
private:
    double percentage;
public:
    PercentDiscountStrategy(double per) : percentage(per) {}
    double calculate(double amt) override {
        return (percentage / 100) * amt;
    }
};

class PercentWithCapDiscountStrategy : public DiscountStrategy {
private:
    double percentage;
    double cap;
public:
    PercentWithCapDiscountStrategy(double per, double cap) 
        : percentage(per), cap(cap) {}

    double calculate(double amt) override {
        return min((percentage / 100) * amt, cap);
    }
};

enum class StrategyType{
    FLAT,
    PERCENTAGE,
    PERCENTAGE_CAP
};

class DiscountStrategyManager{
public:
    static DiscountStrategyManager& getInstance() {
        static DiscountStrategyManager instance;
        return instance;
    }

    DiscountStrategy* getStrategy(StrategyType type, double param1, double param2= 0){
        if(type == StrategyType::FLAT){
            return new FlatDiscountStrategy(param1);
        }
        if(type == StrategyType::PERCENTAGE){
            return new PercentDiscountStrategy(param1);
        }
        if(type == StrategyType::PERCENTAGE_CAP){
            return new PercentWithCapDiscountStrategy(param1, param2);
        }
        return nullptr;
    }
};

class Coupon {
private:
    Coupon* next;
public:
    Coupon() {
        next = nullptr;
    }

    virtual ~Coupon() {
        if(next)    delete next;
    }   

    void setNextCoupon(Coupon* n) {
        next = n;
    }

    Coupon* getNext() const {
        return next;
    }

    virtual bool isApplicable(Cart* cart) = 0;
    virtual double getDiscount(Cart* cart) = 0;
    virtual bool isCombinable() {
        return true;
    }

    virtual string name() = 0;

    void applyDiscount(Cart* cart) {
        if(isApplicable(cart)){
            double discount = getDiscount(cart);
            cart->applyDiscount(discount);
            cout<< name() << "Applied: " << discount<<endl;
            
            if(!isCombinable()){
                return ;
            }
        }

        if(next){
            next->applyDiscount(cart);
        }
    }
};

class SeasonalOffer : public Coupon {
private:
    double percent;
    string category;
    DiscountStrategy* ds;
public:
    SeasonalOffer(double pct, const string& cat) : percent(pct), category(cat) {
        ds = DiscountStrategyManager::getInstance().getStrategy(StrategyType::PERCENTAGE, percent);
    }

    ~SeasonalOffer() {
        delete ds;
    }

    bool isApplicable(Cart* cart) override {
        for(auto &it: cart->getItems()){
            if(it->getProduct()->getProductCategory() == category){
                return true;
            }
        }
        return false;
    }

    bool isCombinable() override {
        return true;
    }

    double getDiscount(Cart* cart) override {
        double amt = 0;
        for(auto &it: cart->getItems()){
            if(it->getProduct()->getProductCategory() == category){
                amt += it->getPrice();
            }
        }
        return ds->calculate(amt);
    }   

    string name() override {
        return "Seasonal Offer " + to_string((int)percent) + " % off " + category;
    }
};

class LoyaltyDiscount : public Coupon {
private:
    double percent;
    DiscountStrategy* ds;
public:
    LoyaltyDiscount(double pct) : percent(pct) {
        ds = DiscountStrategyManager::getInstance().getStrategy(StrategyType::PERCENTAGE, percent);
    }

    ~LoyaltyDiscount() {
        delete ds;
    }

    bool isApplicable(Cart* cart) override {
        return cart->isLoyaltyMember();
    }

    double getDiscount(Cart* cart) override {
        return ds->calculate(cart->getCurrentTotal());
    }   

    string name() override {
        return "Loyalty Discount " + to_string((int)percent) + "% off";
    }
};

class BulkPurchaseDiscount : public Coupon {
private:
    double threshold;
    double flatOff;
    DiscountStrategy* ds;
public:
    BulkPurchaseDiscount(double thr, double off) : threshold(thr), flatOff(off) {
        ds = DiscountStrategyManager::getInstance().getStrategy(StrategyType::FLAT, flatOff);
    }

    ~BulkPurchaseDiscount() {
        delete ds;
    }

    bool isApplicable(Cart* cart) override {
        return cart->getOriginalTotal() >= threshold;
    }

    double getDiscount(Cart* cart) override {
        return ds->calculate(cart->getCurrentTotal());
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
    DiscountStrategy* ds;
public:
    BankingCoupon(const string& bank, double ms, double percent, double offCap) {
        this->bank = bank;
        this->minSpend = ms;
        this->percent = percent;
        this->offCap = offCap;
        ds = DiscountStrategyManager::getInstance().getStrategy(StrategyType::PERCENTAGE_CAP, percent, offCap);
    }

    ~BankingCoupon() {
        delete ds;
    }

    bool isApplicable(Cart* cart) override {
        return cart->getPaymentBank() == bank and cart->getOriginalTotal() >= minSpend;
    }

    double getDiscount(Cart* cart) override {
        return ds->calculate(cart->getCurrentTotal());
    }   

    string name() override {
        return bank + " Bank Rs " + to_string((int)percent) + " off upto " + to_string((int) offCap);
    }
};

class CouponManager {
private:
    Coupon* head = nullptr;
    mutable mutex mtx;
public:
    static CouponManager& getInstance() {
        static CouponManager instance;
        return instance;
    }

    void registerCoupon(Coupon* coupon) {
        lock_guard<mutex> lock(mtx);
        if(!head)   head = coupon;
        else{
            Coupon* cur = head;
            while(cur->getNext()){
                cur = cur->getNext();
            }
            cur->setNextCoupon(coupon);
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

int main() {
    CouponManager& mgr = CouponManager::getInstance();
    mgr.registerCoupon(new SeasonalOffer(10, "Clothing"));
    mgr.registerCoupon(new LoyaltyDiscount(10));
    mgr.registerCoupon(new BulkPurchaseDiscount(1000, 100));
    mgr.registerCoupon(new BankingCoupon("SBI", 2000, 15, 500));

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
    cart->setPaymentBank("SBI");
    cout << "Original Cart Total: " << cart->getOriginalTotal() << " Rs" << endl;

    vector<string> applicable = mgr.getApplicable(cart);
    cout << "Applicable Coupons:" << endl;
    for (string name : applicable) {
        cout << " - " << name << endl;
    }    

    double finalTotal = mgr.applyAll(cart);
    cout << "Final Cart Total after discounts: " << finalTotal << " Rs" << endl;

    // Cleanup code
    delete p1;
    delete p2;
    delete p3;
    delete p4;
    delete cart;
}