#include "DiscoveryPacket.h"
#include "../resource/AnalogInResource.h"
#include "../resource/DigitalOutResource.h"
#include <Arduino.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>

// User function to be called when a packet comes in
// Buffer contains data from the packet coming in at the start of the function
// User data is written into the buffer to send it back
void DiscoveryPacket::event(float *buffer) {
  std::uint8_t *buf = (std::uint8_t *)buffer;

  Serial.println("Got DiscoveryPacket event:");
  // Print the bytes we got
  for (int i = 0; i < 60; i++) {
    Serial.print(buf[i]);
    Serial.print(", ");
  }
  Serial.println();

  parseGeneralDiscoveryPacket(buf);
}

void DiscoveryPacket::parseGeneralDiscoveryPacket(std::uint8_t *buffer) {
  const std::uint8_t operation = buffer[0];
  std::uint8_t *dest = (std::uint8_t *)std::calloc(PAYLOAD_LENGTH, sizeof(std::uint8_t));

  switch (operation) {
  case OPERATION_DISCOVERY_ID:
    parseDiscoveryPacket(buffer, dest);
    break;

  case OPERATION_GROUP_DISCOVERY_ID:
    parseGroupDiscoveryPacket(buffer, dest);
    break;

  case OPERATION_GROUP_MEMBER_DISCOVERY_ID:
    parseGroupMemberDiscoveryPacket(buffer, dest);
    break;

  case OPERATION_DISCARD_DISCOVERY_ID:
    parseGroupMemberDiscoveryPacket(buffer, dest);
    break;

  default:
    dest[0] = STATUS_REJECTED_UNKNOWN_OPERATION;
    break;
  }

  std::memcpy(buffer, dest, PAYLOAD_LENGTH * sizeof(buffer[0]));
}

void DiscoveryPacket::parseDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest) {
  std::uint8_t packetId = buffer[1];
  std::uint8_t resource = buffer[2];
  std::uint8_t attachment = buffer[3];
  const std::uint8_t *attachmentData = buffer + 4;
  attachResource(packetId, resource, attachment, attachmentData, dest);
}

void DiscoveryPacket::parseGroupDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest) {
}

void DiscoveryPacket::parseGroupMemberDiscoveryPacket(const std::uint8_t *buffer,
                                                      std::uint8_t *dest) {
}

void DiscoveryPacket::parseDiscardDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest) {
}

void DiscoveryPacket::attachResource(std::uint8_t packetId,
                                     std::uint8_t resource,
                                     std::uint8_t attachment,
                                     const std::uint8_t *attachmentData,
                                     std::uint8_t *dest) {
  switch (resource) {
  case RESOURCE_TYPE_ANALOG_IN: {
    switch (attachment) {
    case ATTACHMENT_POINT_TYPE_PIN: {
      coms->attach(new ResourceServer(packetId,
                                      std::unique_ptr<AnalogInResource>(new AnalogInResource(
                                        resource, attachment, attachmentData))));

      dest[0] = STATUS_ACCEPTED;
      return;
    }

    default: {
      dest[0] = STATUS_REJECTED_INVALID_ATTACHMENT;
      return;
    }
    }
  }

  case RESOURCE_TYPE_DIGITAL_OUT: {
    switch (attachment) {
    case ATTACHMENT_POINT_TYPE_PIN: {
      coms->attach(new ResourceServer(packetId,
                                      std::unique_ptr<DigitalOutResource>(new DigitalOutResource(
                                        resource, attachment, attachmentData))));

      dest[0] = STATUS_ACCEPTED;
      return;
    }

    default: {
      dest[0] = STATUS_REJECTED_INVALID_ATTACHMENT;
      return;
    }
    }
  }

  default: {
    dest[0] = STATUS_REJECTED_UNKNOWN_RESOURCE;
    return;
  }
  }
}

