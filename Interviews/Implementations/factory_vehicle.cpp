// https://www.geeksforgeeks.org/design-patterns-set-2-factory-method/
#include <iostream>

/************************* Define vehicles ******************/

// Types of vehicles
enum VehicleType {
    TwoWheelerType,
    ThreeWheelerType,
    FourWheelerType
};

// Interface for vehicles
class Vehicle {
    public:
        virtual void printVehicle() = 0;
        virtual ~Vehicle() {};
};
// Concrete Vehicle classes
class TwoWheeler : public Vehicle {
    public:
        void printVehicle () { std::cout << "I am TwoWheeler" << std::endl; }
};

class ThreeWheeler : public Vehicle {
    public:
        void printVehicle () { std::cout << "I am ThreeWheeler" << std::endl; }
};

class FourWheeler : public Vehicle {
    public:
        void printVehicle () { std::cout << "I am FourWheeler" << std::endl; }
};

/************************* Define Client code ******************/
// A design without factory
// Client code has the logic to instantiate various vehicle types - BAD Design
class Client {
    private:
        Vehicle *pvehicle;
    public:
        Client(VehicleType type) {
            switch (type)
            {
                case TwoWheelerType:
                    pvehicle = new TwoWheeler();
                    break;
                case ThreeWheelerType:
                    pvehicle = new ThreeWheeler();
                    break;
                case FourWheelerType:
                    pvehicle = new FourWheeler();
                    break;
                default:
                    break;
            }
            
        }
        ~Client() {
            if (pvehicle) {
                delete pvehicle;
                pvehicle = NULL;
            }
        }
        Vehicle* getVehicle() {
            return pvehicle;
        }
};

int main() {
    Client *client = new Client(TwoWheelerType);
    Vehicle *vehicle = client->getVehicle();
    vehicle->printVehicle();
    return 0;
}
// Execution: 
// g++ factory.cpp -o factory_cpp.o && ./factory_cpp.o && rm -f factory_cpp.o