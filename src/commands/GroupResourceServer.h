#pragma once

#include "../resource/Resource.h"
#include <Esp32SimplePacketComs.h>
#include <SimplePacketComs.h>
#include <cstring>
#include <vector>

class GroupResourceServer : public PacketEventAbstract {
  public:
  GroupResourceServer(std::uint8_t ipacketId) : PacketEventAbstract(ipacketId) {
  }

  virtual ~GroupResourceServer() {
  }

  void event(float *buffer) override {
    std::uint8_t *buf = (std::uint8_t *)buffer;

    for (auto &resource : resources) {
      resource->readFromPayload(buf);
      buf = buf + resource->getReceivePayloadLength();
    }

    buf = (std::uint8_t *)buffer;
    std::memset(buf, 0, PAYLOAD_LENGTH * (sizeof buf[0]));

    for (auto &resource : resources) {
      resource->writeToPayload(buf);
      buf = buf + resource->getReceivePayloadLength();
    }
  }

  void addResource(std::unique_ptr<Resource> resource) {
    resources.push_back(std::move(resource));
  }

  protected:
  std::vector<std::unique_ptr<Resource>> resources{};
};
