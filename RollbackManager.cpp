#include <iostream>
#include "RollbackManager.h"
using namespace std;

RollbackManager::RollbackManager() {
    stackTop = nullptr;
    stackSize = 0;
    historyFront = nullptr;
    historyRear = nullptr;
    historySize = 0;
}

RollbackManager::~RollbackManager() {
    while (stackTop != nullptr) {
        RollbackAction* temp = stackTop;
        stackTop = stackTop->next;
        delete temp;
    }
    while (historyFront != nullptr) {
        HistoryNode* temp = historyFront;
        historyFront = historyFront->next;
        delete temp;
    }
}

void RollbackManager::push(const RollbackAction& action) {
    if (stackSize >= MAX_STACK_SIZE) {
        if (stackTop != nullptr && stackTop->next != nullptr) {
            RollbackAction* current = stackTop;
            RollbackAction* prev = nullptr;
            while (current->next != nullptr) {
                prev = current;
                current = current->next;
            }
            if (prev != nullptr) {
                delete current;
                prev->next = nullptr;
                stackSize--;
            }
        }
    }
    
    RollbackAction* newAction = new RollbackAction(action);
    newAction->next = stackTop;
    stackTop = newAction;
    stackSize++;
    addToHistory(action.requestId);
}

bool RollbackManager::pop(RollbackAction& action) {
    if (stackTop == nullptr) return false;
    RollbackAction* top = stackTop;
    action = *top;
    stackTop = stackTop->next;
    delete top;
    stackSize--;
    return true;
}

bool RollbackManager::isEmpty() const {
    return stackTop == nullptr;
}

int RollbackManager::getStackSize() const {
    return stackSize;
}

bool RollbackManager::rollbackLast(Zone* zones, int zoneCount) {
    if (stackTop == nullptr) return false;
    RollbackAction action;
    if (pop(action)) {
        if (action.zoneId >= 0 && action.zoneId < zoneCount) zones[action.zoneId].releaseSlot(action.slotId);
        markAsRolledBack(action.requestId);
        cout << "Rolled back request " << action.requestId << endl;
        return true;
    }
    return false;
}

bool RollbackManager::rollbackK(int k, Zone* zones, int zoneCount) {
    if (k <= 0 || k > stackSize) return false;
    cout << "Rolling back " << k << " operations..." << endl;
    
    RollbackAction* actions = new RollbackAction[k];
    int count = 0;
    while (count < k && !isEmpty()) {
        if (pop(actions[count])) count++;
    }
    
    for (int i = 0; i < count; i++) {
        if (actions[i].zoneId >= 0 && actions[i].zoneId < zoneCount) zones[actions[i].zoneId].releaseSlot(actions[i].slotId);
        markAsRolledBack(actions[i].requestId);
        cout << "  - Rolled back request " << actions[i].requestId << endl;
    }
    
    delete[] actions;
    return true;
}

void RollbackManager::addToHistory(int requestId) {
    HistoryNode* newNode = new HistoryNode(requestId);
    if (historyRear == nullptr) historyFront = historyRear = newNode;
    else {
        historyRear->next = newNode;
        historyRear = newNode;
    }
    historySize++;
}

void RollbackManager::markAsRolledBack(int requestId) {
    HistoryNode* current = historyFront;
    while (current != nullptr) {
        if (current->requestId == requestId) {
            current->wasRolledBack = true;
            break;
        }
        current = current->next;
    }
}

int RollbackManager::getRollbackCount() const {
    int count = 0;
    HistoryNode* current = historyFront;
    while (current != nullptr) {
        if (current->wasRolledBack) count++;
        current = current->next;
    }
    return count;
}

void RollbackManager::printRecentRollbacks(int count) const {
    cout << "Recent " << count << " rollbacks:" << endl;
    int printed = 0;
    HistoryNode* current = historyFront;
    while (current != nullptr && printed < count) {
        if (current->wasRolledBack) {
            cout << "  Request " << current->requestId << " was rolled back" << endl;
            printed++;
        }
        current = current->next;
    }
    if (printed == 0) cout << "  No rollbacks found" << endl;
}