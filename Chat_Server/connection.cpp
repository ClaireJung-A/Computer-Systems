#include <sstream>
#include <cctype>
#include <cassert>
#include <iostream>
#include "csapp.h"
#include "message.h"
#include "connection.h"

Connection::Connection()
    : m_fd(-1), m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
    : m_fd(fd), m_last_result(SUCCESS)
{
  rio_readinitb(&m_fdbuf, m_fd);
}

void Connection::connect(const std::string &hostname, int port) {
  const char *h = hostname.c_str();
  const char *portArray = std::to_string(port).c_str();
  m_fd = open_clientfd(h, portArray);

  if (m_fd < 0) {
      perror("Cannot Connect to Server");
      exit(-1);
  }

  //  initialize the rio_t
  rio_readinitb(&m_fdbuf, m_fd);
}

Connection::~Connection(){
  close();
}

bool Connection::is_open() const {
  return m_fd > 0;
}

void Connection::close() {
  // close the connection if it is open
  if (is_open()) {
    Close(m_fd);
  }
}

bool Connection::send(const Message &msg) {
    // check if the length of the message data is valid
    if (strlen(msg.data.c_str()) > Message::MAX_LEN) {
        m_last_result = INVALID_MSG;
        return false;
    }

    // get the size of the message and convert it to a string
    ssize_t msg_size = msg.to_str().size();
    std::string message = msg.to_str();
    const char *message_num = message.c_str();
    // send the message through the file descriptor
    ssize_t tot_size = rio_writen(m_fd, message_num, msg_size);

    // check if the entire message was sent
    if (tot_size == msg_size) {
        m_last_result = SUCCESS;
    } else {
        m_last_result = EOF_OR_ERROR;
    }
    return tot_size == msg_size;
}

bool Connection::receive(Message &msg) {
    char buffer[Message::MAX_LEN + 1];

    // read a line from the file descriptor
    ssize_t bytes_read = rio_readlineb(&m_fdbuf, buffer, Message::MAX_LEN);
    if (bytes_read < 0) {
        m_last_result = EOF_OR_ERROR;
        return false;
    }

    // convert the buffer to a string
    std::string msg_long(buffer);
    msg.data = msg_long;

    // extract the tag and data from the message
    std::vector<std::string> result = msg.detectColon();
    msg.tag = result[0];

    if (result.size() == 1) {
        msg.data = "";
    } else if (msg.tag == TAG_DELIVERY) {
        msg.data = result[1] + ":" + result[2] + ":" + result[3];
    } else {
        msg.data = result[1];
    }

    m_last_result = SUCCESS;
    return true;
}



