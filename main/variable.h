#ifndef VARIABLE_H
#define VARIABLE_H

#include <Arduino.h>

// Holds single value behind mutex so get and set are safe from any task
template<typename T> class Variable {
public:
  Variable(T initialValue = T())
    : value(initialValue) {
    mutex = xSemaphoreCreateMutex();
  }

  virtual void set(T newValue) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    value = newValue;
    xSemaphoreGive(mutex);
  }

  virtual T get() const {
    xSemaphoreTake(mutex, portMAX_DELAY);
    T current = value;
    xSemaphoreGive(mutex);
    return current;
  }

protected:
  T value;
  SemaphoreHandle_t mutex;
};

// Fixed size array of values behind single mutex covering whole array
template<typename T, size_t N> class VariableArray {
public:
  VariableArray(T initialValue = T()) {
    mutex = xSemaphoreCreateMutex();
    for (size_t i = 0; i < N; i++) {
      values[i] = initialValue;
    }
  }

  void set(size_t index, T newValue) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    values[index] = newValue;
    xSemaphoreGive(mutex);
  }

  T get(size_t index) const {
    xSemaphoreTake(mutex, portMAX_DELAY);
    T current = values[index];
    xSemaphoreGive(mutex);
    return current;
  }

  size_t length() const {
    return N;
  }

private:
  T values[N];
  SemaphoreHandle_t mutex;
};

// Variable that runs callback once value changes
// Callback fires after mutex released so side effects don't hold lock
template<typename T> class VariableCallback : public Variable<T> {
public:
  using Callback = std::function<void(T)>;

  VariableCallback(T initialValue = T())
    : Variable<T>(initialValue), callback(nullptr) {}

  void set(T newValue) override {
    bool changed = false;

    xSemaphoreTake(this->mutex, portMAX_DELAY);
    if (newValue != this->value) {
      this->value = newValue;
      changed = true;
    }
    xSemaphoreGive(this->mutex);

    if (changed && callback) callback(newValue);
  }

  void onChange(Callback cb) {
    callback = cb;
  }

private:
  Callback callback;
};

#endif
