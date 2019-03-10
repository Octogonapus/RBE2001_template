#pragma once

#include "../resource/Resource.h"
#include <Esp32SimplePacketComs.h>
#include <SimplePacketComs.h>
#include <cstring>

class ResourceServer : public PacketEventAbstract {
  public:
  ResourceServer(std::uint8_t ipacketId, std::unique_ptr<Resource> iresource)
    : PacketEventAbstract(ipacketId), resource(std::move(iresource)) {
  }

  virtual ~ResourceServer() {
  }

  void event(float *buffer) override {
    std::uint8_t *buf = (std::uint8_t *)buffer;
    resource->readFromPayload(buf);
    std::memset(buf, 0, PAYLOAD_LENGTH * (sizeof buf[0]));
    resource->writeToPayload(buf);
  }

  protected:
  std::unique_ptr<Resource> resource;
};
