#include "DiscoveryPacket.h"
#include "../resource/AnalogInResource.h"
#include "../resource/DigitalOutResource.h"
#include "../resource/ServoResource.h"
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
    parseDiscardDiscoveryPacket(buffer, dest);
    break;

  default:
    dest[0] = STATUS_REJECTED_UNKNOWN_OPERATION;
    break;
  }

  std::memcpy(buffer, dest, PAYLOAD_LENGTH * sizeof(buffer[0]));
}

void DiscoveryPacket::parseDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest) {
  std::uint8_t packetId = buffer[1];
  std::uint8_t resourceType = buffer[2];
  std::uint8_t attachment = buffer[3];
  const std::uint8_t *attachmentData = buffer + 4;

  attachResource(packetId, resourceType, attachment, attachmentData, dest);
}

void DiscoveryPacket::parseGroupDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest) {
  std::uint8_t groupId = buffer[1];
  std::uint8_t packetId = buffer[2];
  std::uint8_t count = buffer[3];

  groupServers.emplace(groupId, new GroupResourceServer(packetId));
  coms->attach(groupServers.at(groupId));

  dest[0] = STATUS_ACCEPTED;
}

void DiscoveryPacket::parseGroupMemberDiscoveryPacket(const std::uint8_t *buffer,
                                                      std::uint8_t *dest) {
  std::uint8_t groupId = buffer[1];
  std::uint8_t sendStart = buffer[2];
  std::uint8_t sendEnd = buffer[3];
  std::uint8_t receiveStart = buffer[4];
  std::uint8_t receiveEnd = buffer[5];
  std::uint8_t resourceType = buffer[6];
  std::uint8_t attachment = buffer[7];
  const std::uint8_t *attachmentData = buffer + 8;

  std::unique_ptr<Resource> resource;
  std::uint8_t status;
  std::tie(resource, status) = makeResource(resourceType, attachment, attachmentData);

  if (resource) {
    // Send length is from the PC perspective, which is our receive length
    resource->setReceivePayloadLength(sendEnd - sendStart);

    // Receive length is from the PC perspective, which is our send length
    resource->setSendPayloadLength(receiveEnd - receiveStart);

    groupServers.at(groupId)->addResource(std::move(resource));
  }

  dest[0] = status;
}

void DiscoveryPacket::parseDiscardDiscoveryPacket(const std::uint8_t *buffer, std::uint8_t *dest) {
  for (auto const &elem : resourceServers) {
    coms->detach(elem.first);
    delete elem.second;
  }
  resourceServers.clear();

  for (auto const &elem : groupServers) {
    coms->detach(elem.first);
    delete elem.second;
  }
  groupServers.clear();

  dest[0] = STATUS_DISCARD_COMPLETE;
}

void DiscoveryPacket::attachResource(std::uint8_t packetId,
                                     std::uint8_t resourceType,
                                     std::uint8_t attachment,
                                     const std::uint8_t *attachmentData,
                                     std::uint8_t *dest) {
  std::unique_ptr<Resource> resource;
  std::uint8_t status;
  std::tie(resource, status) = makeResource(resourceType, attachment, attachmentData);

  if (resource) {
    resourceServers.emplace(packetId, new ResourceServer(packetId, std::move(resource)));
    coms->attach(resourceServers.at(packetId));
  }

  dest[0] = status;
}

std::tuple<std::unique_ptr<Resource>, std::uint8_t>
DiscoveryPacket::makeResource(std::uint8_t resourceType,
                              std::uint8_t attachment,
                              const std::uint8_t *attachmentData) {
  switch (resourceType) {
  case RESOURCE_TYPE_ANALOG_IN: {
    switch (attachment) {
    case ATTACHMENT_POINT_TYPE_PIN: {
      return std::make_tuple(std::unique_ptr<AnalogInResource>(
                               new AnalogInResource(resourceType, attachment, attachmentData)),
                             STATUS_ACCEPTED);
    }

    default: { return std::make_tuple(nullptr, STATUS_REJECTED_UNKNOWN_ATTACHMENT); }
    }
  }

  case RESOURCE_TYPE_DIGITAL_OUT: {
    switch (attachment) {
    case ATTACHMENT_POINT_TYPE_PIN: {
      return std::make_tuple(std::unique_ptr<DigitalOutResource>(
                               new DigitalOutResource(resourceType, attachment, attachmentData)),
                             STATUS_ACCEPTED);
    }

    default: { return std::make_tuple(nullptr, STATUS_REJECTED_UNKNOWN_ATTACHMENT); }
    }
  }

  case RESOURCE_TYPE_SERVO: {
    switch (attachment) {
    case ATTACHMENT_POINT_TYPE_PIN: {
      return std::make_tuple(
        std::unique_ptr<ServoResource>(new ServoResource(resourceType, attachment, attachmentData)),
        STATUS_ACCEPTED);
    }

    default: { return std::make_tuple(nullptr, STATUS_REJECTED_UNKNOWN_ATTACHMENT); }
    }
  }

  default: { return std::make_tuple(nullptr, STATUS_REJECTED_UNKNOWN_RESOURCE); }
  }
}
