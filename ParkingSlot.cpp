#include "ParkingSlot.h"

ParkingSlot::ParkingSlot() {
    slotId = -1;
    zoneId = -1;
    isAvailable = true;
}

ParkingSlot::ParkingSlot(int sId, int zId) {
    slotId = sId;
    zoneId = zId;
    isAvailable = true;
}

bool ParkingSlot::available() {
    return isAvailable;
}

void ParkingSlot::occupy() {
    isAvailable = false;
}

void ParkingSlot::release() {
    isAvailable = true;
}

int ParkingSlot::getSlotId() {
    return slotId;
}

int ParkingSlot::getZoneId() {
    return zoneId;
}