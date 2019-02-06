#pragma once

#include <SimplePacketComs.h>
#include <cstdint>

class DigitalOutResource : public PacketEventAbstract {
  public:
  // Packet ID needs to be set
  DigitalOutResource(int id, std::uint8_t ipin) : PacketEventAbstract(id), pin(ipin) {
  }

  virtual ~DigitalOutResource() {
  }

  // User function to be called when a packet comes in
  // Buffer contains data from the packet coming in at the start of the function
  // User data is written into the buffer to send it back
  void event(float *buffer);

  static void provision(std::uint8_t pin);

  protected:
  std::uint8_t pin;
};
