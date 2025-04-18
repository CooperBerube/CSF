#include <sstream>
#include <cctype>
#include <cassert>
#include <iostream>
#include "csapp.h"
#include "message.h"
#include "connection.h"

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, fd); 
}

void Connection::connect(const std::string &hostname, int port) {
  // TODO: call open_clientfd to connect to the server
  // TODO: call rio_readinitb to initialize the rio_t object
  int fd = open_clientfd(hostname.c_str(), std::to_string(port).c_str());
  if (fd < 0) {
      std::cerr << "Error: unable to connect to server\n";
      exit(EOF_OR_ERROR); // Handle connection failure
  }

  // Initialize member variables
  m_last_result = SUCCESS;
  m_fd = fd;

  // Initialize the rio_t object
  rio_readinitb(&m_fdbuf, fd);
}

Connection::~Connection() {
  // TODO: close the socket if it is open
  if (is_open()) {
    close();
  }
}

bool Connection::is_open() const {
  // TODO: return true if the connection is open
  return m_fd >= 0;
}

void Connection::close() {
  // TODO: close the connection if it is open
  if (is_open()) {
    Close(m_fd);
  }
}

bool Connection::send(const Message &msg) {
  // TODO: send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  std::string buf = msg.tag + ":" + msg.data + "\n";
  ssize_t total_written = 0;
  ssize_t buf_size = buf.length();

  while (total_written < buf_size) {
    ssize_t n = rio_writen(m_fd, buf.c_str() + total_written, buf_size - total_written);
    if (n > 0) {
      total_written += n;
    } else {
      if (errno == EINTR) {
        // Retry if interrupted by a signal
        continue;
      } else {
        m_last_result = EOF_OR_ERROR;
        return false;
      }
    }
  }

  m_last_result = SUCCESS;
  return true;
}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  char buf[Message::MAX_LEN + 1];
  buf[0] = '\0';  // Ensure the buffer is null-terminated
  ssize_t n = rio_readlineb(&m_fdbuf, buf, Message::MAX_LEN + 1);

  if (n <= 0) {  // Check for EOF or error
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  buf[n] = '\0';  // Null-terminate the buffer to avoid overflow

  // Find the first colon (:) as the delimiter
  char* colon = strchr(buf, ':');
  if (colon == NULL) {
    m_last_result = INVALID_MSG;
    return false;
  }

  // Split the message into tag and data
  *colon = '\0';  // Replace colon with null terminator
  const char* tag = buf;
  const char* data = colon + 1;

  // Validate tag and data
  if (strlen(tag) == 0 || strlen(data) == 0) {
    m_last_result = INVALID_MSG;
    return false;
  }

  msg.tag = tag;
  msg.data = data;
  m_last_result = SUCCESS;
  return true;
}