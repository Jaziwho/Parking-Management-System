#ifndef VEHICLE_H
#define VEHICLE_H

class Vehicle {
private:
    int vehicleId;
    int preferredZone;
    
public:
    Vehicle(int id = -1, int zone = -1);
    int getVehicleId();
    int getPreferredZone();
};

#endif