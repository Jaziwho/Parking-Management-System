#include <iostream>
#include "AllocationEngine.h"
using namespace std;

ConnectionNode* AllocationEngine::zoneConnections[10] = {nullptr};

AllocationEngine::AllocationEngine(Zone* zonesArray, int count) {
    zones = zonesArray;
    zoneCount = count;
    queueFront = nullptr;
    queueRear = nullptr;
    queueSize = 0;
    allocationStack = nullptr;
    for (int i = 0; i < zoneCount - 1; i++) addZoneConnection(i, i + 1);
}

AllocationEngine::~AllocationEngine() {
    while (queueFront != nullptr) {
        RequestNode* temp = queueFront;
        queueFront = queueFront->next;
        delete temp;
    }
    while (allocationStack != nullptr) {
        AllocationRecord* temp = allocationStack;
        allocationStack = allocationStack->next;
        delete temp;
    }
}

AllocationResult AllocationEngine::allocate(ParkingRequest* request, RollbackManager& rollbackMgr) {
    AllocationResult result;
    result.success = false;
    result.isCrossZone = false;
    result.penalty = 0;
    
    result = allocateInZone(request->getVehicleId(), request->getRequestedZoneId());
    
    if (!result.success) {
        result = allocateCrossZone(request->getVehicleId(), request->getRequestedZoneId());
        if (result.success) {
            result.isCrossZone = true;
            result.penalty = 10;
        }
    }
    
    if (result.success) {
        request->allocate(result.zoneId, result.slotId);
        pushAllocation(request->getRequestId(), result.zoneId, result.slotId);
        
        RollbackAction action;
        action.requestId = request->getRequestId();
        action.zoneId = result.zoneId;
        action.slotId = result.slotId;
        action.previousState = REQUESTED;
        action.newState = ALLOCATED;
        
        rollbackMgr.push(action);
    }
    
    return result;
}

AllocationResult AllocationEngine::allocateInZone(int vehicleId, int requestedZoneId) {
    AllocationResult result;
    result.success = false;
    
    if (requestedZoneId >= 0 && requestedZoneId < zoneCount) {
        ParkingSlot* slot = zones[requestedZoneId].getAvailableSlot();
        if (slot != nullptr) {
            slot->occupy();
            result.success = true;
            result.zoneId = requestedZoneId;
            result.slotId = slot->getSlotId();
        }
    }
    
    return result;
}

AllocationResult AllocationEngine::allocateCrossZone(int vehicleId, int requestedZoneId) {
    AllocationResult result;
    result.success = false;
    
    ConnectionNode* current = zoneConnections[requestedZoneId];
    while (current != nullptr) {
        int connectedZone = current->connectedZone;
        if (connectedZone >= 0 && connectedZone < zoneCount) {
            ParkingSlot* slot = zones[connectedZone].getAvailableSlot();
            if (slot != nullptr) {
                slot->occupy();
                result.success = true;
                result.zoneId = connectedZone;
                result.slotId = slot->getSlotId();
                break;
            }
        }
        current = current->next;
    }
    
    return result;
}

void AllocationEngine::enqueueRequest(ParkingRequest* request) {
    RequestNode* newNode = new RequestNode(request);
    if (queueRear == nullptr) queueFront = queueRear = newNode;
    else {
        queueRear->next = newNode;
        queueRear = newNode;
    }
    queueSize++;
}

ParkingRequest* AllocationEngine::dequeueRequest() {
    if (queueFront == nullptr) return nullptr;
    RequestNode* temp = queueFront;
    ParkingRequest* request = temp->request;
    queueFront = queueFront->next;
    if (queueFront == nullptr) queueRear = nullptr;
    delete temp;
    queueSize--;
    return request;
}

bool AllocationEngine::isQueueEmpty() const {
    return queueFront == nullptr;
}

int AllocationEngine::getQueueSize() const {
    return queueSize;
}

void AllocationEngine::pushAllocation(int reqId, int zoneId, int slotId) {
    AllocationRecord* newRecord = new AllocationRecord(reqId, zoneId, slotId);
    newRecord->next = allocationStack;
    allocationStack = newRecord;
}

AllocationEngine::AllocationRecord* AllocationEngine::popAllocation() {
    if (allocationStack == nullptr) return nullptr;
    AllocationRecord* top = allocationStack;
    allocationStack = allocationStack->next;
    return top;
}

void AllocationEngine::addZoneConnection(int zone1, int zone2) {
    ConnectionNode* newNode1 = new ConnectionNode(zone2);
    newNode1->next = zoneConnections[zone1];
    zoneConnections[zone1] = newNode1;
    
    ConnectionNode* newNode2 = new ConnectionNode(zone1);
    newNode2->next = zoneConnections[zone2];
    zoneConnections[zone2] = newNode2;
}

bool AllocationEngine::isConnected(int zone1, int zone2) const {
    ConnectionNode* current = zoneConnections[zone1];
    while (current != nullptr) {
        if (current->connectedZone == zone2) return true;
        current = current->next;
    }
    return false;
}