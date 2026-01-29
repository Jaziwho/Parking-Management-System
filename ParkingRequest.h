#ifndef PARKINGREQUEST_H
#define PARKINGREQUEST_H

enum RequestState {
    REQUESTED,
    ALLOCATED,
    OCCUPIED,
    RELEASED,
    CANCELLED
};

class ParkingRequest {
private:
    int requestId;
    int vehicleId;
    int requestedZoneId;
    int allocatedZoneId;
    int slotId;
    RequestState state;
    int requestTime;
    int allocationTime;
    int occupiedTime;
    int releaseTime;
    bool isValidTransition(RequestState newState) const;
    
public:
    ParkingRequest(int reqId, int vehId, int zoneId, int time);
    bool allocate(int zoneId, int slotId);
    bool occupy(int time);
    bool release(int time);
    bool cancel();
    int getRequestId() const;
    int getVehicleId() const;
    int getRequestedZoneId() const;
    int getZoneId() const;
    int getSlotId() const;
    RequestState getState() const;
    int getDuration() const;
    void printInfo() const;
    bool isActive() const;
    bool isCompleted() const;
    bool isCancelled() const;
};

#endif