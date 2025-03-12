#include <Wire.h>
#include "Constants.h"
#include "GameManager.h"
#include "Player.h"
#include "NetworkManager.h"

NetworkManager *NetworkManager::instance = nullptr;
volatile HandshakeState handshakeState = HandshakeState::NotReady;

NetworkManager::NetworkManager(GameManager *gmRef)
  : gm(gmRef) {
  NetworkManager::instance = this;
  reset();
}

void NetworkManager::reset() {
  deviceType = DeviceType::NotDefined;
  handshakeState = HandshakeState::NotReady;
  Wire.end();
}

void NetworkManager::update() {
  if (deviceType == DeviceType::Controller) {
    Wire.requestFrom(TARGET_ADDRESS, 2);
    pullData();
  }
}

bool NetworkManager::isAvailable() {
  return deviceType != DeviceType::NotDefined;
}

HandshakeState NetworkManager::getHandshakeState() const {
  return handshakeState;
}

// Un-queue one event and push it to the Controller
// We can't push too much as once otherwise it risks to be discarded
// by the Controller.
// Anyway, the Controller will constantly ask for new event.
// So we can be pretty sure that it will be consumed anyway.
void NetworkManager::pushDataToController(int numBytes) {
  Event e;

  eventQueue.pop(e);
  switch (e.msgType) {
    case MessageType::setPipe:
      sendSetPipe();
      break;
    case MessageType::setPlayerPosition:
      {
        uint8_t x = unpackX(e.data);
        uint8_t y = unpackY(e.data);
        sendSetPlayerPosition(x, y);
        break;
      }
    case MessageType::setPause:
      sendSetPause(e.data);
      break;
    case MessageType::setFillPipeFast:
      sendSetFillPipeFast();
      break;
    default:
      // Handle unexpected message types if needed.
      break;
  }
}

void NetworkManager::pullData() {
  // Always read 2 byte per 2 byte
  while (Wire.available()) {
    MessageType msg = (MessageType)Wire.read();
#ifdef DEBUG_MODE
    Serial.print(F("Pulling Data... MessageType="));
    Serial.println((uint8_t)msg);
#endif

    if (msg == MessageType::goToNextLevel) {
      gm->setState(GameState::Playing);
      gm->setNextLevel();
      Wire.read();  // read useless boolean
    } else if (msg == MessageType::setPlayerPosition) {
      uint8_t data = Wire.read();
      uint8_t x = unpackX(data);
      uint8_t y = unpackY(data);
      gm->getPlayer()->setPosition(x, y);
    } else if (msg == MessageType::setPipe) {
      gm->getPlayer()->pushTile();
      gm->getPlayer()->setIsPlayerTurn(true);
      Wire.read();  // read useless boolean
    } else if (msg == MessageType::setRoundFinished) {
      // TODO: Decide how to signal that a round is finished.}
    } else if (msg == MessageType::setRandomSeed) {
      uint8_t currentRandomSeed = Wire.read();
      randomSeed(currentRandomSeed);
    } else if (msg == MessageType::setPause) {
      bool pause = Wire.read();
      if (pause) {
        gm->setState(GameState::Paused);
      } else {
        gm->setState(GameState::Playing);
      }
    } else if (msg == MessageType::setFillPipeFast) {
      gm->getTileMap()->setFillPipeFast();
      Wire.read();  // read useless boolean
    }
  }
}

DeviceType NetworkManager::initHandshake() {
  power_twi_enable();
  Wire.begin();
  Wire.requestFrom(TARGET_ADDRESS, 1);

  if (Wire.available()) {
    deviceType = DeviceType::Controller;
    if (Wire.read()) {
      handshakeState = HandshakeState::Started;
    }
#ifdef DEBUG_MODE
    Serial.println(F("Set DeviceType to Controller"));
#endif
  } else {
    deviceType = DeviceType::Peripheral;
    Wire.begin(TARGET_ADDRESS);
    Wire.onRequest(handshakeRequestCallback);

#ifdef DEBUG_MODE
    Serial.println(F("Set DeviceType to Peripherical"));
#endif
  }
  return deviceType;
}

