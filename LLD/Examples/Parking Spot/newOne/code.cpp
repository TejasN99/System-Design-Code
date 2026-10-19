#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <stdexcept>

using namespace std;

enum class VehicleType {
    BIKE,
    CAR,
    TRUCK
};

class Vehicle {
private:
    string vehicleNumber;
    VehicleType type;
public:
    Vehicle(const string& num, VehicleType t) : vehicleNumber(num), type(t) {}

    string getVehicleNumber() const { return vehicleNumber; }
    VehicleType getType() const { return type; }
};

class ParkingSpot {
private:
    static int nextSpot;
    int spotId;
    int floorId;
    VehicleType type;
    Vehicle* parkedVehicle;
public:
    ParkingSpot(int floor, VehicleType t){
        this->spotId = nextSpot++;
        this->floorId = floor;
        this->type = t;
        this->parkedVehicle = nullptr;
    }

    ~ParkingSpot() {
        // Vehicle lifecycle is managed externally
        parkedVehicle = nullptr;
    }

    bool isSpotAvailable(){
        return !parkedVehicle;
    }

    bool canFitVehicle(VehicleType t) {
        return this->type == t;
    }

    bool parkVehicle(Vehicle* veh) {
        if(!isSpotAvailable()){
            return false;
        }
        parkedVehicle = veh;
        return true;
    }

    bool vacateSpot() {
        if(isSpotAvailable()){
            return false;
        }
        parkedVehicle = nullptr;
        return true;
    }

    VehicleType getVehicleType() const { return type; }
    int getSpotId() const { return spotId; }
    int getFloorId() const { return floorId; }
};

int ParkingSpot::nextSpot = 0;

class ParkingFloor {
private:
    static int nextFloorId;
    int floorId;
    map<int, ParkingSpot*> allSpots;
    map<VehicleType, set<ParkingSpot*>> availableSpots;
public:
    ParkingFloor(){
        this->floorId = nextFloorId++;
    }

    void addParkingSpot(ParkingSpot* spot){
        auto spotId = spot->getSpotId();
        if(allSpots.count(spotId)) {
            return;
        }
        auto type = spot->getVehicleType();
        availableSpots[type].insert(spot);
        allSpots[spotId] = spot;
    }

    int getAvailableCount(VehicleType type) {
        return availableSpots[type].size();
    }

    bool parkVehicle(Vehicle* veh, ParkingSpot* spot) {
        if(availableSpots[spot->getVehicleType()].count(spot)){
            availableSpots[spot->getVehicleType()].erase(spot);
            return spot->parkVehicle(veh);
        }
        return false;
    }

    bool vacateSpot(ParkingSpot* spot) {
        if(!availableSpots[spot->getVehicleType()].count(spot)){
            availableSpots[spot->getVehicleType()].insert(spot);
            return spot->vacateSpot();
        }
        return false;
    }

    int getFloorId() const { return floorId; }

    vector<ParkingSpot*> getTypeSpots(VehicleType tp) {
        auto spotList = availableSpots[tp];
        vector<ParkingSpot*> spots;
        for(auto &it: spotList) spots.push_back(it);
        return spots;
    }
};

int ParkingFloor::nextFloorId = 0;

class ParkingAssignmentStrategy {
public:
    virtual ParkingSpot* findSpot(vector<ParkingFloor*> floors, VehicleType type) = 0;
    virtual ~ParkingAssignmentStrategy() = default;
};

class FirstAvailableAssignment : public ParkingAssignmentStrategy {
public:
    ParkingSpot* findSpot(vector<ParkingFloor*> floors, VehicleType type) override {
        for(auto &floor: floors) {
            if(floor->getAvailableCount(type)){
                auto spots = floor->getTypeSpots(type);
                auto spot = spots[0];
                return spot;
            }
        }
        return nullptr;
    }
};

class Ticket {
private:
    static int nextTicketID;
    int ticketID;
    Vehicle* type;
    ParkingSpot* allocatedSpot;
    time_t entry_time;
    time_t exit_time;
    double totalFare;
    bool isPaid;
public:
    Ticket(Vehicle* t, ParkingSpot* spot){
        this->ticketID = nextTicketID++;
        this->type = t;
        this->allocatedSpot = spot;
        entry_time = time(nullptr);
        exit_time = 0;
        this->totalFare = 0.0;
        this->isPaid = false;
    }

