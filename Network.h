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

    void printLog(string log_id, vector<Client> &clients);

    Client *find_client_MAC(string MAC, vector<Client> &clients);

    Client *find_client(string id, vector<Client> &clients);

    void putToQueue(string sender_id, string receiver_id,
                    string message, int message_limit, const string &sender_port, const string &receiver_port,
                    vector<Client> &clients);

    std::string deleteSubstring(std::string str);

    int find_frame_size(string message, int message_limit);

    void receive(vector<Client> &clients);

    bool checkEnd(stack<Packet *> frame);

    void showFrameInfo(string infoId, string outIn, int frameNo, vector<Client> &clients);

    void send(vector<Client> &clients);

    void showQInfo(string infoId, string outIn, vector<Client> &clients);

    string find_MAC(string id, vector<Client> &clients);

    void print_frame(stack<Packet *> frame);

    static bool containsPunctuation(const std::string &str);

    // Initialize the network from the input files.
    vector<Client> read_clients(const string &filename);

    void read_routing_tables(vector<Client> &clients, const string &filename);

    vector<string> read_commands(const string &filename);
};

#endif  // NETWORK_H
