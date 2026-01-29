#ifndef PARKINGSYSTEM_H
#define PARKINGSYSTEM_H

#include "Zone.h"
#include "ParkingRequest.h"
#include "AllocationEngine.h"
#include "RollbackManager.h"

class ParkingSystem {
private:
    Zone* zones;
    ParkingRequest** activeRequests;
    ParkingRequest** completedRequests;
    RollbackManager* rollbackMgr;
    AllocationEngine* allocator;
    int zoneCount;
    int maxRequests;
    int activeCount;
    int completedCount;
    int requestIdCounter;
    int* vehicleRegistrations;
    int vehicleCount;
    int maxVehicles;
    
    struct HistoryEntry {
        int requestId;
        int zoneId;
        int duration;
        bool completed;
    };
    HistoryEntry* history;
    int historyCount;
    int historySize;
    
public:
    ParkingSystem(int zCount);
    ~ParkingSystem();
    bool registerVehicle(int vehicleId, int preferredZone);
    int createRequest(int vehicleId, int zoneId, int time);
    bool cancelRequest(int requestId);
    bool releaseParking(int requestId, int time);
    bool rollbackLast();
    bool rollbackK(int k);
    void showStatus() const;
    void analytics() const;
    void addToHistory(int reqId, int zoneId, int duration, bool completed);
    float calculateUtilization(int zoneId) const;
    ParkingRequest* findRequest(int requestId) const;
};

#endif