#include <iostream>
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <cstring>//for memset
#include<sstream>


class Client{
    int fd;
    std::string ip_add;
    std::string Nickname;
    std::string Username;
    std::string Password;
      bool hasPassword;
    bool hasNickname;
    bool hasUsername;
    public:
        int getFd()
        {
            return fd;
        };
        void SetFd(int Fd)
        {
            fd = Fd;
        }
        void SetIppAdd(std::string ip)
        {
            ip_add = ip;

        }
       
    std::string getNickname() { return Nickname; }
    void setNickname(std::string nick) { Nickname = nick; }
    std::string getUsername() { return Username; }
    void setUsername(std::string user) { Username = user; }
    std::string getPassword() { return Password; }
    void setPassword(std::string pass) { Password = pass; }
    bool hasPasswordReceived() { return hasPassword; }
    bool hasNicknameReceived() { return hasNickname; }
    bool hasUsernameReceived() { return hasUsername; }
};

class Server{

    private:
    std::vector<Client> clients;
    
    
    int fd_Server;
    //static bool signal;
    std::vector<struct pollfd>fds;
    public:
    int port;
    int count;
    std::string pass;
        Server(){
            fd_Server = -1;
            count = 0;
        }
    int be_ready_for_connection();
    void AcceptNewConnetinClient();
    void ReceiveNewData(int fd);
    void ClearClients(int fd);
    void sendToClient(int fd, const std::string& message);
    void parseClientInput(int fd, const std::string& data);
};

void Server::sendToClient(int fd, const std::string& message) {
    send(fd, message.c_str(), message.size(), 0);
}


//for the hexchat
// void Server::parseClientInput(int fd, const std::string& data) {
//     std::istringstream stream(data);
//     std::string line;
//     while (std::getline(stream, line)) {
//         std::istringstream linestream(line);
//         std::string command;
//         linestream >> command;

//         if (command == "PASS") {
//             std::string pass;
//             linestream >> pass;

//             // Find the client and set the password
//             for (auto& client : clients) {
//                 if (client.getFd() == fd) {
//                     client.setPassword(pass);
//                     std::cout<<"our password "<<pass<<std::endl;
//                     break;
//                 }
//             }
//         } else if (command == "NICK") {
//             std::string nick;
//             linestream >> nick;

//             // Find the client and set the nickname
//             for (auto& client : clients) {
//                 if (client.getFd() == fd) {
//                     client.setNickname(nick);
//                     std::cout<<"our Nickname "<<nick<<std::endl;
//                     break;
//                 }
//             }
//         } else if (command == "USER") {
//             std::string user, mode, unused, realname;
//             linestream >> user >> mode >> unused;
//             std::getline(linestream, realname);
//             if (!realname.empty() && realname[0] == ':') {
//                 realname = realname.substr(1); // Remove the leading colon
//             }

//             // Find the client and set the username and realname
//             for (auto& client : clients) {
//                 if (client.getFd() == fd) {
//                     client.setUsername(user);
//                     std::cout<<"our Username "<<user<<std::endl;
//                    // client.setRealname(realname);
//                     break;
//                 }
//             }
//         }
//     }
// }




void Server::AcceptNewConnetinClient(){
    Client new_client;
    struct sockaddr_in client_add;
    struct pollfd clientPoll;
    socklen_t length = sizeof(client_add);
    int accept_cl = accept(fd_Server,(sockaddr *)&(client_add),&length);
     if (accept_cl == -1)
  {std::cout << "client cannot connect" << std::endl; return;}
    clientPoll.fd=accept_cl;
    clientPoll.events=POLLIN;
    clientPoll.revents=0;
    new_client.SetFd(accept_cl); //-> set the client file descriptor
 new_client.SetIppAdd(inet_ntoa((client_add.sin_addr))); //-> convert the ip address to string and set it
 clients.push_back(new_client); //-> add the client to the vector of clients
 fds.push_back(clientPoll); //-> add the client socket to the pollfd

 std::cout <<"client connected seccefully" << std::endl;
 // Send IRC welcome messages
    std::string welcomeMsg = ":myserver 001 " + new_client.getNickname()  + " :Welcome to the IRC server\r\n";
    std::string yourHostMsg = ":myserver 002 " + std::string(inet_ntoa(client_add.sin_addr)) + " :Your host is myserver\r\n";
    std::string createdMsg = ":myserver 003 " + std::string(inet_ntoa(client_add.sin_addr)) + " :This server was created just now\r\n";
    std::string myInfoMsg = ":myserver 004 " + std::string(inet_ntoa(client_add.sin_addr)) + " myserver v1.0 i\r\n";

    sendToClient(accept_cl, welcomeMsg);
    sendToClient(accept_cl, yourHostMsg);
    sendToClient(accept_cl, createdMsg);
    sendToClient(accept_cl, myInfoMsg);
 }

