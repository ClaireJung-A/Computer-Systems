#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <set>
#include <pthread.h>

struct User;

// A Room object is a representation of a chat room.
// At a minimum, it should keep track of the User objects representing
// receivers who have joined the room.
class Room {
public:
  Room(const std::string &room_name);
  ~Room();

  void add_member(User *user);
  void remove_member(User *user);

  void broadcast_message(const std::string &sender_username, const std::string &message_text);

private:
  const std::string room_name;
  pthread_mutex_t lock;

  typedef std::set<User *> UserSet;
  UserSet members; // receivers
};

#endif // ROOM_H


