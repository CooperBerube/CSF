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
  // Initialize the rio_t object for buffered I/O operations
  rio_readinitb(&m_fdbuf, fd); 
}

void Connection::connect(const std::string &hostname, int port) {
  // Call open_clientfd to establish a connection to the server
  int fd = open_clientfd(hostname.c_str(), std::to_string(port).c_str());
  if (fd < 0) {
      // Handle connection failure by printing an error message and exiting
      std::cerr << "Error: unable to connect to server\n";
      exit(EOF_OR_ERROR);
  }

  // Initialize member variables after a successful connection
  m_last_result = SUCCESS;
  m_fd = fd;

  // Initialize the rio_t object for buffered I/O operations
  rio_readinitb(&m_fdbuf, fd);
}

Connection::~Connection() {
  // Check if the connection is open and close it before destruction
  if (is_open()) {
    close();
  }
}

bool Connection::is_open() const {
  // Return true if the file descriptor is valid and the connection is open
  return m_fd >= 0;
}

void Connection::close() {
  // Close the connection if it is open
  if (is_open()) {
    Close(m_fd);  // Use the wrapper function Close from csapp for safety
    m_fd = -1;    // Reset the file descriptor to indicate the connection is closed
  }
}

bool Connection::send(const Message &msg) {
  // Prepare the message as a string in the format: "tag:data\n"
  std::string buffer = msg.tag + ":" + msg.data + "\n";
  ssize_t total_written = 0;
  ssize_t buffer_size = buffer.length();

  // Write the message to the socket in a loop to handle partial writes
  while (total_written < buffer_size) {
    ssize_t n = rio_writen(m_fd, buffer.c_str() + total_written, buffer_size - total_written);
    if (n > 0) {
      // Increment the total bytes written
      total_written += n;
    } else {
      // Handle errors during the write operation
      if (errno == EINTR) {
        // Retry if the operation was interrupted by a signal
        continue;
      } else {
        // Set the last result to indicate an error and return false
        m_last_result = EOF_OR_ERROR;
        return false;
      }
    }
  }

  // Set the last result to SUCCESS if the message was sent completely
  m_last_result = SUCCESS;
  return true;
}

bool Connection::receive(Message &msg) {
  // Buffer to store the received message, ensuring it is null-terminated
  char buffer[Message::MAX_LEN + 1];
  buffer[0] = '\0';  // Initialize the buffer
  ssize_t line_length = rio_readlineb(&m_fdbuf, buffer, Message::MAX_LEN + 1);

  // Check for EOF or error in reading
  if (line_length <= 0) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  buffer[line_length] = '\0';  // Null-terminate the buffer to avoid overflow

  // Locate the first colon (:) in the message, which separates tag and data
  char* colon = strchr(buffer, ':');
  if (colon == NULL) {
    // If no colon is found, the message format is invalid
    m_last_result = INVALID_MSG;
    return false;
  }

  // Split the message into tag and data
  *colon = '\0';  // Replace colon with null terminator
  const char* tag = buffer;       // The tag is the portion before the colon
  const char* data = colon + 1; // The data is the portion after the colon

  // Validate that both tag and data are present
  if (strlen(tag) == 0 || strlen(data) == 0) {
    m_last_result = INVALID_MSG;
    return false;
  }

  // Populate the Message object with the received tag and data
  msg.tag = tag;
  msg.data = data;

  // Set the last result to SUCCESS to indicate a successful receive
  m_last_result = SUCCESS;
  return true;
}