void Server::ReceiveNewData(int fd)
{
    //this is the buff thata we wiil store our data received in
	char buff[1024]; 
    Client c;

    //clear the buffer
	memset(buff, 0, sizeof(buff)); 
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0); 
    //if the client disconnected
	if(bytes <= 0){ 
		std::cout  << "Client <" << fd << "> Disconnected" << std::endl;
	
		close(fd);
	}
//if not print the data received
	else{ 
		buff[bytes] = '\0';
        std::string data(buff);
        
        parseClientInput(fd, data);
		std::cout << "Client <" << fd_Server << "> Data: "  << buff;
	}
}

int Server::be_ready_for_connection()
{
    struct sockaddr_in add;
    struct pollfd NewPoll;
    add.sin_family=AF_INET;
    add.sin_port=htons(this->port);
   this->fd_Server = socket(AF_INET,SOCK_STREAM,0);
   if( this->fd_Server==-1)
    throw(std::runtime_error("failed to create socket"));

    //active non_blocking socket
    int en =1;
    if(setsockopt( this->fd_Server, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to reuse the address
        throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
    if (fcntl( this->fd_Server, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
        throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
      // Bind socket to address and port
    if (bind( this->fd_Server, (struct sockaddr *)&add, sizeof(add)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }   
    // Listen for incoming connections
    if (listen( this->fd_Server, SOMAXCONN) == -1) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }
    NewPoll.fd =  this->fd_Server; //-> add the server socket to the pollfd
    NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
    NewPoll.revents = 0; //-> set the revents to 0
    fds.push_back(NewPoll); //-> add the server socket to the pollfd
    std::cout<<"server"<< this->fd_Server<<" connected and ready for receiving data"<<std::endl;
    std::cout<<"Waiting to accept conection"<<std::endl;
    while(1)
    {
        if(poll(&fds[0],fds.size(),-1) ==-1)
            throw (std::runtime_error("our pool function failed"));
        for(size_t i = 0;i < fds.size();i++)
        {
            if(fds[i].revents & POLLIN)
            {
                if(fds[i].fd ==  this->fd_Server)
                {
                    std::cout<<"accepting new client"<<std::endl;
                    AcceptNewConnetinClient();
                }
                else{
                     
                    ReceiveNewData(fds[i].fd);
                    //std::cout<<"receive a new data from a registred client"<<std::endl;
                }
            }
        }
    }
    close(this->fd_Server);
}









void Server::parseClientInput(int fd, const std::string& data) {
    std::istringstream stream(data);  // Create an input string stream from the data string
    std::string line;
    while (std::getline(stream, line)) {  // Read each line from the stream
        std::istringstream linestream(line);  // Create an input string stream from the line
        std::string command;
        linestream >> command;  // Read the command from the line

        for (auto& client : clients) {
            if (client.getFd() == fd) {
                if (command == "CAP")
                {
                    std::string pass = "Please enter your password:\r\n";
                    send(fd, pass.c_str(), pass.size(), 0);
                }
                if (!client.hasPasswordReceived() && command == "PASS" && count ==0) {
                    std::string pass;
                    linestream >> pass;  // Read the password
                    client.setPassword(pass);
                    count =1;

                    // Prompt for nickname after receiving password
                    std::string nicknamePrompt = "Please enter your nickname:\r\n";
                    send(fd, nicknamePrompt.c_str(), nicknamePrompt.size(), 0);
                } else if (client.hasPasswordReceived() && !client.hasNicknameReceived() && command == "NICK"&&count ==1) {
                    std::string nick;
                    linestream >> nick;  // Read the nickname
                    client.setNickname(nick);

                    // Prompt for username after receiving nickname
                    std::string usernamePrompt = "Please enter your username:\r\n";
                    send(fd, usernamePrompt.c_str(), usernamePrompt.size(), 0);
                    count=2;
                } else if (client.hasPasswordReceived() && client.hasNicknameReceived() && !client.hasUsernameReceived() && command == "USER"&&count ==2) {
                    std::string user, mode, unused, realname;
             
                    client.setUsername(user);
                    // client.setRealname(realname);

                    // Client setup is complete, you can now proceed with further handling
                    std::string welcomeMessage = ":myserver 001 " + client.getNickname()  + " :Welcome to the IRC server\r\n";
                    send(fd, welcomeMessage.c_str(), welcomeMessage.size(), 0);
                }
                }
                break;
            }
        }}
 



















void Server::ClearClients(int fd){ //-> clear the clients
 for(size_t i = 0; i < fds.size(); i++){ //-> remove the client from the pollfd
  if (fds[i].fd == fd)
   {fds.erase(fds.begin() + i); break;}
 }
 for(size_t i = 0; i < clients.size(); i++){ //-> remove the client from the vector of clients
  if (clients[i].getFd() == fd)
   {clients.erase(clients.begin() + i); break;}
 }

}
int main(int ac,char **av)
{
    if(ac!=3)
    {
        std::cout<<"invalide arguments";
        return 0;
    }
    
    Server s;
    s.port=atoi(av[1]);
    s.pass=av[2];
    s.be_ready_for_connection();
}