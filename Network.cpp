#include "Network.h"
#include <sstream>
#include <string>
#include <ctime>

Network::Network() {

}

std::string Network::deleteSubstring(std::string str) {
    //delete the last # and the last character
    while (!str.empty()) {
        if (str.back() == '#') break;
        str.pop_back();
    }
    str.pop_back();
    //delete the first #
    auto it = str.begin();
    while (it != str.end()) {
        if (*it == '#') break;
        ++it;
    }
    str.erase(str.begin(), it + 1);
    return str;
}

int Network::find_frame_size(string message, int message_limit) {
    int size;
    size = message.size() / message_limit;
    if (message.size() % message_limit != 0) {
        size++;
    }
    return size;
}

Client *Network::find_client(string id, vector<Client> &clients) {
    for (auto &client: clients) {
        if (client.client_id == id) {
            return const_cast<Client *>(&client);
        }
    }
    return nullptr;
}

Client *Network::find_client_MAC(string mac, vector<Client> &clients) {
    for (auto &client: clients) {
        if (client.client_mac == mac) {
            return const_cast<Client *>(&client);
        }
    }
    return nullptr;
}

string Network::find_MAC(string id, vector<Client> &clients) {
    const Client *client = find_client(id, clients);
    if (client) {
        return client->client_mac;
    } else {
        return "";
    }
}

void Network::print_frame(stack<Packet *> frame) {
    PhysicalLayerPacket *pLayerPacket = dynamic_cast<PhysicalLayerPacket *>(frame.top());
    if (pLayerPacket) {
        pLayerPacket->print();
        frame.pop();
    }

    NetworkLayerPacket *pNetworkLayerPacket = dynamic_cast<NetworkLayerPacket *>(frame.top());
    if (pNetworkLayerPacket) {
        pNetworkLayerPacket->print();
        frame.pop();
    }

    TransportLayerPacket *pTransportLayerPacket = dynamic_cast<TransportLayerPacket *>(frame.top());
    if (pTransportLayerPacket) {
        pTransportLayerPacket->print();
        frame.pop();
    }
    ApplicationLayerPacket *pApplicationLayerPacket = dynamic_cast<ApplicationLayerPacket *>(frame.top());
    if (pApplicationLayerPacket) {
        pApplicationLayerPacket->print();
    }

    if (pApplicationLayerPacket && !pApplicationLayerPacket->message_data.empty()) {
        std::cout << "Message chunk carried: \"" << pApplicationLayerPacket->message_data << "\"" << std::endl;
    }

    if (pLayerPacket) {
        std::cout << "Number of hops so far: " << pLayerPacket->hopNumbers << std::endl;
        std::cout << "--------" << std::endl;
    }

}

void Network::putToQueue(string sender_id, string receiver_id,
                         string message, int message_limit, const string &sender_port, const string &receiver_port,
                         vector<Client> &clients) {
    int number;
    number = 0;
    int frame_size;
    frame_size = find_frame_size(message, message_limit);

    Client *receiver = find_client(receiver_id, clients);
    Client *client = find_client(sender_id, clients);

    cout << "Message to be sent: \"" << message << "\"" << endl << endl;

    for (int i = 0; i < frame_size; ++i) {
        int multiplier = 1;
        int adder = 0;
        number++;

        string data = message.substr(i * message_limit, message_limit);

        stack<Packet *> outQueue;
        outQueue.push(new ApplicationLayerPacket(0, sender_id, receiver_id, data));
        if(outQueue.empty()) {
            cout << "The stack is empty" << endl;
        }
        outQueue.push(new TransportLayerPacket(1, sender_port, receiver_port));
        if(outQueue.empty()) {
            cout << "The stack is empty" << endl;
        }
        outQueue.push(new NetworkLayerPacket(2, client->client_ip, receiver->client_ip));
        if(outQueue.empty()) {
            cout << "The stack is empty" << endl;
        }
        string findMac = find_MAC(client->routing_table[receiver_id], clients);
        if(findMac == ""){
            cout << "Error: Unreachable destination. Packets are dropped after "
                 << 0
                 << " hops!" << endl;
            return;
        }
        outQueue.push(new PhysicalLayerPacket(3, client->client_mac, findMac));
        if(outQueue.empty()) {
            cout << "The stack is empty" << endl;
        }
        adder*=multiplier;
        client->outgoing_queue.push(outQueue);

        cout << "Frame #" << number*multiplier << endl;
        print_frame(outQueue);
    }

    time_t currentTime = time(nullptr);
    if(currentTime == -1) {
        cout << "The time() function failed" << endl;
    }
    tm *timestamp = localtime(&currentTime);
    if(timestamp == nullptr) {
        cout << "The localtime() function failed" << endl;
    }
    ActivityType type = ActivityType::MESSAGE_SENT;
    ostringstream oss;

    oss << put_time(timestamp, "%Y-%m-%d %H:%M:%S");
    if(oss.fail()) {
        cout << "The put_time() function failed" << endl;
    }
    string timestampString = oss.str();
    if(timestampString.empty()) {
        cout << "The string is empty" << endl;
    }
    Log log(timestampString, message, number, 0, sender_id, receiver_id, true, type);
    client->log_entries.push_back(log);
    if(client->log_entries.empty()) {
        cout << "The vector is empty" << endl;
    }
}

