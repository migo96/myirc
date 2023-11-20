#include "EventPool.h"
#include <algorithm>

void irc::EventPool::changeEvent(int ident, int filter, int flags) {
  static struct kevent ev;
  EV_SET(&ev, ident, filter, flags, 0, 0, NULL);
  if (kevent(kqueue_, &ev, 1, NULL, 0, NULL) == -1) {
    throw std::runtime_error(std::string("kevent: ") +
                             std::string(strerror(errno)));
  }
  if (flags & EV_ADD) {
    new_events_.resize(std::min<unsigned int>(new_events_.size() + 2u, 1u << 30));
  } else if (flags & EV_DELETE) {
    new_events_.resize(std::max(static_cast<int>(new_events_.size()) - 2, 0));
  }
}

bool irc::EventPool::pollEvent(struct kevent &ev) {
  static int len;
  if (len == 0) {
    len = kevent(kqueue_, NULL, 0, &new_events_[0], new_events_.size(), NULL);
    if (len == 0) {
      return false;
    } else if (len == -1) {
      throw std::runtime_error(std::string("kevent: ") +
                               std::string(strerror(errno)));
    }
  }
  memcpy(&ev, &new_events_[--len], sizeof(struct kevent));
  return true;
}
