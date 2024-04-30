#ifndef STACK_H
#define STACK_H

/*
** An implementation of a Stack
** with a fixed sized.
** 
** This is simply a dynamically allocated array
** with limited size, that you can't exceed
** 
** It could be seen as a simple proxy class
** with some nice method names
*/

template<class T, uint8_t Capacity>
class Stack {
  static constexpr uint8_t capacity = Capacity;
  T buffer[capacity];
  uint8_t cursor = 0;
  uint8_t bufferSize = 0;

public:
  void printAll();
  void pop(uint8_t);
  void push(T);
  void clear();
  uint8_t getSize();
  T getItem(uint8_t currentIndex);
};

template<class T, uint8_t Capacity>
void Stack<T, Capacity>::printAll() {
  for (uint8_t i = 0; i < getSize(); i++) {
    Serial.print(String("(" + String(buffer[i].x) + " - " + String(buffer[i].y) + ")"));
  }
}

template<class T, uint8_t Capacity>
void Stack<T, Capacity>::pop(uint8_t nbToRemove) {
  cursor -= nbToRemove;
}

template<class T, uint8_t Capacity>
void Stack<T, Capacity>::push(T item) {
  buffer[cursor++] = item;
}

template<class T, uint8_t Capacity>
void Stack<T, Capacity>::clear() {
  cursor = 0;
}

template<class T, uint8_t Capacity>
uint8_t Stack<T, Capacity>::getSize() {
  return cursor;
}

template<class T, uint8_t Capacity>
T Stack<T, Capacity>::getItem(uint8_t currentIndex) {
  return buffer[currentIndex];
}

#endif