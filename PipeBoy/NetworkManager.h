#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

class Player;
class GameManager;

constexpr int8_t TARGET_ADDRESS = 0x10;

enum class MessageType : uint8_t {
  Ping,
  goToNextLevel,
  setPlayerPosition,
  setPipe,
  setRoundFinished,
  setRandomSeed,
  setPause,
  setFillPipeFast
};
constexpr uint8_t MESSAGE_TYPE_LEN = 5;

enum class DeviceType : uint8_t {
  NotDefined,
  Controller,
  Peripheral
};

typedef union {
  uint16_t value;
  struct {
    MessageType msgType : 8;
    uint8_t data : 8;
  };
} Event;

class NetworkManager {
private:
  Queue<Event, MESSAGE_TYPE_LEN> eventQueue;
  GameManager *gm;
  DeviceType deviceType;

  void sendSetFillPipeFast();
  void sendSetPause(bool pause);
  void sendSetPlayerPosition(uint8_t x, uint8_t y);
  void sendSetPipe();
  void sendGoToNextLevel();

  uint8_t packPosition(uint8_t x, uint8_t y);
  uint8_t unpackX(uint8_t data);
  uint8_t unpackY(uint8_t data);


public:
  static NetworkManager *instance;
  NetworkManager(GameManager *gmRef);

  void reset();
  bool isAvailable();
  void update();
  DeviceType getDeviceType() const;
  DeviceType handshake();
  void pullData();
  void pushDataToController(int numBytes);

  void setFillPipeFast();
  void setPause(bool pause);
  void setPlayerPosition(uint8_t x, uint8_t y);
  void setPipe();
  void goToNextLevel();

  // Declare the static callback functions.
  static void handshakeRequestCallback();
  static void pullDataCallback();
  static void pushDataCallback(int numBytes);
};

#endif