void Network::showFrameInfo(string infoId, string outIn, int frameNo, vector<Client> &clients) {

    Client *client = find_client(infoId, clients);
    string data;
    std::queue<std::stack<Packet *>> temp;

    if (outIn == "out") {
        temp = client->outgoing_queue;
    } else {
        temp = client->incoming_queue;
    }

    if (frameNo > temp.size()) {
        cout << "No such frame." << endl;
        return;
    }

    for (int i = 0; i + 1 < frameNo; i++) {
        i++;
        temp.pop();
        i--;
    }

    std::stack<Packet *> frame = temp.front();

    if (outIn == "out") {
        cout << "Current Frame #" << frameNo << " on the outgoing queue of client " << infoId << endl;
    } else {
        cout << "Current Frame #" << frameNo << " on the incoming queue of client " << infoId << endl;
    }

    if(frame.empty()) {
        cout << "The stack is empty" << endl;
    }
    auto *pPhysicalLayerPacket = dynamic_cast<PhysicalLayerPacket *>(frame.top());
    if(pPhysicalLayerPacket == nullptr) {
        cout << "The dynamic_cast() function failed" << endl;
    }
    frame.pop();
    auto *pLayerPacket = dynamic_cast<NetworkLayerPacket *>(frame.top());
    if(pLayerPacket == nullptr) {
        cout << "The dynamic_cast() function failed" << endl;
    }
    frame.pop();
    auto *pTransportLayerPacket = dynamic_cast<TransportLayerPacket *>(frame.top());
    if(pTransportLayerPacket == nullptr) {
        cout << "The dynamic_cast() function failed" << endl;
    }
    frame.pop();
    auto *pApplicationLayerPacket = dynamic_cast<ApplicationLayerPacket *>(frame.top());
    if(pApplicationLayerPacket == nullptr) {
        cout << "The dynamic_cast() function failed" << endl;
    }

    if (pApplicationLayerPacket) {
        std::cout << "Carried Message: \"" << pApplicationLayerPacket->message_data << "\"" << std::endl;
        std::cout << "Layer 0 info: Sender ID: " << pApplicationLayerPacket->sender_ID << ", Receiver ID: "
                  << pApplicationLayerPacket->receiver_ID << std::endl;
    }

    if (pTransportLayerPacket) {
        std::cout << "Layer 1 info: Sender port number: " << pTransportLayerPacket->sender_port_number
                  << ", Receiver port number: " << pTransportLayerPacket->receiver_port_number << std::endl;
    }

    if (pLayerPacket) {
        std::cout << "Layer 2 info: Sender IP address: " << pLayerPacket->sender_IP_address << ", Receiver IP address: "
                  << pLayerPacket->receiver_IP_address << std::endl;
    }

    if (pPhysicalLayerPacket) {
        std::cout << "Layer 3 info: Sender MAC address: " << pPhysicalLayerPacket->sender_MAC_address
                  << ", Receiver MAC address: " << pPhysicalLayerPacket->receiver_MAC_address << std::endl;
    }

    cout << "Number of hops so far: " << pPhysicalLayerPacket->hopNumbers << endl;
}

