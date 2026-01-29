#include <iostream>
#include <iomanip>
#include "ParkingSystem.h"
using namespace std;

ParkingSystem::ParkingSystem(int zCount) {
    zoneCount = zCount;
    zones = new Zone[zoneCount];
    for (int i = 0; i < zoneCount; i++) zones[i] = Zone(i, (i + 1) * 2);
    
    maxRequests = 100;
    activeRequests = new ParkingRequest*[maxRequests];
    completedRequests = new ParkingRequest*[maxRequests];
    activeCount = completedCount = 0;
    requestIdCounter = 1;
    
    maxVehicles = 100;
    vehicleRegistrations = new int[maxVehicles];
    vehicleCount = 0;
    
    historySize = 50;
    history = new HistoryEntry[historySize];
    historyCount = 0;
    
    rollbackMgr = new RollbackManager();
    allocator = new AllocationEngine(zones, zoneCount);
}

ParkingSystem::~ParkingSystem() {
    delete[] zones;
    delete[] vehicleRegistrations;
    delete[] history;
    for (int i = 0; i < activeCount; i++) delete activeRequests[i];
    for (int i = 0; i < completedCount; i++) delete completedRequests[i];
    delete[] activeRequests;
    delete[] completedRequests;
    delete rollbackMgr;
    delete allocator;
}

bool ParkingSystem::registerVehicle(int vehicleId, int preferredZone) {
    for (int i = 0; i < vehicleCount; i++) if (vehicleRegistrations[i] == vehicleId) return false;
    if (vehicleCount < maxVehicles) {
        vehicleRegistrations[vehicleCount++] = vehicleId;
        return true;
    }
    return false;
}

int ParkingSystem::createRequest(int vehicleId, int zoneId, int time) {
    bool registered = false;
    for (int i = 0; i < vehicleCount; i++) if (vehicleRegistrations[i] == vehicleId) registered = true;
    if (!registered) return -1;
    
    ParkingRequest* request = new ParkingRequest(requestIdCounter++, vehicleId, zoneId, time);
    AllocationResult result = allocator->allocate(request, *rollbackMgr);
    
    if (result.success) {
        if (activeCount < maxRequests) {
            activeRequests[activeCount++] = request;
            cout << "Allocated Zone " << result.zoneId;
            if (result.isCrossZone) cout << " (Cross-zone, Penalty: " << result.penalty << ")";
            cout << endl;
            return request->getRequestId();
        }
    }
    delete request;
    return -1;
}

bool ParkingSystem::cancelRequest(int requestId) {
    for (int i = 0; i < activeCount; i++) {
        if (activeRequests[i]->getRequestId() == requestId) {
            if (activeRequests[i]->cancel()) {
                completedRequests[completedCount++] = activeRequests[i];
                for (int j = i; j < activeCount - 1; j++) activeRequests[j] = activeRequests[j + 1];
                activeCount--;
                addToHistory(requestId, activeRequests[i]->getZoneId(), 0, false);
                return true;
            }
        }
    }
    return false;
}

bool ParkingSystem::releaseParking(int requestId, int time) {
    for (int i = 0; i < activeCount; i++) {
        if (activeRequests[i]->getRequestId() == requestId) {
            if (activeRequests[i]->release(time)) {
                int duration = activeRequests[i]->getDuration();
                completedRequests[completedCount++] = activeRequests[i];
                for (int j = i; j < activeCount - 1; j++) activeRequests[j] = activeRequests[j + 1];
                activeCount--;
                addToHistory(requestId, activeRequests[i]->getZoneId(), duration, true);
                return true;
            }
        }
    }
    return false;
}

bool ParkingSystem::rollbackLast() {
    return rollbackMgr->rollbackLast(zones, zoneCount);
}

bool ParkingSystem::rollbackK(int k) {
    return rollbackMgr->rollbackK(k, zones, zoneCount);
}

