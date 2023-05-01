#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: ./sender [server_address] [port] [user_name]\n";
        return 1;
    }

    std::string host_name;
    int port;
    std::string user_name;

    // assign command line arguments to variables
    host_name = argv[1];
    port = std::stoi(argv[2]);
    user_name = argv[3];

    // TODO: connect to server
    // create a connection object
    Connection conn;
    conn.connect(host_name, port);
    // check if the connection is open
    if (!conn.is_open()) {
        std::cerr << "Failed to connect to server\n";
        return 1;
    }

    // TODO: send slogin message with user_name
    try {
        Message msg(TAG_SLOGIN, user_name);
        send(conn, msg);
        receive(conn, msg, true);
    } catch (const std::runtime_error &ex) {
        std::cerr << "Failed to Send Login Message";
        conn.close();
        exit(1);
    }

    // TODO: loop reading commands from user, sending messages to
    //       server as appropriate
    while (true) {
        std::cout << "> ";
        std::cout.flush();

        std::string input;
        std::getline(std::cin, input);

        // match tag
        Message msg2;
        if (input.substr(0, 6).compare("/join ") == 0) {
            msg2.tag = TAG_JOIN;
            msg2.data = input.substr(6);    //room host_name
        } else if (input.compare("/leave") == 0) {
            msg2.tag = TAG_LEAVE;
            msg2.data = "Bye!";
            receive(conn, msg2, true);
        } else if (input.compare("/quit") == 0) {
            msg2.tag = TAG_QUIT;
            msg2.data = "bye!";

            send(conn, msg2);
            receive(conn, msg2, true);
            break;
        } else {
            msg2.tag = TAG_SENDALL;
            msg2.data = input;
        }

        // send check
        if (!(conn.send(msg2))) {
            perror("Error while executing the given tag");
            exit(1);
        }

        // execution  depending on the check
        try {
            receive(conn, msg2, true);
        } catch (const std::runtime_error &ex) {
            std::cerr << "Runtime Error";
            continue;
        }
    }
    // close the connection
    conn.close();
    return 0;
}

