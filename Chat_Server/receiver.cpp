#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
    // Check if the correct number of command-line arguments were passed in
    if (argc != 5) {
        std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
        return 1;
    }
    // Read the server address, port, username, and room name from the command-line arguments
    std::string server_hostname = argv[1];
    int server_port = std::stoi(argv[2]);
    std::string username = argv[3];
    std::string room_name = argv[4];

    // Create a Connection object
    Connection conn;

    // Connect to the server
    conn.connect(server_hostname, server_port);
    if (!conn.is_open()) {
        std::cerr << "Error: Failed to connect to server\n";
        return 1;
    }

    // Create a Message object
    Message msg;

    // Send rlogin messages (expect a response from the server for each one)
    try {
        // Set the message tag to TAG_RLOGIN
        msg.tag = TAG_RLOGIN;
        // Set the message data to the username
        msg.data = username;
        // Send the message to the server
        send(conn, msg);
        // Receive a response from the server
        receive(conn, msg, true);
    } catch (const std::runtime_error &ex) {
        std::cerr << "Error: Failed to send rlogin message\n";
        conn.close();
        exit(1);
    }

    // Send join messages (expect a response from the server for each one)
    try {
        // Set the message tag to TAG_JOIN
        msg.tag = TAG_JOIN;
        // Set the message data to the room name
        msg.data = room_name;
        // Send the message to the server
        send(conn, msg);
        // Receive a response from the server
        receive(conn, msg, true);
    } catch (const std::runtime_error &ex) {
        std::cerr << "Error: Failed to send join message\n";
        conn.close();
        exit(1);
    }

    // Loop waiting for messages from the server
    while (true) {
        // Create a Message object
        Message msg2;
        // Receive a message from the server
        receive(conn, msg2, false);

        // Check message format
        std::vector<std::string> split = msg2.detectColon();
        if (split.size() != 3) {
            std::cerr << "Error: Invalid message format\n";
            break;
        }
        // Print the received message
        std::cout << split[1] << ": " << split[2] << std::endl;
    }
    // Close the connection
    conn.close();
    return 0;
}