    void markExit() {
        this->exit_time = time(nullptr);
    }

    int getTicketID() const { return ticketID; }
    ParkingSpot* getAllocatedSpot() const { return allocatedSpot; }
    double getTotalFare() const { return totalFare; }
    void setTotalFare(double totalFare_) { totalFare = totalFare_; }

    bool markPaid() {
        this->isPaid = true;
        return true;
    }

    double getDurationHours() {
        if (exit_time == 0) return 0.0;
        double duration = difftime(exit_time, entry_time);
        double hours = ceil(duration / 3600.0);
        return hours;
    }

    bool isCompleted() const {
        return isPaid;
    }

    Vehicle* getVehicle() const { return type; }
};

int Ticket::nextTicketID = 0;

class FareCalculationStrategy {
public:
    virtual double calculateFare(Ticket* ticket) = 0;
    virtual ~FareCalculationStrategy() = default;
};

class HourlyFareStrategy : public FareCalculationStrategy {
private:
    unordered_map<VehicleType, double> hourlyRates;

public:
    HourlyFareStrategy() {
        hourlyRates[VehicleType::BIKE] = 10.0;
        hourlyRates[VehicleType::CAR] = 20.0;
        hourlyRates[VehicleType::TRUCK] = 50.0;
    }

    HourlyFareStrategy(const unordered_map<VehicleType, double>& rates) : hourlyRates(rates) {}

    double calculateFare(Ticket* ticket) override {
        if (!ticket || !ticket->getVehicle()) return 0.0;

        VehicleType type = ticket->getVehicle()->getType();
        double hours = max(1.0, ticket->getDurationHours());
        double rate = hourlyRates.count(type) ? hourlyRates[type] : 20.0;

        return hours * rate;
    }
};

class FlatFareStrategy : public FareCalculationStrategy {
private:
    double flatRate;
public:
    FlatFareStrategy(double rate = 50.0) : flatRate(rate) {}

    double calculateFare(Ticket* ticket) override {
        return flatRate;
    }
};

class ParkingLot {
private:
    vector<ParkingFloor*> floors;
    map<int, Ticket*> allTickets;            // ticketId -> Ticket*
    map<string, Ticket*> activeVehicles;     // vehicleNumber -> Ticket*
    ParkingAssignmentStrategy* assign = nullptr;
    FareCalculationStrategy* fareCal = nullptr;

    ParkingLot() {}

    ParkingFloor* getFloorById(int floorId) {
        for(auto &floor : floors) {
            if(floor->getFloorId() == floorId) return floor;
        }
        return nullptr;
    }

public:
    ParkingLot(const ParkingLot&) = delete;
    ParkingLot& operator=(const ParkingLot&) = delete;

    static ParkingLot& getParkingLotInstance() {
        static ParkingLot instance;
        return instance;
    }

    void addFloor(ParkingFloor* floor) {
        if(find(floors.begin(), floors.end(), floor) == floors.end()){
            floors.push_back(floor);
        }
    }

    void setParkAssign(ParkingAssignmentStrategy* assign_) { assign = assign_; }
    void setFareCal(FareCalculationStrategy* fareCal_) { fareCal = fareCal_; }

    bool isAvailable(VehicleType tp){
        for(auto &floor: floors){
            if(floor->getAvailableCount(tp)){
                return true;
            }
        }
        return false;
    }

    Ticket* parkVehicle(Vehicle* veh){
        if (!veh) throw invalid_argument("Invalid vehicle.");
        if (!assign) throw runtime_error("Parking assignment strategy not set.");

        string vNum = veh->getVehicleNumber();
        if(activeVehicles.count(vNum) && !activeVehicles[vNum]->isCompleted()){
            throw runtime_error("Vehicle is already inside the parking lot.");
        }

        ParkingSpot* spot = assign->findSpot(floors, veh->getType());
        if(!spot) {
            throw runtime_error("Parking is full for this vehicle type.");
        }

        ParkingFloor* floor = getFloorById(spot->getFloorId());
        if(!floor || !floor->parkVehicle(veh, spot)) {
            throw runtime_error("Failed to occupy parking spot.");
        }

        Ticket* ticket = new Ticket(veh, spot);
        allTickets[ticket->getTicketID()] = ticket;
        activeVehicles[vNum] = ticket;

        return ticket;
    }

