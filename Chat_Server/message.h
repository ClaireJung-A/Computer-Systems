#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

struct Message
{
    // An encoded message may have at most this many characters,
    // including the trailing newline ('\n'). Note that this does
    // *not* include a NUL terminator (if one is needed to
    // temporarily store the encoded message.)
    static const unsigned MAX_LEN = 255;

    std::string tag;
    std::string data;

    Message() {}

    Message(const std::string &tag, const std::string &data)
            : tag(tag), data(data) {}


    std::vector<std::string> detectColon() const {
        // create a vector to store result
        std::vector<std::string> result;
        // create a stringstream to process data
        std::stringstream ss;
        std::string pos;
        // insert data into the stringstream
        ss << data;
        // while there are still parts of the data separated by ':'
        while (std::getline(ss, pos, ':')) {
            // add the part to the result vector
            result.push_back(pos);
        }
        // return the result vector
        return result;
    }

    std::string to_str() const {
        // create an empty string variable to store the final string
        std::string s;
        s.append(tag);
        s.append(":");
        s.append(data);
        // return the final string
        return s;
    }

};

// standard message tags (note that you don't need to worry about
// "senduser" or "empty" messages)
#define TAG_ERR       "err"       // protocol error
#define TAG_OK        "ok"        // success response
#define TAG_SLOGIN    "slogin"    // register as specific user for sending
#define TAG_RLOGIN    "rlogin"    // register as specific user for receiving
#define TAG_JOIN      "join"      // join a chat room
#define TAG_LEAVE     "leave"     // leave a chat room
#define TAG_SENDALL   "sendall"   // send message to all users in chat room
#define TAG_SENDUSER  "senduser"  // send message to specific user in chat room
#define TAG_QUIT      "quit"      // quit
#define TAG_DELIVERY  "delivery"  // message delivered by server to receiving client
#define TAG_EMPTY     "empty"     // sent by server to receiving client to indicate no msgs available

#endif // MESSAGE_H