void Network::showQInfo(string infoId, string outIn, vector<Client> &clients) {
    Client *client = find_client(infoId, clients);
    std::queue<std::stack<Packet *>> temp;

    if (outIn == "out") {
        temp = client->outgoing_queue;
        cout << "Client " << infoId << " Outgoing Queue Status" << endl;
    } else {
        temp = client->incoming_queue;
        cout << "Client " << infoId << " Incoming Queue Status" << endl;
    }

    cout << "Current total number of frames: " << temp.size() << endl;
}

bool Network::containsPunctuation(const std::string &str) {
    const string punctuation = ".?!";
    return str.find_first_of(punctuation) != std::string::npos;
}

bool Network::checkEnd(stack<Packet *> frame) {
    if (frame.size() >= 3) {
        frame.pop(); // Skip PhysicalLayerPacket
        frame.pop(); // Skip NetworkLayerPacket
        frame.pop(); // Skip TransportLayerPacket

        ApplicationLayerPacket *pApplicationLayerPacket = dynamic_cast<ApplicationLayerPacket *>(frame.top());

        if (pApplicationLayerPacket && containsPunctuation(pApplicationLayerPacket->message_data)) {
            return true;
        }
    }
    return false;
}

void Network::send(vector<Client> &clients) {
    for (int i = 0; i < clients.size(); i++) {
        if(clients.size() == 0) {
            cout << "The vector is empty" << endl;
        }
        Client *client = &clients[i];
        if(client->outgoing_queue.empty()) {
            continue;
        }
        queue<stack<Packet *>> queue;

        if (client->outgoing_queue.empty()) {
            continue;
        }
        queue = client->outgoing_queue;
        int number;
        number = 0;
        int multiplier = 1;
        while (!queue.empty()) {
            if (queue.empty()) break;
            number += 2;
            stack<Packet *> frame = queue.front();
            auto *pPhysicalLayerPacket = dynamic_cast<PhysicalLayerPacket *>(frame.top());
            if(pPhysicalLayerPacket == nullptr) {
            }
            pPhysicalLayerPacket->hopNumbers++;
            frame.pop();
            frame.pop();
            frame.pop();
            number--;
            auto *pApplicationLayerPacket = dynamic_cast<ApplicationLayerPacket *>(frame.top());
            cout << "Client " << find_client_MAC(pPhysicalLayerPacket->sender_MAC_address, clients)->client_id
                 << " sending frame #"
                 << number << " to client "
                 << find_client_MAC(pPhysicalLayerPacket->receiver_MAC_address, clients)->client_id << endl;
            if(queue.front().empty()) {
                number*=multiplier;
            }
            print_frame(queue.front());
            char tmpChar = pApplicationLayerPacket->message_data[pApplicationLayerPacket->message_data.size() - 1];
            if (tmpChar == '?' || tmpChar == '!' || tmpChar == '.' || tmpChar == 'xxx') {
                number = 0;
            }
            string to_go = client->routing_table[pApplicationLayerPacket->receiver_ID];
            if(to_go == ""){
                cout << "Error: Unreachable destination. Packets are dropped after "
                     << pPhysicalLayerPacket->hopNumbers
                     << " hops!" << endl;
                number = 0;
            }
            find_client(to_go, clients)->incoming_queue.push(queue.front());
            client->outgoing_queue.pop();
            queue = client->outgoing_queue;
        }
    }
}

