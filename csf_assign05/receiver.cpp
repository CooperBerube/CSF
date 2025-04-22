#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;
  try {
    // Attempt to establish a connection to the server using the provided hostname and port
    conn.connect(server_hostname, server_port);
  } catch (const std::exception &e) {
    // Handle connection failure and print the error message
    std::cerr << "Error connecting to server: " << e.what() << "\n";
    return 1;
  }

  // Send the RLOGIN message and handle server response
  // RLOGIN is typically used for authentication or session initialization
  conn.send(Message(TAG_RLOGIN, username));
  Message message;
  conn.receive(message);

  // Handle potential errors in the RLOGIN response
  if (message.tag == TAG_ERR) {
    // Print the error message received from the server
    std::cerr << message.data;
    return 1;
  }
  if (message.tag != TAG_OK) {
    // Ensure the server sent an OK response; otherwise, terminate with an error
    std::cerr << "Error: Unexpected message from the server. No OK received.\n";
    return 1;
  }

  // Send the JOIN message to join a specific room or channel
  conn.send(Message(TAG_JOIN, room_name));

  // Handle the server's response to the JOIN message
  if (!conn.receive(message)) {
    // Check if a response was received from the server
    std::cerr << "Failed to receive response after JOIN.\n";
    return 1;
  }
  if (message.tag == TAG_ERR) {
    // Print the error message if the server reports an issue
    std::cerr << message.data;
    return 1;
  }
  if (message.tag != TAG_OK) {
    // Ensure the server sent an OK response; otherwise, terminate with an error
    std::cerr << "Error: Unexpected message from the server. No OK received.\n";
    return 1;
  }

  // Start a loop to wait for and process messages from the server
  // The expectation is to receive messages tagged with TAG_DELIVERY
  while (true) { 
    // Receive a message from the server
    conn.receive(message);
    if (message.tag != TAG_DELIVERY) {
      // Handle unexpected messages from the server
      std::cerr << "Unexpected message: " << message.data << "\n";
    } else {
      // Process a delivery message, which contains sender and message content
      char *content = (char *) message.data.c_str();
      strtok_r(content, ":", &content);  // Parse message content
      char *sender = strtok_r(content, ":", &content);  // Extract sender
      char *msg = strtok_r(content, ":", &content);  // Extract actual message
      std::cout << sender << ": " << msg;  // Display the message
    }
  }
    return 0;
}