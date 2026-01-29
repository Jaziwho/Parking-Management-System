#include <iostream>
#include "ParkingSystem.h"
using namespace std;

int main() {
    ParkingSystem system(3);
    int choice, vehicleId, zoneId, time, requestId, k;
    
    do {
        cout << "\n=== SMART PARKING SYSTEM ===" << endl;
        cout << "1. Register Vehicle" << endl;
        cout << "2. Create Parking Request" << endl;
        cout << "3. Cancel Request" << endl;
        cout << "4. Release Parking" << endl;
        cout << "5. Rollback Last Operation" << endl;
        cout << "6. Rollback K Operations" << endl;
        cout << "7. Show System Status" << endl;
        cout << "8. Show Analytics" << endl;
        cout << "0. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;
        
        switch(choice) {
            case 1:
                cout << "Enter Vehicle ID: ";
                cin >> vehicleId;
                cout << "Enter Preferred Zone (0-2): ";
                cin >> zoneId;
                system.registerVehicle(vehicleId, zoneId);
                break;
                
            case 2:
                cout << "Enter Vehicle ID: ";
                cin >> vehicleId;
                cout << "Enter Requested Zone (0-2): ";
                cin >> zoneId;
                cout << "Enter Current Time: ";
                cin >> time;
                requestId = system.createRequest(vehicleId, zoneId, time);
                if (requestId != -1) cout << "Request " << requestId << " created" << endl;
                else cout << "Request failed" << endl;
                break;
                
            case 3:
                cout << "Enter Request ID: ";
                cin >> requestId;
                system.cancelRequest(requestId);
                break;
                
            case 4:
                cout << "Enter Request ID: ";
                cin >> requestId;
                cout << "Enter Current Time: ";
                cin >> time;
                system.releaseParking(requestId, time);
                break;
                
            case 5:
                system.rollbackLast();
                break;
                
            case 6:
                cout << "Enter K: ";
                cin >> k;
                system.rollbackK(k);
                break;
                
            case 7:
                system.showStatus();
                break;
                
            case 8:
                system.analytics();
                break;
                
            case 0:
                cout << "Exiting" << endl;
                break;
                
            default:
                cout << "Invalid choice" << endl;
        }
    } while(choice != 0);
    
    return 0;
}