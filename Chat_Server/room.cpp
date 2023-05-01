#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"
#include "client_util.h"
#include <string>
#include <sstream>

// constructor for the Room class
Room::Room(const std::string &room_name)
        : room_name(room_name) {
    // TODO: initialize the mutex
    pthread_mutex_init(&lock, nullptr);
}

// destructor for the Room class
Room::~Room() {
    // TODO: destroy the mutex
    pthread_mutex_destroy(&lock);
}

// function to add a member to the Room
void Room::add_member(User *user) {
    // TODO: add User to the room
    // check if the user is already in the room
    if (members.count(user) <= 0) {
        // use a guard to ensure thread-safety
        Guard g(lock);
        members.insert(user);
    }
}

// function to remove a member from the Room
void Room::remove_member(User *user) {
    // TODO: remove User from the room
    // use a guard to ensure thread-safety
    Guard g(lock);
    members.erase(user);
}

// function to broadcast a message to all members in the Room
void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
    // create the message to send in the format "room:sender:txt"
    std::stringstream ss;
    ss << rtrim(room_name);
    ss << ":" << rtrim(sender_username);
    ss << ":" << message_text;

    std::string message = ss.str();

    // TODO: send a message to every (receiver) User in the room
    std::set<User *>::iterator it;
    for (it = members.begin(); it != members.end(); it++) { //iterating through each member of the room
        User *current_user = *it; //get the current user
        if (current_user->username.compare(sender_username) != 0) {
            // check if the current user is not the sender of the message
            Message *msg = new Message(TAG_DELIVERY, message); //create a new message object with the delivery tag and the message text
            (current_user->mqueue).enqueue(msg); //enqueue the message in the user's message queue
        }
    }
}

