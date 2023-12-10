//
// Created by alperen on 27.09.2023.
//

#include "Client.h"

Client::Client(string const &_id, string const &_ip, string const &_mac) {
    client_id = _id;
    client_ip = _ip;
    client_mac = _mac;
}

ostream &operator<<(ostream &os, const Client &client) {
    os << "client_id: " << client.client_id << " client_ip: " << client.client_ip << " client_mac: "
       << client.client_mac << endl;
    return os;
}

void Client::clearQueue(queue<stack<Packet *>> &q) {
    while (!q.empty()) {
        stack<Packet *> &tmp = q.front();
        while (!tmp.empty()) {
            if (tmp.empty()) break;
            delete tmp.top();
            tmp.pop();
        }
        if (q.empty()) break;
        q.pop();
    }
}

Client::~Client() {
    clearQueue(incoming_queue);
    clearQueue(outgoing_queue);
}