void ParkingSystem::showStatus() const {
    cout << "\n=== SYSTEM STATUS ===" << endl;
    cout << "Zones: " << zoneCount << endl;
    cout << "Registered Vehicles: " << vehicleCount << endl;
    cout << "Active Requests: " << activeCount << endl;
    cout << "Completed Requests: " << completedCount << endl;
    
    cout << "\n--- Zone Status ---" << endl;
    for (int i = 0; i < zoneCount; i++) {
        int avail = zones[i].getAvailableSlotsCount();
        int total = zones[i].getTotalSlots();
        float util = (total - avail) * 100.0 / total;
        cout << "Zone " << i << ": " << (total - avail) << "/" << total 
             << " occupied (" << fixed << setprecision(1) << util << "%)" << endl;
    }
    
    cout << "\n--- Active Requests ---" << endl;
    for (int i = 0; i < activeCount; i++) {
        cout << "Req " << activeRequests[i]->getRequestId() 
             << " - Vehicle " << activeRequests[i]->getVehicleId()
             << " in Zone " << activeRequests[i]->getZoneId()
             << " - State: ";
        switch(activeRequests[i]->getState()) {
            case REQUESTED: cout << "REQUESTED"; break;
            case ALLOCATED: cout << "ALLOCATED"; break;
            case OCCUPIED: cout << "OCCUPIED"; break;
            default: cout << "UNKNOWN";
        }
        cout << endl;
    }
}

void ParkingSystem::analytics() const {
    cout << "\n=== ANALYTICS ===" << endl;
    
    int totalRequests = activeCount + completedCount;
    int completed = 0, cancelled = 0, totalDuration = 0;
    
    for (int i = 0; i < historyCount; i++) {
        if (history[i].completed) {
            completed++;
            totalDuration += history[i].duration;
        } else {
            cancelled++;
        }
    }
    
    int zoneUsage[10] = {0};
    for (int i = 0; i < historyCount; i++) {
        if (history[i].zoneId >= 0 && history[i].zoneId < 10) zoneUsage[history[i].zoneId]++;
    }
    
    int peakZone = 0, maxUsage = 0;
    for (int i = 0; i < zoneCount; i++) {
        if (zoneUsage[i] > maxUsage) {
            maxUsage = zoneUsage[i];
            peakZone = i;
        }
    }
    
    cout << "Total Requests: " << totalRequests << endl;
    cout << "Completed: " << completed << endl;
    cout << "Cancelled: " << cancelled << endl;
    
    if (completed > 0) {
        float avgDuration = (float)totalDuration / completed;
        cout << "Average Duration: " << fixed << setprecision(1) << avgDuration << " minutes" << endl;
    }
    
    cout << "Peak Usage Zone: Zone " << peakZone << " (" << maxUsage << " requests)" << endl;
    
    cout << "\nZone Utilization Rates:" << endl;
    for (int i = 0; i < zoneCount; i++) {
        float util = calculateUtilization(i);
        cout << "Zone " << i << ": " << fixed << setprecision(1) << (util * 100) << "%" << endl;
    }
}

void ParkingSystem::addToHistory(int reqId, int zoneId, int duration, bool completed) {
    if (historyCount < historySize) {
        history[historyCount].requestId = reqId;
        history[historyCount].zoneId = zoneId;
        history[historyCount].duration = duration;
        history[historyCount].completed = completed;
        historyCount++;
    } else {
        for (int i = 0; i < historySize - 1; i++) history[i] = history[i + 1];
        history[historySize - 1].requestId = reqId;
        history[historySize - 1].zoneId = zoneId;
        history[historySize - 1].duration = duration;
        history[historySize - 1].completed = completed;
    }
}

float ParkingSystem::calculateUtilization(int zoneId) const {
    if (zoneId >= 0 && zoneId < zoneCount) {
        int avail = zones[zoneId].getAvailableSlotsCount();
        int total = zones[zoneId].getTotalSlots();
        if (total > 0) return (float)(total - avail) / total;
    }
    return 0.0f;
}

ParkingRequest* ParkingSystem::findRequest(int requestId) const {
    for (int i = 0; i < activeCount; i++) if (activeRequests[i]->getRequestId() == requestId) return activeRequests[i];
    for (int i = 0; i < completedCount; i++) if (completedRequests[i]->getRequestId() == requestId) return completedRequests[i];
    return nullptr;
}