#include <bits/stdc++.h>
using namespace std;

enum VehicleType {
    CAR,
    BIKE,
    TRUCK
};

class ParkingSpot {
private:
    int id;
    int floorNumber;
    VehicleType type;
    bool occupied;

public:
    ParkingSpot(int id, int floorNumber, VehicleType type)
        : id(id), floorNumber(floorNumber), type(type), occupied(false) {}

    bool isAvailable() const {
        return !occupied;
    }

    void occupy() {
        occupied = true;
    }

    void free() {
        occupied = false;
    }

    int getFloorNumber() const {
        return floorNumber;
    }

    int getId() const {
        return id;
    }

    VehicleType getType() const {
        return type;
    }
};

class Floor {
private:
    int floorNumber;
    vector<ParkingSpot> spots;

public:
    Floor(int floorNumber, vector<ParkingSpot> spots)
        : floorNumber(floorNumber), spots(spots) {}

    ParkingSpot* getAvailableSpot(VehicleType type) {
        for (auto &spot : spots) {
            if (spot.getType() == type && spot.isAvailable()) {
                return &spot;
            }
        }
        return nullptr;
    }
};

class Ticket {
protected:
    string ticketId;
    string vehicleNumber;
    VehicleType vehicleType;
    ParkingSpot* spot;
    time_t entryTime;
    time_t exitTime;

public:
    Ticket(string ticketId,
           string vehicleNumber,
           VehicleType vehicleType,
           ParkingSpot* spot)
        : ticketId(ticketId),
          vehicleNumber(vehicleNumber),
          vehicleType(vehicleType),
          spot(spot)
    {
        entryTime = time(nullptr);
        exitTime = 0;
    }

    virtual double getDiscount() const {
        return 0.0;
    }

    void closeTicket() {
        exitTime = time(nullptr);
    }

    string getTicketID() const {
        return ticketId;
    }

    ParkingSpot* getSpot() const {
        return spot;
    }

    time_t getEntryTime() const { return entryTime; }
    time_t getExitTime() const { return exitTime; }
    VehicleType getVehicleType() const { return vehicleType; }

    virtual ~Ticket() {}
};

class RegularTicket : public Ticket {
public:
    RegularTicket(string id,
                  string vehicleNumber,
                  VehicleType type,
                  ParkingSpot* spot)
        : Ticket(id, vehicleNumber, type, spot) {}
};

class VipTicket : public Ticket {
public:
    VipTicket(string id,
              string vehicleNumber,
              VehicleType type,
              ParkingSpot* spot)
        : Ticket(id, vehicleNumber, type, spot) {}

    double getDiscount() const override {
        return 0.5;
    }
};

class EventTicket : public Ticket {
public:
    EventTicket(string id,
                string vehicleNumber,
                VehicleType type,
                ParkingSpot* spot)
        : Ticket(id, vehicleNumber, type, spot) {}

    double getDiscount() const override {
        return 0.2;
    }
};

enum TicketCategory {
    REGULAR,
    VIP,
    EVENT
};

class TicketFactory {
private:
    static int counter;

    string generateId() {
        return "TICKET_" + to_string(++counter);
    }

public:
    Ticket* createTicket(TicketCategory category,
                         string vehicleNumber,
                         VehicleType type,
                         ParkingSpot* spot) {

        string id = generateId();

        switch (category) {
            case VIP:
                return new VipTicket(id, vehicleNumber, type, spot);
            case EVENT:
                return new EventTicket(id, vehicleNumber, type, spot);
            default:
                return new RegularTicket(id, vehicleNumber, type, spot);
        }
    }
};

int TicketFactory::counter = 0;

class PricingStrategy {
public:
    virtual double calculatePrice(const Ticket& ticket) = 0;
    virtual ~PricingStrategy() {}
};

