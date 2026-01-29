#include "Zone.h"

Zone::Zone(int zId, int count) {
    zoneId = zId;
    slotCount = count;
    occupiedCount = 0;
    slots = new ParkingSlot[slotCount];
    for (int i = 0; i < slotCount; i++) slots[i] = ParkingSlot(i, zoneId);
    maxConnections = 5;
    connectedZones = new int[maxConnections];
    connectedCount = 0;
}

Zone::Zone(const Zone& other) {
    zoneId = other.zoneId;
    slotCount = other.slotCount;
    occupiedCount = other.occupiedCount;
    slots = new ParkingSlot[slotCount];
    for (int i = 0; i < slotCount; i++) slots[i] = other.slots[i];
    maxConnections = other.maxConnections;
    connectedZones = new int[maxConnections];
    connectedCount = other.connectedCount;
    for (int i = 0; i < connectedCount; i++) connectedZones[i] = other.connectedZones[i];
}

Zone::~Zone() {
    delete[] slots;
    delete[] connectedZones;
}

Zone& Zone::operator=(const Zone& other) {
    if (this != &other) {
        delete[] slots;
        delete[] connectedZones;
        zoneId = other.zoneId;
        slotCount = other.slotCount;
        occupiedCount = other.occupiedCount;
        slots = new ParkingSlot[slotCount];
        for (int i = 0; i < slotCount; i++) slots[i] = other.slots[i];
        maxConnections = other.maxConnections;
        connectedZones = new int[maxConnections];
        connectedCount = other.connectedCount;
        for (int i = 0; i < connectedCount; i++) connectedZones[i] = other.connectedZones[i];
    }
    return *this;
}

ParkingSlot* Zone::getAvailableSlot() {
    for (int i = 0; i < slotCount; i++) {
        if (slots[i].available()) {
            slots[i].occupy();
            occupiedCount++;
            return &slots[i];
        }
    }
    return nullptr;
}

bool Zone::releaseSlot(int slotId) {
    if (slotId >= 0 && slotId < slotCount) {
        if (!slots[slotId].available()) {
            slots[slotId].release();
            occupiedCount--;
            return true;
        }
    }
    return false;
}

int Zone::getAvailableSlotsCount() const {
    return slotCount - occupiedCount;
}

int Zone::getTotalSlots() const {
    return slotCount;
}

void Zone::addConnection(int zoneId) {
    if (connectedCount < maxConnections) {
        for (int i = 0; i < connectedCount; i++) if (connectedZones[i] == zoneId) return;
        connectedZones[connectedCount++] = zoneId;
    }
}

bool Zone::isConnectedTo(int zoneId) const {
    for (int i = 0; i < connectedCount; i++) if (connectedZones[i] == zoneId) return true;
    return false;
}

int Zone::getZoneId() const {
    return zoneId;
}

float Zone::getUtilizationRate() const {
    if (slotCount == 0) return 0.0f;
    return (float)occupiedCount / slotCount;
}