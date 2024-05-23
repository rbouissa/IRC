#include <iostream>
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()

class Client{
    int fd;
    std::string ip_add;
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
};

class Server{
    private:
    std::vector<Client> clients;
    int port;
    std::string pass;
    int fd_Server;
    //static bool signal;
    std::vector<struct pollfd>fds;
    public:
        Server(){
            fd_Server = -1;
        }
    int be_ready_for_connection();
    void AcceptNewConnetinClient();
    void ClearClients(int fd);
};

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
 }

int Server::be_ready_for_connection()
{
    struct sockaddr_in add;
    struct pollfd NewPoll;
    add.sin_family=AF_INET;
    this->port= 4444;
    add.sin_port=htons(this->port);
    //creating sockets 
    
   int fd = socket(AF_INET,SOCK_STREAM,0);
   if(fd==-1)
    throw(std::runtime_error("failed to create socket"));

    //active non_blocking socket
    int en =1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to reuse the address
        throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
        throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
      // Bind socket to address and port
    if (bind(fd, (struct sockaddr *)&add, sizeof(add)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }   
    // Listen for incoming connections
    if (listen(fd, SOMAXCONN) == -1) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }
    NewPoll.fd = fd; //-> add the server socket to the pollfd
    NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
    NewPoll.revents = 0; //-> set the revents to 0
    fds.push_back(NewPoll); //-> add the server socket to the pollfd
    std::cout<<"server"<<fd_Server<<" connected and ready for receiving data"<<std::endl;
    std::cout<<"Waiting to accept conection"<<std::endl;
    while(1)
    {
        if(poll(&fds[0],fds.size(),-1) ==-1)
            throw (std::runtime_error("our pool function failed"));
        for(size_t i = 0;i < fds.size();i++)
        {
            if(fds[i].revents & POLLIN)
            {
                if(fds[i].fd == fd_Server)
                {
                    std::cout<<"accepting new client"<<std::endl;
                    AcceptNewConnetinClient();
                }
                else{
                    // std::cout<<"receive a new data from a registred client"<<std::endl;
                    //ReceiveNewData();
                }
            }
        }
    }
    close(fd_Server);
}



















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
int main()
{
    
    Server s;
    s.be_ready_for_connection();
}