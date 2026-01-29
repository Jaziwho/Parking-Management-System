#ifndef PARKINGSLOT_H
#define PARKINGSLOT_H

class ParkingSlot {
private:
    int slotId;
    int zoneId;
    bool isAvailable;
    
public:
    ParkingSlot();
    ParkingSlot(int sId, int zId);
    bool available();
    void occupy();
    void release();
    int getSlotId();
    int getZoneId();
};

#endif