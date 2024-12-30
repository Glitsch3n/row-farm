#ifndef OBSERVABLE_H
#define OBSERVABLE_H

class NetworkManager;

class Observable {
protected:
  NetworkManager *networkManager;
public:
  void attachNetworkManager(NetworkManager *nm);
  void detachNetworkManager();
};

#endif