// bool DiscoveryPacket::parseDiscoveryPacket(std::int8_t *buffer) {
//  int attachmentPointIndex = 3;
//  std::int8_t packetId = buffer[1];
//  std::uint8_t resourceType = buffer[2];
//  std::uint8_t attachmentPoint = buffer[attachmentPointIndex];
//  switch (resourceType) {
//  case 1: {
//    // DigitalIn
//    switch (attachmentPoint) {
//    case 1: {
//      // Pin
//      std::uint8_t pinNumber = buffer[attachmentPointIndex + 1];
//      coms->attach(new DigitalInResource(packetId, pinNumber));
//      return true;
//    }
//
//    default:
//      return false;
//    }
//  }
//
//  case 2: {
//    // DigitalOut
//    switch (attachmentPoint) {
//    case 1: {
//      // Pin
//      std::uint8_t pinNumber = buffer[attachmentPointIndex + 1];
//      coms->attach(new DigitalOutResource(packetId, pinNumber));
//      return true;
//    }
//
//    default:
//      return false;
//    }
//  }
//
//  case 3: {
//    // AnalogIn
//    switch (attachmentPoint) {
//    case 1: {
//      // Pin
//      std::uint8_t pinNumber = buffer[attachmentPointIndex + 1];
//      coms->attach(new AnalogInResource(packetId, pinNumber));
//      return true;
//    }
//
//    default:
//      return false;
//    }
//  }
//
//  case 4: {
//    // AnalogOut
//    switch (attachmentPoint) {
//    case 1: {
//      // Pin
//      std::uint8_t pinNumber = buffer[attachmentPointIndex + 1];
//      coms->attach(new AnalogInResource(packetId, pinNumber));
//      return true;
//    }
//
//    default:
//      return false;
//    }
//  }
//
//  case 5: {
//    // SerialConnection
//    return false;
//  }
//
//  case 6: {
//    // Servo
//    switch (attachmentPoint) {
//    case 1: {
//      // Pin
//      std::uint8_t pinNumber = buffer[attachmentPointIndex + 1];
//      return true;
//    }
//
//    default:
//      return false;
//    }
//  }
//
//  case 7:
//    // Stepper
//    switch (attachmentPoint) {
//    case 2: {
//      // PinGroup
//      std::int8_t count = buffer[attachmentPointIndex + 1];
//      switch (count) {
//      case 2: {
//        std::uint8_t pin1 = buffer[attachmentPointIndex + 2];
//        std::uint8_t pin2 = buffer[attachmentPointIndex + 3];
//        return true;
//      }
//
//      case 4: {
//        std::uint8_t pin1 = buffer[attachmentPointIndex + 2];
//        std::uint8_t pin2 = buffer[attachmentPointIndex + 3];
//        std::uint8_t pin3 = buffer[attachmentPointIndex + 4];
//        std::uint8_t pin4 = buffer[attachmentPointIndex + 5];
//        return true;
//      }
//
//      default:
//        return false;
//      }
//    }
//
//    default:
//      return false;
//    }
//
//  case 8: {
//    // Encoder
//    switch (attachmentPoint) {
//    case 2: {
//      // PinGroup
//      std::int8_t count = buffer[attachmentPointIndex + 1];
//      switch (count) {
//      case 2: {
//        std::uint8_t pin1 = buffer[attachmentPointIndex + 2];
//        std::uint8_t pin2 = buffer[attachmentPointIndex + 3];
//        coms->attach(new EncoderResource(packetId, pin1, pin2));
//        return true;
//      }
//
//      default:
//        return false;
//      }
//    }
//
//    default:
//      return false;
//    }
//  }
//
//  case 9: {
//    // Button
//    switch (attachmentPoint) {
//    case 1: {
//      // Pin
//      std::uint8_t pinNumber = buffer[attachmentPointIndex + 1];
//      return true;
//    }
//
//    default:
//      return false;
//    }
//  }
//
//  case 10: {
//    // Ultrasonic
//    switch (attachmentPoint) {
//    case 2: {
//      // PinGroup
//      std::int8_t count = buffer[attachmentPointIndex + 1];
//      switch (count) {
//      case 2: {
//        std::uint8_t pin1 = buffer[attachmentPointIndex + 2];
//        std::uint8_t pin2 = buffer[attachmentPointIndex + 3];
//        return true;
//      }
//
//      default:
//        return false;
//      }
//    }
//
//    default:
//      return false;
//    }
//  }
//
//  case 11: {
//    // PiezoelectricSpeaker
//    switch (attachmentPoint) {
//    case 1: {
//      // Pin
//      std::uint8_t pinNumber = buffer[attachmentPointIndex + 1];
//      return true;
//    }
//
//    default:
//      return false;
//    }
//  }
//
//  default:
//    Serial.println("invalid resource type");
//    return false;
//  }
//}
