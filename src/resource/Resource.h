#pragma once

#include "../commands/DiscoveryMetadata.h"
#include <array>
#include <cstdint>

class Resource {
  public:
  /**
   * This should connect the resource.
   *
   * @param resource The resource type.
   * @param attachment The attachment point type.
   * @param attachmentData Any attachment data.
   */
  Resource(std::uint8_t resource, std::uint8_t attachment, const std::uint8_t *attachmentData) {
  }

  /**
   * This should disconnect the resource, if applicable.
   */
  virtual ~Resource() {
  }

  /**
   * Reads from the buffer to receive a command.
   *
   * @param buffer The buffer.
   */
  virtual void readFromPayload(std::uint8_t *buffer) = 0;

  /**
   * Writes into the buffer to send a response.
   *
   * @param buffer The buffer.
   */
  virtual void writeToPayload(std::uint8_t *buffer) = 0;

  /**
   * Sets the length of the receive payload.
   *
   * @param length The length of the receive payload.
   */
  std::uint8_t setReceivePayloadLength(std::uint8_t length) {
    receiveLength = length;
  }

  /**
   * Sets the length of the send payload.
   *
   * @param length The length of the send payload.
   */
  std::uint8_t setSendPayloadLength(std::uint8_t length) {
    sendLength = length;
  }

  /**
   * Gets the length of the receive payload.
   *
   * @return The length of the receive payload.
   */
  std::uint8_t getReceivePayloadLength() const {
    return receiveLength;
  }

  /**
   * Gets the length of the send payload.
   *
   * @return The length of the send payload.
   */
  std::uint8_t getSendPayloadLength() const {
    return sendLength;
  }

  protected:
  std::uint8_t sendLength{60};
  std::uint8_t receiveLength{60};
};
