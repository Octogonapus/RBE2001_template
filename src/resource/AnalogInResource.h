#pragma once

#include "../commands/DiscoveryMetadata.h"
#include "Resource.h"
#include <Arduino.h>
#include <cstring>

class AnalogInResource : public Resource {
  public:
  AnalogInResource(std::uint8_t resource,
                   std::uint8_t attachment,
                   const std::uint8_t *attachmentData)
    : Resource(resource, attachment, attachmentData), pin(attachmentData[0]) {
    pinMode(pin, INPUT);
  }

  virtual ~AnalogInResource() {
  }

  void handlePayload(std::uint8_t *buffer) override {
    std::memset(buffer, 0, PAYLOAD_LENGTH * (sizeof buffer[0]));

    const int value = analogRead(pin);

    buffer[0] = value & 0xF;
    buffer[1] = value & 0xF0;
  }

  protected:
  std::uint8_t pin;
};
