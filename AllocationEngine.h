#ifndef ALLOCATIONENGINE_H
#define ALLOCATIONENGINE_H

#include "Zone.h"
#include "ParkingRequest.h"
#include "RollbackManager.h"

struct AllocationResult {
    bool success;
    int zoneId;
    int slotId;
    bool isCrossZone;
    int penalty;
};

struct ConnectionNode {
    int connectedZone;
    ConnectionNode* next;
    ConnectionNode(int zone) : connectedZone(zone), next(nullptr) {}
};

class AllocationEngine {
private:
    Zone* zones;
    int zoneCount;
    
    struct RequestNode {
        ParkingRequest* request;
        RequestNode* next;
        RequestNode(ParkingRequest* req) : request(req), next(nullptr) {}
    };
    RequestNode* queueFront;
    RequestNode* queueRear;
    int queueSize;
    
    struct AllocationRecord {
        int requestId;
        int zoneId;
        int slotId;
        AllocationRecord* next;
        AllocationRecord(int reqId, int zId, int sId) 
            : requestId(reqId), zoneId(zId), slotId(sId), next(nullptr) {}
    };
    AllocationRecord* allocationStack;
    
    static ConnectionNode* zoneConnections[10];
    
    AllocationResult allocateInZone(int vehicleId, int requestedZoneId);
    AllocationResult allocateCrossZone(int vehicleId, int requestedZoneId);
    
public:
    AllocationEngine(Zone* zonesArray, int count);
    ~AllocationEngine();
    AllocationResult allocate(ParkingRequest* request, RollbackManager& rollbackMgr);
    void enqueueRequest(ParkingRequest* request);
    ParkingRequest* dequeueRequest();
    bool isQueueEmpty() const;
    int getQueueSize() const;
    void pushAllocation(int reqId, int zoneId, int slotId);
    AllocationRecord* popAllocation();
    void addZoneConnection(int zone1, int zone2);
    bool isConnected(int zone1, int zone2) const;
};

#endif