#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

struct ConnServer {
    Connection *conn;
    Server *server;

    ConnServer(Connection *conn, Server *server) : conn(conn), server(server) { }

    ~ConnServer() {
        delete conn;
    }
};

std::string rtrim(const std::string &s) {
    const std::string WHITESPACE = " \n\r\t\f\v";
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {
    void chat_with_receiver(Connection *conn, Server *server, std::string &username) {
        // Receive first message from the client
        Message message;
        if (!conn->receive(message)) {
            // Check if the message is invalid
            if (conn->get_last_result() == Connection::INVALID_MSG) {
                conn->send(Message(TAG_ERR, "Error: Invalid message format\n"));
                return;
            }
        }
        // Check if the length of the data is shorter than the maximum length
        if (message.data.length() > message.MAX_LEN) {
            conn->send(Message(TAG_ERR, "Error: Invalid message format\n"));
            return;
        }
        // Check if the first message is a join message
        else if (message.tag != TAG_JOIN) {
            conn->send(Message(TAG_ERR, "Error: Failed to send join message\n"));
            return;
        }

        // Create a new user with the provided username
        User *user = new User(username);
        // Extract the room name from the join message
        std::string room_name = message.data;
        // Find or create the specified room
        Room *room = server->find_or_create_room(room_name);
        // Add the user to the room
        room->add_member(user);
        // Send a message to the client to confirm that they have joined the room
        if (!conn->send(Message(TAG_OK, "joined room: " + room_name))) {
            return;
        }


        // Continuously check for new messages in the user's message queue
        while (true) {
            // Dequeue the next message from the user's message queue
            Message *msg = user->mqueue.dequeue();
            if (msg == nullptr) {
                // Delete the message if it was null free memory
                delete (msg);
            } else {
                // Send the message to the client
                bool status = conn->send(*msg);
                // Delete the message
                delete (msg);
                if (!status) {
                    // Break out of the loop if the message could not be sent
                    break;
                }
            }
        }

        // Remove the user from the room
        room->remove_member(user);
    }


    void chat_with_sender(Connection *conn, Server *server, std::string &username) {
        // pointer to the current room the user is in
        Room *room = nullptr;

        while (true) {
            // Receive message from client
            Message message;
            if (conn->receive(message)) {

                if  (message.tag == TAG_QUIT) {
                    // Handle QUIT request
                    // Send confirmation message to client
                    conn->send(Message(TAG_OK, "bye!\n"));
                    // Exit the while loop and end the function
                    break;
                } else if (message.tag == TAG_JOIN) {
                    // Handle JOIN ROOM request
                    // Find or create a room with the specified name
                    room = server->find_or_create_room(message.data);
                    // Send confirmation message to client
                    conn->send(Message(TAG_OK, "joined room" + message.data));
                    continue;
                } else if (room == nullptr) {
                    conn->send(Message(TAG_ERR, "Error: User not in a room.\n"));
                } else if (message.tag == TAG_LEAVE) {
                    // Leave the current room and set room pointer to null
                    room = nullptr;
                    // Send confirmation message to client
                    conn->send(Message(TAG_OK, "Left room successfully\n"));
                } else if (message.tag == TAG_SENDALL) {
                    // Handle SEND ALL request
                    // Broadcast message to all users in the current room
                    room->broadcast_message(username, message.data);
                    // Send confirmation message to client
                    conn->send(Message(TAG_OK, "Message sent successfully\n"));
                } else {
                    if (conn->get_last_result() == Connection::INVALID_MSG) {
                        // Send error message if received message is invalid
                        conn->send(Message(TAG_ERR, "Error: Invalid message format\n"));
                    }
                }
            }
        }
    }


    void *worker(void *arg) {
      pthread_detach(pthread_self());

      // TODO: use a static cast to convert arg from a void* to
      //       whatever pointer type describes the object(s) needed
      //       to communicate with a client (sender or receiver)
      ConnServer *info_ = static_cast<ConnServer *>(arg);
      std::unique_ptr<ConnServer> info(info_);

      Message msg;

      if (!info->conn->receive(msg)) {
          if (info->conn->get_last_result() == Connection::INVALID_MSG) {
              info->conn->send(Message(TAG_ERR, "Error: Invalid message format\n"));
          }
          return nullptr;
      }

      // TODO: read login message (should be tagged either with
      //       TAG_SLOGIN or TAG_RLOGIN), send response

      if (msg.tag != TAG_SLOGIN && msg.tag != TAG_RLOGIN) {
          info->conn->send(Message(TAG_ERR, "Error: Enter slogin or rlogin"));
          return nullptr;
      }


      if (!info->conn->send(Message(TAG_OK, "Username: " + msg.data))) {
          return nullptr;
      }

      // TODO: depending on whether the client logged in as a sender or
      //       receiver, communicate with the client (implementing
      //       separate helper functions for each of these possibilities
      //       is a good idea)

      if (msg.tag == TAG_RLOGIN) {
          chat_with_receiver(info->conn, info->server, msg.data);
      } else if (msg.tag == TAG_SLOGIN) {
          chat_with_sender(info->conn, info->server, msg.data);
      }
      return nullptr;
  }

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
        : m_port(port)
        , m_ssock(-1) {
    // TODO: initialize mutex
    pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
    // TODO: destroy mutex
    pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
    // TODO: use open_listenfd to create the server socket, return true
    //       if successful, false if not
    m_ssock = open_listenfd(std::to_string(m_port).c_str());

    if (m_ssock < 0) {
        std::cerr << "Server cannot be opened" << std::endl;
        return false;
    }
    return true;
}

void Server::handle_client_requests() {
    // TODO: infinite loop calling accept or Accept, starting a new
    //       pthread for each connected client
    assert(m_ssock >= 0);

    while (true) {
        int clientfd = accept(m_ssock, NULL, NULL);
        if (clientfd < 0) {
            std::cerr << "Connection not accepted\n";
        } else {
            Connection *conn = new Connection(clientfd);
            ConnServer *info = new ConnServer(conn, this);

            pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, worker, info) != 0) {
                std::cerr << "Cannot create thread" << std::endl;
                delete info;
            }
        }
    }
}

Room *Server::find_or_create_room(const std::string &room_name) {
    // TODO: return a pointer to the unique Room object representing
    //       the named chat room, creating a new one if necessary
    Guard G(m_lock);
    Room *spot = new Room(room_name);

    auto room_iter = m_rooms.find(room_name);

    if (!(room_iter == m_rooms.end())) {
        return room_iter->second;
    } else {
        m_rooms.insert({room_name, spot});
        return spot;
    }
}

