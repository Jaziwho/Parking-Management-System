#ifndef ZONE_H
#define ZONE_H

#include "ParkingSlot.h"

class Zone {
private:
    int zoneId;
    ParkingSlot* slots;
    int slotCount;
    int occupiedCount;
    int* connectedZones;
    int connectedCount;
    int maxConnections;
    
public:
    Zone(int zId = 0, int count = 0);
    Zone(const Zone& other);
    ~Zone();
    Zone& operator=(const Zone& other);
    ParkingSlot* getAvailableSlot();
    bool releaseSlot(int slotId);
    int getAvailableSlotsCount() const;
    int getTotalSlots() const;
    void addConnection(int zoneId);
    bool isConnectedTo(int zoneId) const;
    int getZoneId() const;
    float getUtilizationRate() const;
};

#endif