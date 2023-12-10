#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <vector>
#include "Client.h"
#include "Packet.h"

using namespace std;

class Network {
public:
    Network();

    ~Network();

    // Executes commands given as a vector of strings while utilizing the remaining arguments.
    void
    process_commands(vector<Client> &clients, vector<string> &commands, int message_limit, const string &sender_port,
                     const string &receiver_port);

    void printLog(string logId, vector<Client> &clients);

    Client *findClientMac(string MAC, vector<Client> &clients);


    std::string deleteSubstring(std::string str);

    void toQueue(string senderId, const string &sender_port, string receiverId, int message_limit, string message,
                 const string &receiver_port, vector<Client> &clients);


    Client *findClient(string id, vector<Client> &clients);

    int findFrameSize(string message, int message_limit);

    void receive(vector<Client> &clients);

    bool checkEnd(stack<Packet *> frame);

    void showFrameInfo(string infoId, string outIn, int frameNo, vector<Client> &clients);

    void send(vector<Client> &clients);

    void queueInfo(string infoId, string outIn, vector<Client> &clients);

    string find_MAC(string id, vector<Client> &clients);

    static bool hasSigns(const std::string &str);

    void printFrame(stack<Packet *> frame);

    // Initialize the network from the input files.
    vector<Client> read_clients(const string &filename);

    void read_routing_tables(vector<Client> &clients, const string &filename);

    vector<string> read_commands(const string &filename);
};

#endif  // NETWORK_H
