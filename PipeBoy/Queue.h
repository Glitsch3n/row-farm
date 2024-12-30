#ifndef QUEUE_H
#define QUEUE_H

/*
** An implementation of a Queue
** with a fixed sized, using circular buffer
*/

template<class T, uint8_t Capacity>
class Queue {
  // The circular buffer need an empty item case to work (that the reason of the +1, here)
  static constexpr uint8_t capacity = Capacity + 1;
  T buffer[capacity];
  uint8_t startPos = 0;
  uint8_t cursor = 0;
  //uint8_t bufferSize = 0;

public:
  bool pop(T& refItem);
  bool push(T);
  void clear();
  T& operator[](uint8_t index);
};

template<class T, uint8_t Capacity>
T& Queue<T, Capacity>::operator[](uint8_t index) {
  uint8_t actualIndex = (startPos + index) % capacity;
  return buffer[actualIndex];
}

template<class T, uint8_t Capacity>
bool Queue<T, Capacity>::pop(T& refItem) {
  // Let's avoid exceeding pop
  if (startPos == cursor) {
    return false;
  }

  uint8_t oldStartPos = startPos;
  startPos = (startPos + 1) % capacity;

  refItem = buffer[oldStartPos];
  return true;
}

template<class T, uint8_t Capacity>
bool Queue<T, Capacity>::push(T item) {
  if ((cursor + 1) % capacity == startPos) {
    // Let's avoid overflow
    return false;
  }
  buffer[cursor] = item;
  cursor = (cursor + 1) % capacity;
  return true;
}

template<class T, uint8_t Capacity>
void Queue<T, Capacity>::clear() {
  cursor = 0;
  startPos = 0;
}

#endif
