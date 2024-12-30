#include "Observable.h"

void Observable::attachNetworkManager(NetworkManager *nm) {
  networkManager = nm;
}

void Observable::detachNetworkManager() {
  networkManager = nullptr;
}