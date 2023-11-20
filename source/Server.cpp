# include "Server.h"


namespace irc {

void Server::setPort(std::string port) {
  unsigned long size;
  port_ = 0;
  
  for (size = 0; size < port.size(); ++size) {
    if (port.size() > 5 || !isdigit(port[size])) {
      break ;
    }
    port_ = port_ * 10 + static_cast<int>(port[size]) - 48;
  }
  if (size != port.size() || port_ > 65535) {
    throw std::runtime_error(std::string("port error\n"));
  }
}

void Server::setPassword(std::string password) {
  unsigned long size;

  for (size = 0; size < password.size(); ++size) {
    if (!isprint(password[size])) {
      break ;
    }
  }
  if (size != password.size()) {
    throw std::runtime_error(std::string("password error\n"));
  }
  password_ = password;
}

int Server::getSocket() const{
  return server_socket_;
}

Connections &Server::getConnections() {
  return connections_;
}

Client &Server::getClient (int socket) {
  return connections_[socket];
}

void Server::standby() {
  int Server_socket;
  struct sockaddr_in server_addr;
  int flag = 1;

  if ((Server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    throw std::runtime_error(std::string("socket() error\n") + 
                             std::string(strerror(errno)));
  }
  setsockopt(Server_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)); //프로그램이 종료되어도 8080port가 해제되지 않는 현상 없애기 위해 필요
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family      = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port        = htons(port_);
  fcntl(Server_socket, F_SETFL, O_NONBLOCK);
  if (bind(Server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    throw std::runtime_error(std::string("bind() error\n") + 
                             std::string(strerror(errno)));
  }
  if (listen(Server_socket, MAX_BACKLOG) == -1) {
    throw std::runtime_error(std::string("listen() error\n") + 
                             std::string(strerror(errno)));
  }
  server_socket_ = Server_socket;
}

void Server::preProcess() {

}

void Server::disconnect(int socket) {
  // nick_to_socket_.erase(connections_[socket].getNickname());
  connections_.erase(socket);
  close (socket);
}

int Server::accept() {
  int socket = ::accept(server_socket_, NULL, NULL);
  if (socket == -1) {
    throw std::runtime_error(std::string("accept() error\n") + 
                             std::string(strerror(errno)));
  }
  fcntl(socket, F_SETFL, O_NONBLOCK);
  connections_[socket];
  return socket;
}

std::string Server::getPassword() const {
  return password_;
}

int Server::samename(std::string nickname) const {
  Connections::const_iterator iter;
  int num = 0;

  for (iter = connections_.begin(); iter != connections_.end(); ++iter) {
    if (iter->second.getNickname() == nickname)
      num++;
  }
  std::cout << num << std::endl;
  return num;
}

bool Server::setChannel(std::string name, std::string password, std::string nick) {
  if (sev_channel_.find(name) == sev_channel_.end()) {
    sev_channel_[name];
    sev_channel_[name].inputOp(nick);
    sev_channel_[name].inputCh(nick);
    sev_channel_[name].setPassword(password);
    return true;

  }
  else {
    return false;
  }
}

Channel &Server::getChannel(std::string name) {
  return sev_channel_[name];
}

bool Server::canChannel(std::string name) {
  if (sev_channel_.find(name) != sev_channel_.end()) {
    return true;
  }
  else {
    return false;
  }
}

void Server::mangeCh(std::string name) {
  std::vector<std::string> ch_nick_ = sev_channel_[name].getChNick();

  if (ch_nick_.size() == 0) {
    sev_channel_.erase(name);
    std::cout << "channel size " << sev_channel_.size() << std::endl;
  }
}

int Server::nicktosocket(std::string nick) {
  Connections::const_iterator iter;

  for (iter = connections_.begin(); iter != connections_.end(); ++iter) {
    if (iter->second.getNickname() == nick) {
      return iter->first;
    }
  }
  return 0;
}

std::string Server::getChTopic(std::string chname) {
  return sev_channel_[chname].getTopic();
}

void Server::setChTopic(std::string chname, std::string topic) {
  sev_channel_[chname].setTopic(topic);
}

}