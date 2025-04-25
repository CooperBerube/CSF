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
};


// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

// TODO: add reciever and sender helper functions

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());
  
  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  Connection *aux = static_cast<Connection*>(arg); 

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)
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
    int clientfd = Accept(m_ssock, nullptr, nullptr);
    if (clientfd < 0) {
      // Error handling here
    }
    struct ConnInfo *aux = static_cast<struct ConnInfo *>(malloc(sizeof(struct ConnInfo)));
    aux->connection = new Connection(clientfd);
    aux->port = &m_port;
    

    pthread_t curr_thread;
    if(pthread_create(&curr_thread, nullptr, worker, aux) != 0) {/*add error handling*/};  
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
}
