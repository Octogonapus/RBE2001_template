#pragma once

#include "../commands/DiscoveryMetadata.h"
#include "Resource.h"
#include <Arduino.h>
#include <cstring>

class DigitalOutResource : public Resource {
  public:
  DigitalOutResource(std::uint8_t resource,
                     std::uint8_t attachment,
                     const std::uint8_t *attachmentData)
    : Resource(resource, attachment, attachmentData), pin(attachmentData[0]) {
    pinMode(pin, OUTPUT);
  }

  virtual ~DigitalOutResource() {
  }

  void readFromPayload(std::uint8_t *buffer) override {
    digitalWrite(pin, buffer[0]);
  }

  void writeToPayload(std::uint8_t *buffer) override {
  }

  std::uint8_t getReceivePayloadLength() const override {
    return 1;
  }

  std::uint8_t getSendPayloadLength() const override {
    return 0;
  }

  protected:
  std::uint8_t pin;
};
