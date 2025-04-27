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
struct ConnInfo {
  Connection* connection;
  const int* port;
  int clientfd;
  rio_t currClient;
};


// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());
  // NOTES: Add user object and room in this part too?
  
  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  ConnInfo *aux = static_cast<ConnInfo*>(arg); 
  Message* response;
  char buffer[Message::MAX_LEN+1];
  buffer[0] = '\0';
  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response
  ssize_t line_length = rio_readlineb(&(aux->currClient),buffer, Message::MAX_LEN + 1);
  if (line_length < 0) {
    //Error handling
  }
  buffer[line_length] = '\0';  // Null-terminate the buffer to avoid overflow
  char* colon = strchr(buffer, ':');
  //error handling
  *colon = '\0';
  char* tag = buffer;
  char* payload = colon + 1;
  User* currUser;
  char* loginTag;

  if (strlen(tag) == 0 || strlen(payload) == 0) {
    //error handling, send message to reciever or sender?
    response->tag = TAG_ERR;
    response->data = "incorrect tag";
  } else if (strcmp(tag, TAG_RLOGIN) == 0) {
    currUser = new User(payload);
    response->tag = TAG_OK;
    response->data = "recieved";
    currUser->mqueue.enqueue(response);
    loginTag = tag;
  } else if (strcmp(tag, TAG_SLOGIN) == 0) {
    currUser = new User(payload);
    response->tag = TAG_OK;
    response->data = "recieved";
    currUser->mqueue.enqueue(response);\
    loginTag = tag;
  } else {
    response->tag = TAG_ERR;
    response->data = "incorrect tag";
  }

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)

  while(1) { //communicate with sender
    if (strcmp(loginTag, TAG_RLOGIN)) { //for reciever loop

    } else if (strcmp(loginTag, TAG_SLOGIN)) { //for sender loop

    }
  }

  close(aux->clientfd);
  free(aux);

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
  int ret = pthread_mutex_init(&m_lock, nullptr);
  assert(ret = 0);
}

Server::~Server() {
  // TODO: destroy mutex
  int ret = pthread_mutex_destroy(&m_lock);
  assert(ret == 0);
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  int serverfd = open_listenfd((char *) m_port); //Review later
  if (serverfd < 0) {
    return false;
  } else {
    m_ssock = serverfd;
    return true;
  }

}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client

  while(1) {
    int clientfd = accept(m_ssock, nullptr, nullptr);
    if (clientfd < 0) {
      // Error handling here
    }
    struct ConnInfo *aux = static_cast<struct ConnInfo *>(malloc(sizeof(struct ConnInfo)));
    aux->connection = new Connection(clientfd);
    aux->port = &m_port;
    aux->clientfd = clientfd;
    rio_t in;
    rio_readinitb(&in, clientfd);
    aux->currClient = in;
    

    pthread_t curr_thread;
    if(pthread_create(&curr_thread, nullptr, worker, aux) != 0) {
      std::cerr << "Error pthread problem" << std::endl; //fix
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  auto it = m_rooms.find(room_name);
  if (it == m_rooms.end()) {
    m_rooms[room_name] = new Room(room_name); //check to see how we should alocate this
    return m_rooms[room_name];
  } else {
    return m_rooms[room_name];
  }
}