void NetworkManager::initNetworkTransmission() {
  if (deviceType == DeviceType::Controller) {
    uint8_t currentRandomSeed = arduboy.generateRandomSeed();
    randomSeed(currentRandomSeed);
    Wire.beginTransmission(TARGET_ADDRESS);
    Wire.write((uint8_t)MessageType::setRandomSeed);
    Wire.write(currentRandomSeed);
    Wire.endTransmission();
    goToNextLevel();
  }
#ifdef DEBUG_MODE
  handshakeState = HandshakeState::Completed;
  Serial.println(F("Handshake Completed !"));
#endif
}

DeviceType NetworkManager::getDeviceType() const {
  return deviceType;
}

// --- Methods to send specific actions ---

void NetworkManager::sendSetPlayerPosition(uint8_t x, uint8_t y) {
  Wire.write((uint8_t)MessageType::setPlayerPosition);
  Wire.write(packPosition(x, y));
}

void NetworkManager::setPlayerPosition(uint8_t x, uint8_t y) {
  if (deviceType == DeviceType::Controller) {
    Wire.beginTransmission(TARGET_ADDRESS);
    sendSetPlayerPosition(x, y);
    Wire.endTransmission();
  } else {
    Event e;
    e.msgType = MessageType::setPlayerPosition;
    e.data = ((x & 0x0F) << 4) | (y & 0x0F);
    eventQueue.push(e);
  }
}

void NetworkManager::sendSetPipe() {
  Wire.write((uint8_t)MessageType::setPipe);
  Wire.write(true);
}

void NetworkManager::setPipe() {
  if (deviceType == DeviceType::Controller) {
    Wire.beginTransmission(TARGET_ADDRESS);
    sendSetPipe();
    Wire.endTransmission();
  } else {
    Event e;
    e.msgType = MessageType::setPipe;
    eventQueue.push(e);
  }
  gm->getPlayer()->setIsPlayerTurn(false);
}

void NetworkManager::sendGoToNextLevel() {
  Wire.write((uint8_t)MessageType::goToNextLevel);
  Wire.write(true);
}

void NetworkManager::goToNextLevel() {
  if (deviceType == DeviceType::Controller) {
    Wire.beginTransmission(TARGET_ADDRESS);
    sendGoToNextLevel();
    Wire.endTransmission();
  } else {
    Event e;
    e.msgType = MessageType::goToNextLevel;
    eventQueue.push(e);
  }
}

void NetworkManager::sendSetPause(bool pause) {
  Wire.write((uint8_t)MessageType::setPause);
  Wire.write(pause);
}

void NetworkManager::setPause(bool pause) {
  if (deviceType == DeviceType::Controller) {
    Wire.beginTransmission(TARGET_ADDRESS);
    sendSetPause(pause);
    Wire.endTransmission();
  } else {
    Event e;
    e.msgType = MessageType::setPause;
    e.data = pause;
    eventQueue.push(e);
  }
}

void NetworkManager::sendSetFillPipeFast() {
  Wire.write((uint8_t)MessageType::setFillPipeFast);
  Wire.write(true);
}

void NetworkManager::setFillPipeFast() {
  if (deviceType == DeviceType::Controller) {
    Wire.beginTransmission(TARGET_ADDRESS);
    sendSetFillPipeFast();
    Wire.endTransmission();
  } else {
    Event e;
    e.msgType = MessageType::setFillPipeFast;
    eventQueue.push(e);
  }
}


// --- Helper Functions for Packing/Unpacking Coordinates ---

uint8_t NetworkManager::packPosition(uint8_t x, uint8_t y) {
  // Pack two 4-bit values into one byte.
  return ((x & 0x0F) << 4) | (y & 0x0F);
}

uint8_t NetworkManager::unpackX(uint8_t data) {
  return (data >> 4) & 0x0F;
}

uint8_t NetworkManager::unpackY(uint8_t data) {
  return data & 0x0F;
}

// --- Static Callback Functions ---
// These static functions forward the calls to the current instance.

void NetworkManager::handshakeRequestCallback() {
#ifdef DEBUG_MODE
  Serial.println(F("Validate Handshake"));
#endif
  // Let's setup the new callback right before we send the response to the
  // Controller. So we make sure, we will receive all the request from him 
  Wire.onReceive(pullDataCallback);
  Wire.onRequest(pushDataCallback);
  Wire.write(true);
  handshakeState = HandshakeState::Started;
}

void NetworkManager::pullDataCallback() {
  instance->pullData();
}

void NetworkManager::pushDataCallback(int numBytes) {
  instance->pushDataToController(numBytes);
}
