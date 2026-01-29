#include "Vehicle.h"

Vehicle::Vehicle(int id, int zone) {
    vehicleId = id;
    preferredZone = zone;
}

int Vehicle::getVehicleId() {
    return vehicleId;
}

int Vehicle::getPreferredZone() {
    return preferredZone;
}