void Network::receive(std::vector<Client> &clients) {
    if(clients.empty()) {
        cout << "The vector is empty" << endl;
    }
    for (int i = 0; i < clients.size(); i++) {
        if(clients.size() == 0) {
            cout << "The vector is empty" << endl;
        }
        Client *client = &clients[i];

        std::queue<std::stack<Packet *>> queue;

        if (client->incoming_queue.empty()) {
            continue;
        }

        queue = client->incoming_queue;
        int number;
        number = 0;
        std::string message;

        while (!queue.empty()) {
            number++;

            std::stack<Packet *> frame = queue.front();

            auto *pPhysicalLayerPacket = dynamic_cast<PhysicalLayerPacket *>(frame.top());
            number++;
            frame.pop();
            auto *pLayerPacket = dynamic_cast<NetworkLayerPacket *>(frame.top());
            number--;
            frame.pop();
            auto *pTransportLayerPacket = dynamic_cast<TransportLayerPacket *>(frame.top());
            frame.pop();
            auto *pApplicationLayerPacket = dynamic_cast<ApplicationLayerPacket *>(frame.top());

            bool success = true;
            ActivityType type = ActivityType::MESSAGE_RECEIVED;
            bool needDel = false;

            if (pApplicationLayerPacket->receiver_ID == client->client_id) {
                message += pApplicationLayerPacket->message_data;
                std::cout << "Client " << client->client_id << " receiving frame #" << number << " from client "
                          << find_client_MAC(pPhysicalLayerPacket->sender_MAC_address, clients)->client_id
                          << ", originating from client " << pApplicationLayerPacket->sender_ID << std::endl;
                print_frame(queue.front());
                needDel = true;
            } else if (client->routing_table.count(client->routing_table[pApplicationLayerPacket->receiver_ID]) == 0) {
                std::cout << "Client " << client->client_id << " receiving frame #" << number << " from client "
                          << find_client_MAC(pPhysicalLayerPacket->sender_MAC_address, clients)->client_id
                          << ", but intended for client " << pApplicationLayerPacket->receiver_ID << ". Forwarding... "
                          << std::endl;
                std::cout << "Error: Unreachable destination. Packets are dropped after "
                          << pPhysicalLayerPacket->hopNumbers
                          << " hops!" << std::endl;
                type = ActivityType::MESSAGE_DROPPED;
                success = false;
                needDel = true;
            } else {
                if (number == 1) {
                    std::cout << "Client " << client->client_id << " receiving a message from client "
                              << find_client_MAC(pPhysicalLayerPacket->sender_MAC_address, clients)->client_id
                              << ", but intended for client " << pApplicationLayerPacket->receiver_ID
                              << ". Forwarding... "
                              << std::endl;
                }

                pPhysicalLayerPacket->sender_MAC_address = client->client_mac;
                pPhysicalLayerPacket->receiver_MAC_address =
                        find_client(client->routing_table[pApplicationLayerPacket->receiver_ID], clients)->client_mac;

                std::cout << "Frame #" << number << " MAC address change: New sender MAC "
                          << pPhysicalLayerPacket->sender_MAC_address
                          << ", new receiver MAC " << pPhysicalLayerPacket->receiver_MAC_address << std::endl;

                client->outgoing_queue.push(queue.front());
                type = ActivityType::MESSAGE_FORWARDED;
            }

            if (checkEnd(queue.front())) {
                std::time_t currentTime = std::time(nullptr);
                int multiplier = 1;
                std::tm *timestamp = std::localtime(&currentTime);
                int adder = 0;
                std::ostringstream oss;

                oss << std::put_time(timestamp, "%Y-%m-%d %H:%M:%S");
                std::string timestampString = oss.str();

                Log log(timestampString, message, number+adder, pPhysicalLayerPacket->hopNumbers,
                        pApplicationLayerPacket->sender_ID, pApplicationLayerPacket->receiver_ID,
                        success, type);
                number *= multiplier;
                client->log_entries.push_back(log);
                number *= multiplier;
                number = 0;

                if (type == ActivityType::MESSAGE_RECEIVED) {
                    std::cout << "Client " << client->client_id << " received the message \"" << message << "\""
                              << " from client " << pApplicationLayerPacket->sender_ID << "." << std::endl;
                }

                std::cout << "--------" << std::endl;
                message = "";
            }

            std::stack<Packet *> tmpPack = client->incoming_queue.front();
            if (needDel) {
                if(!tmpPack.empty()) {
                    delete tmpPack.top();
                    tmpPack.pop();
                }
                if(!tmpPack.empty()) {
                    delete tmpPack.top();
                    tmpPack.pop();
                }
                if(!tmpPack.empty()) {
                    delete tmpPack.top();
                    tmpPack.pop();
                }
                if(!tmpPack.empty()) {
                    delete tmpPack.top();
                }
            }

            if(client->incoming_queue.empty()) {
                break;
            }
            client->incoming_queue.pop();
            if(client->incoming_queue.empty()) {
                break;
            }
            queue = client->incoming_queue;
        }
    }
}


