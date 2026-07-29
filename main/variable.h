#ifndef VARIABLE_H
#define VARIABLE_H

// Holds single value behind mutex so get and set are safe from any task
// Set access restricted to owning class
template<typename T, typename Owner> class Variable {
public:
  Variable(T initialValue = T())
    : value(initialValue) {
    mutex = xSemaphoreCreateMutex();
  }

  T get() const {
    xSemaphoreTake(mutex, portMAX_DELAY);
    T current = value;
    xSemaphoreGive(mutex);
    return current;
  }

protected:
  T value;
  SemaphoreHandle_t mutex;

private:
  virtual void set(T newValue) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    value = newValue;
    xSemaphoreGive(mutex);
  }

  friend Owner;
};

// Fixed size array of values behind single mutex covering whole array
// Set access restricted to owning class
template<typename T, size_t N, typename Owner> class VariableArray {
public:
  VariableArray(T initialValue = T()) {
    mutex = xSemaphoreCreateMutex();
    for (size_t i = 0; i < N; i++) {
      values[i] = initialValue;
    }
  }

  T get(size_t index) const {
    // Clamp index to valid range
    if (index >= N) {
      index = N - 1;
    }

    xSemaphoreTake(mutex, portMAX_DELAY);
    T current = values[index];
    xSemaphoreGive(mutex);
    return current;
  }

  size_t length() const {
    return N;
  }

private:
  void set(size_t index, T newValue) {
    // Clamp index to valid range
    if (index >= N) {
      index = N - 1;
    }

    xSemaphoreTake(mutex, portMAX_DELAY);
    values[index] = newValue;
    xSemaphoreGive(mutex);
  }

  T values[N];
  SemaphoreHandle_t mutex;

  friend Owner;
};

// Variable that runs callback once value changes
// Callback fires after mutex released so side effects don't hold lock
// Set and callback registration both restricted to owning class
template<typename T, typename Owner> class VariableCallback : public Variable<T, Owner> {
public:
  using Callback = std::function<void(T)>;

  VariableCallback(T initialValue = T())
    : Variable<T, Owner>(initialValue), callback(nullptr) {}

private:
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

  Callback callback;

  friend Owner;
};

#endif