    // Phase 1: Inquire fare and stamp exit time
    double processExit(int ticketId) {
        if(!allTickets.count(ticketId)) {
            throw runtime_error("Invalid ticket ID.");
        }
        if(!fareCal) {
            throw runtime_error("Fare calculation strategy not set.");
        }

        Ticket* ticket = allTickets[ticketId];
        ticket->markExit();
        double fare = fareCal->calculateFare(ticket);
        ticket->setTotalFare(fare);
        return fare;
    }

    // Phase 2: Settle payment, vacate spot, and mark ticket completed
    bool completePaymentAndRelease(int ticketId) {
        if(!allTickets.count(ticketId)) {
            throw runtime_error("Invalid ticket ID.");
        }

        Ticket* ticket = allTickets[ticketId];
        if(ticket->isCompleted()) {
            return true;
        }

        ParkingSpot* spot = ticket->getAllocatedSpot();
        ParkingFloor* floor = getFloorById(spot->getFloorId());
        if(!floor || !floor->vacateSpot(spot)) {
            return false;
        }

        ticket->markPaid();
        activeVehicles.erase(ticket->getVehicle()->getVehicleNumber());
        return true;
    }

    Ticket* getTicket(int ticketId) {
        if(allTickets.count(ticketId)) return allTickets[ticketId];
        return nullptr;
    }
};

int main() {
    // 1. Get the singleton ParkingLot instance
    ParkingLot& lot = ParkingLot::getParkingLotInstance();

    // 2. Set up Strategies
    FirstAvailableAssignment assignmentStrategy;
    HourlyFareStrategy fareStrategy;

    lot.setParkAssign(&assignmentStrategy);
    lot.setFareCal(&fareStrategy);

    // 3. Create Floors and Spots
    ParkingFloor* floor0 = new ParkingFloor();
    floor0->addParkingSpot(new ParkingSpot(floor0->getFloorId(), VehicleType::BIKE));
    floor0->addParkingSpot(new ParkingSpot(floor0->getFloorId(), VehicleType::CAR));

    ParkingFloor* floor1 = new ParkingFloor();
    floor1->addParkingSpot(new ParkingSpot(floor1->getFloorId(), VehicleType::CAR));
    floor1->addParkingSpot(new ParkingSpot(floor1->getFloorId(), VehicleType::TRUCK));

    lot.addFloor(floor0);
    lot.addFloor(floor1);

    cout << "--- Parking Lot Initialized ---\n";
    cout << "Car spots available: " << lot.isAvailable(VehicleType::CAR) << "\n\n";

    // 4. Vehicles Arrive
    Vehicle car1("MH-04-AB-1234", VehicleType::CAR);
    Vehicle bike1("MH-04-CD-5678", VehicleType::BIKE);

    try {
        // Park Car 1
        Ticket* ticket1 = lot.parkVehicle(&car1);
        cout << "Parked Car: " << car1.getVehicleNumber() 
             << " | Ticket ID: " << ticket1->getTicketID() 
             << " | Floor: " << ticket1->getAllocatedSpot()->getFloorId() 
             << " | Spot: " << ticket1->getAllocatedSpot()->getSpotId() << "\n";

        // Park Bike 1
        Ticket* ticket2 = lot.parkVehicle(&bike1);
        cout << "Parked Bike: " << bike1.getVehicleNumber() 
             << " | Ticket ID: " << ticket2->getTicketID() 
             << " | Floor: " << ticket2->getAllocatedSpot()->getFloorId() 
             << " | Spot: " << ticket2->getAllocatedSpot()->getSpotId() << "\n\n";

        // 5. Exit Flow (Two-Phase) for Car 1
        int carTicketId = ticket1->getTicketID();

        // Phase 1: Inquire Fare
        double fare = lot.processExit(carTicketId);
        cout << "Processing Exit for Ticket " << carTicketId << "\n";
        cout << "Calculated Fare: Rs. " << fare << "\n";

        // Phase 2: Settle and Free Spot
        bool released = lot.completePaymentAndRelease(carTicketId);
        if (released) {
            cout << "Payment confirmed. Spot vacated and vehicle exited successfully.\n\n";
        }

        // 6. Check Availability after Car 1 Exited
        cout << "Car spots available after exit: " << lot.isAvailable(VehicleType::CAR) << "\n";

    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }

    return 0;
}