void Network::printLog(string log_id, vector<Client> &clients) {
    Client *client = find_client(log_id, clients);

    std::vector<Log> &logs = client->log_entries;

    if (logs.empty()) {
        return;
    }

    std::cout << "Client " << log_id << " Logs:" << std::endl;

    for (size_t i = 0; i < logs.size(); ++i) {
        std::cout << "--------------" << std::endl;
        std::cout << "Log Entry #" << i + 1 << ":" << std::endl;
        if(logs.empty()) {
            cout << "The vector is empty" << endl;
        }
        logs[i].print();
    }
}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                               const string &sender_port, const string &receiver_port) {
    for (const auto &command: commands) {
        int len = command.size() + 9;
        std::stringstream ss(command);
        std::string cmd;
        ss >> cmd;

        for (int i = 0; i < len; i++) {
            std::cout << "-";
        }
        std::cout << "\nCommand: " << command << std::endl;
        for (int i = 0; i < len; i++) {
            std::cout << "-";
        }
        std::cout << std::endl;

        if (cmd == "MESSAGE") {
            std::string sender_id, receiver_id, message;
            if(ss.fail()) {
                cout << "The stringstream() function failed" << endl;
            }
            ss >> sender_id >> receiver_id;
            std::getline(ss, message);
            message = deleteSubstring(message);
            if(message.empty()) {
                cout << "The string is empty" << endl;
            }
            putToQueue(sender_id, receiver_id, message, message_limit, sender_port, receiver_port, clients);

        } else if (cmd == "SHOW_FRAME_INFO") {
            std::string infoId, outIn;
            int frameNo;
            frameNo = 0;
            ss >> infoId >> outIn >> frameNo;
            showFrameInfo(infoId, outIn, frameNo, clients);
        } else if (cmd == "SHOW_Q_INFO") {
            std::string infoId, outIn;
            ss >> infoId >> outIn;
            showQInfo(infoId, outIn, clients);
        } else if (cmd == "SEND") {
            send(clients);
        } else if (cmd == "RECEIVE") {
            receive(clients);
        } else if (cmd == "PRINT_LOG") {
            std::string logId;
            ss >> logId;
            printLog(logId, clients);
        } else {
            std::cout << "Invalid command." << std::endl;
        }
    }
}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    string line;
    fstream file(filename, ios::in);
    int counter;
    counter = 0;
    file >> counter;
    getline(file, line);
    for (int i = 0; i < counter; ++i) {
        getline(file, line);
        stringstream ss(line);
        string id, ip, mac;
        ss >> id >> ip >> mac;
        Client client(id, ip, mac);
        clients.push_back(client);
    }
    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    int count = clients.size();

    for (int i = 0; i < count; ++i) {
        for (int j = 0; j < count; ++j) {
            std::string line;
            if (std::getline(file, line)) {
                std::stringstream ss(line);
                if(ss.fail()) {
                    cout << "The stringstream() function failed" << endl;
                }
                std::string id, id2;
                if (ss >> id >> id2) {
                    clients[i].routing_table[id] = id2;
                } else {
                    std::cerr << "Error: Invalid line format in " << filename << std::endl;
                }
            } else {
                std::cerr << "Error: Unexpected end of file in " << filename << std::endl;
                return;
            }
        }
    }
}

vector<string> Network::read_commands(const string &filename) {
    std::vector<std::string> commands;
    std::ifstream file(filename);
    int multiplier = 1;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return commands;
    }

    std::string line;
    std::getline(file, line);
    int count = 0;

    try {
        count = std::stoi(line);
    } catch (const std::invalid_argument &e) {
        std::cerr << "Error converting count to integer in file: " << filename << std::endl;
        return commands;
    }

    for (int i = 0; i < count; ++i) {
        std::getline(file, line);
        commands.push_back(line);
    }
    count *= multiplier;

    return commands;
}

Network::~Network() {
}