class TimeBasedStrategy : public PricingStrategy {
public:
    double calculatePrice(const Ticket& ticket) override {
        double durationInSeconds =
            difftime(ticket.getExitTime(), ticket.getEntryTime());

        double hours = durationInSeconds / 3600.0;

        double ratePerHour = 20.0;

        if (ticket.getVehicleType() == TRUCK)
            ratePerHour = 40.0;
        else if (ticket.getVehicleType() == BIKE)
            ratePerHour = 10.0;

        return hours * ratePerHour;
    }
};

class FlatRateStrategy : public PricingStrategy {
public:
    double calculatePrice(const Ticket& ticket) override {
        if (ticket.getVehicleType() == TRUCK)
            return 100.0;
        else if (ticket.getVehicleType() == CAR)
            return 50.0;
        else
            return 20.0;
    }
};

class SpotAssignmentStrategy {
public:
    virtual ParkingSpot* assignSpot(vector<Floor>& floors,
                                    VehicleType type) = 0;
    virtual ~SpotAssignmentStrategy() {}
};

class BasicAssignment : public SpotAssignmentStrategy {
public:
    ParkingSpot* assignSpot(vector<Floor>& floors,
                            VehicleType type) override {
        for (auto &floor : floors) {
            ParkingSpot* spot = floor.getAvailableSpot(type);
            if (spot != nullptr)
                return spot;
        }
        return nullptr;
    }
};

class ParkingLot {
private:
    vector<Floor> floors;
    SpotAssignmentStrategy* assignmentStrategy;
    PricingStrategy* pricingStrategy;
    TicketFactory ticketFactory;

    ParkingLot() {}

public:
    static ParkingLot* getInstance() {
        static ParkingLot instance;
        return &instance;
    }

    void initialize(vector<Floor> floorList,
                    SpotAssignmentStrategy* assignStrat,
                    PricingStrategy* priceStrat) {
        floors = floorList;
        assignmentStrategy = assignStrat;
        pricingStrategy = priceStrat;
    }

    Ticket* parkVehicle(string vehicleNumber,
                        VehicleType type,
                        TicketCategory category) {

        ParkingSpot* spot =
            assignmentStrategy->assignSpot(floors, type);

        if (spot == nullptr)
            return nullptr;

        spot->occupy();

        return ticketFactory.createTicket(
            category, vehicleNumber, type, spot);
    }

    double exitVehicle(Ticket* ticket) {

        ticket->closeTicket();

        ParkingSpot* spot = ticket->getSpot();
        if (spot)
            spot->free();

        double basePrice =
            pricingStrategy->calculatePrice(*ticket);

        return basePrice * (1 - ticket->getDiscount());
    }
};

int main() {

    vector<ParkingSpot> floor1Spots = {
        ParkingSpot(1, 1, CAR),
        ParkingSpot(2, 1, CAR),
        ParkingSpot(3, 1, BIKE)
    };

    vector<ParkingSpot> floor2Spots = {
        ParkingSpot(1, 2, CAR),
        ParkingSpot(2, 2, TRUCK)
    };

    Floor floor1(1, floor1Spots);
    Floor floor2(2, floor2Spots);

    vector<Floor> floors = {floor1, floor2};

    SpotAssignmentStrategy* assignStrat =
        new BasicAssignment();

    PricingStrategy* priceStrat =
        new FlatRateStrategy();

    ParkingLot* lot = ParkingLot::getInstance();

    lot->initialize(floors, assignStrat, priceStrat);

    string vehicle = "MH02AA1527";

    Ticket* ticket =
        lot->parkVehicle(vehicle, BIKE, VIP);

    if (ticket == nullptr) {
        cout << "Parking Full\n";
        return 0;
    }

    cout << "Vehicle Parked: " << vehicle << endl;

    this_thread::sleep_for(chrono::seconds(2));

    double price = lot->exitVehicle(ticket);

    cout << "Final Price: " << price << endl;

    delete ticket;
    delete assignStrat;
    delete priceStrat;

    return 0;
}
