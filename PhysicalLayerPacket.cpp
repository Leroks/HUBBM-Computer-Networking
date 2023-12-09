#include "PhysicalLayerPacket.h"

PhysicalLayerPacket::PhysicalLayerPacket(int _layer_ID, const string& _sender_MAC, const string& _receiver_MAC)
        : Packet(_layer_ID) {
    sender_MAC_address = _sender_MAC;
    receiver_MAC_address = _receiver_MAC;
}

void PhysicalLayerPacket::print() {
    // Call the base class (Packet) print function
    Packet::print();

    // Print layer-specific properties
    std::cout << "Sender MAC address: " << sender_MAC_address << ", Receiver MAC address: " << receiver_MAC_address << std::endl;
}

PhysicalLayerPacket::~PhysicalLayerPacket() {
    // TODO: Free any dynamically allocated memory if necessary.
}