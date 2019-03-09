#pragma once

#include "../resource/Resource.h"
#include <Esp32SimplePacketComs.h>
#include <SimplePacketComs.h>

class ResourceServer : public PacketEventAbstract {
  public:
  ResourceServer(std::uint8_t ipacketId, std::unique_ptr<Resource> iresource)
    : PacketEventAbstract(ipacketId), resource(std::move(iresource)) {
  }

  virtual ~ResourceServer() {
  }

  void event(float *buffer) {
    resource->handlePayload((std::uint8_t *)buffer);
  }

  protected:
  std::unique_ptr<Resource> resource;
};
