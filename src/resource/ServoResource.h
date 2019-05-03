#pragma once

#include "../commands/DiscoveryMetadata.h"
#include "Resource.h"
#include <Arduino.h>
#include <ESP32Servo.h>
#include <cstring>

class ServoResource : public Resource {
  public:
  ServoResource(std::uint8_t resource, std::uint8_t attachment, const std::uint8_t *attachmentData)
    : Resource(resource, attachment, attachmentData), pin(attachmentData[0]) {
    servo.attach(pin);
  }

  virtual ~ServoResource() {
    servo.detach();
  }

  void readFromPayload(std::uint8_t *buffer) override {
    servo.write(buffer[0]);
  }

  void writeToPayload(std::uint8_t *buffer) override {
  }

  protected:
  std::uint8_t pin;
  Servo servo;
};
