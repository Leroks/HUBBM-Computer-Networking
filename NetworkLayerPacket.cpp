#include "NetworkLayerPacket.h"
#include <iostream>
using namespace std;

NetworkLayerPacket::NetworkLayerPacket(int _layer_ID, const string &_sender_IP, const string &_receiver_IP)
        : Packet(_layer_ID) {
    sender_IP_address = _sender_IP;
    receiver_IP_address = _receiver_IP;
}

void NetworkLayerPacket::print() {
    // TODO: Override the virtual print function from Packet class to additionally out layer-specific properties.
    Packet::print();
    cout << "Sender IP address: " << sender_IP_address << ", Receiver IP address: " << receiver_IP_address << endl;
}

NetworkLayerPacket::~NetworkLayerPacket() {
    // TODO: Free any dynamically allocated memory if necessary.
}
