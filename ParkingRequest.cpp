#include <iostream>
#include "ParkingRequest.h"
using namespace std;

ParkingRequest::ParkingRequest(int reqId, int vehId, int zoneId, int time) {
    requestId = reqId;
    vehicleId = vehId;
    requestedZoneId = zoneId;
    allocatedZoneId = -1;
    slotId = -1;
    state = REQUESTED;
    requestTime = time;
    allocationTime = -1;
    occupiedTime = -1;
    releaseTime = -1;
}

bool ParkingRequest::isValidTransition(RequestState newState) const {
    switch (state) {
        case REQUESTED: return newState == ALLOCATED || newState == CANCELLED;
        case ALLOCATED: return newState == OCCUPIED || newState == CANCELLED;
        case OCCUPIED: return newState == RELEASED;
        case RELEASED: return false;
        case CANCELLED: return false;
    }
    return false;
}

bool ParkingRequest::allocate(int zoneId, int slotId) {
    if (!isValidTransition(ALLOCATED)) return false;
    allocatedZoneId = zoneId;
    this->slotId = slotId;
    state = ALLOCATED;
    return true;
}

bool ParkingRequest::occupy(int time) {
    if (!isValidTransition(OCCUPIED)) return false;
    occupiedTime = time;
    state = OCCUPIED;
    return true;
}

bool ParkingRequest::release(int time) {
    if (!isValidTransition(RELEASED)) return false;
    releaseTime = time;
    state = RELEASED;
    return true;
}

bool ParkingRequest::cancel() {
    if (!isValidTransition(CANCELLED)) return false;
    state = CANCELLED;
    return true;
}

int ParkingRequest::getRequestId() const { return requestId; }
int ParkingRequest::getVehicleId() const { return vehicleId; }
int ParkingRequest::getRequestedZoneId() const { return requestedZoneId; }
int ParkingRequest::getZoneId() const { return allocatedZoneId; }
int ParkingRequest::getSlotId() const { return slotId; }
RequestState ParkingRequest::getState() const { return state; }

int ParkingRequest::getDuration() const {
    if (state == RELEASED && occupiedTime != -1 && releaseTime != -1) return releaseTime - occupiedTime;
    return 0;
}

void ParkingRequest::printInfo() const {
    cout << "Request " << requestId 
         << " - Vehicle " << vehicleId
         << " - Zone " << requestedZoneId;
    if (allocatedZoneId != -1) cout << " (Allocated to Zone " << allocatedZoneId << ", Slot " << slotId << ")";
    cout << " - State: ";
    switch(state) {
        case REQUESTED: cout << "REQUESTED"; break;
        case ALLOCATED: cout << "ALLOCATED"; break;
        case OCCUPIED: cout << "OCCUPIED"; break;
        case RELEASED: cout << "RELEASED"; break;
        case CANCELLED: cout << "CANCELLED"; break;
    }
    if (state == RELEASED && getDuration() > 0) cout << " (Duration: " << getDuration() << " min)";
    cout << endl;
}

bool ParkingRequest::isActive() const { return state == REQUESTED || state == ALLOCATED || state == OCCUPIED; }
bool ParkingRequest::isCompleted() const { return state == RELEASED; }
bool ParkingRequest::isCancelled() const { return state == CANCELLED; }