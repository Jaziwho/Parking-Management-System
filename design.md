# Smart Parking System Design

## 1. System Architecture

### 1.1 Data Structures Used
- Arrays: Zones, vehicles, requests, history
- Linked Lists: Zone connections, request queue, rollback stack
- Stacks: Operation history for rollback
- Queues: Request processing
- State Machines: Parking request lifecycle

### 1.2 Class Structure
ParkingSystem
├── Zone[] (City zones)
│   └── ParkingSlot[] (Parking slots)
├── Vehicle[] (Registered vehicles)
├── ParkingRequest[] (Active/completed requests)
├── AllocationEngine (Allocation logic)
│   └── RequestQueue (Linked List)
└── RollbackManager (Rollback operations)
    └── OperationStack (Linked List)

## 2. Core Components

### 2.1 Zone Management
- Each zone has array of parking slots
- Zone connections via adjacency list
- Capacity tracking with counters

### 2.2 Allocation Strategy
1. Same-zone preference first
2. Cross-zone fallback if full
3. First-available slot allocation
4. Penalty for cross-zone

### 2.3 State Machine
REQUESTED → ALLOCATED → OCCUPIED → RELEASED
    ↓           ↓
CANCELLED   CANCELLED

### 2.4 Rollback Mechanism
- Stack of operations (LIFO)
- Rollback single or k operations
- Restore slot availability
- Revert request states

## 3. Algorithms

### 3.1 Slot Allocation
- Linear search through zones/slots
- Time: O(z*s) where z=zones, s=slots
- Space: O(1)

### 3.2 Rollback
- k operations popped from stack
- Time: O(k)
- Space: O(1)

### 3.3 Analytics
- Linear traversal of history
- Time: O(n) where n=requests
- Space: O(1)

## 4. Complexity Analysis

### 4.1 Time Complexity
- Allocation: O(z*s)
- Cancellation: O(1)
- Rollback: O(k)
- Analytics: O(n)

### 4.2 Space Complexity
- O(z*s + n) for storage
- O(k) for rollback stack

## 5. File Structure
- Zone.h/Zone.cpp
- ParkingSlot.h/ParkingSlot.cpp
- Vehicle.h/Vehicle.cpp
- ParkingRequest.h/ParkingRequest.cpp
- AllocationEngine.h/AllocationEngine.cpp
- RollbackManager.h/RollbackManager.cpp
- ParkingSystem.h/ParkingSystem.cpp
- main.cpp

## 6. Testing
- Slot allocation correctness
- Cross-zone allocation
- Cancellation and rollback
- State transition validation
- Analytics accuracy
- User input handling