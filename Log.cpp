//
// Created by alperen on 2.10.2023.
//

#include "Log.h"

Log::Log(const string &_timestamp, const string &_message, int _number_of_frames, int _number_of_hops, const string &_sender_id,
         const string &_receiver_id, bool _success, ActivityType _type) {
    timestamp = _timestamp;
    message_content = _message;
    number_of_frames = _number_of_frames;
    number_of_hops = _number_of_hops;
    sender_id = _sender_id;
    receiver_id = _receiver_id;
    success_status = _success;
    activity_type = _type;
}

void Log::out() const {
    const char* activityTypes[] = {
            "Message Received",
            "Message Forwarded",
            "Message Sent",
            "Message Dropped"
    };

    cout << "Activity: " << activityTypes[static_cast<int>(activity_type)] << endl;
    cout << "Timestamp: " << timestamp << endl;
    cout << "Number of frames: " << number_of_frames << endl;
    cout << "Number of hops: " << number_of_hops << endl;
    cout << "Sender ID: " << sender_id << endl;
    cout << "Receiver ID: " << receiver_id << endl;

    string success = success_status ? "Yes" : "No";
    cout << "Success: " << success << endl;

    if (!message_content.empty()) {
        cout << "Message: \"" << this->message_content << "\"" << endl;
    }
}

Log::~Log() {
    // TODO: Free any dynamically allocated memory if necessary.
}
