/*Jessica Nguyen + Donghee Lee
CSS432 Final Program
NetworkAPI.cpp
This program acts as the networking protocol interface for the TCP 
communication between 2 clients and server hosting the 4 in a row game.
The interface has the following functions: create TCP sockets between client/server;
send messages to client -> server (vice versa); listen to incoming messages
from client -> server (vice versa); and closing the TCP socket once done.
*/

#include <iostream>
#include <cstring>
#include <fstream>
#include <pthread.h>     // pthread
#include <sys/types.h>   // socket, bind
#include <sys/socket.h>  // socket, bind, listen, inet_ntoa
#include <netinet/in.h>  // htonl, htons, inet_ntoa
#include <arpa/inet.h>   // inet_ntoa
#include <netdb.h>       // gethostbyname
#include <unistd.h>      // read, write, close
#include <strings.h>     // bzero
#include <netinet/tcp.h> // SO_REUSEADDR
#include <sys/uio.h>     // writev
#include <sys/time.h>    //gettimeofday
#include <stdio.h>
#include <signal.h>



class NetworkAPI
{
public:
    NetworkAPI();                       //constructor
    ~NetworkAPI();                      //destructor
    bool setup4Client(char hostname[]); //helps the client connect to the server
    bool setup4Server();                // opens up port for listening, etc
    // void registerMove(int x, int y);    //listens to client move request then updates the board
    bool sendToServer(const char message[], int);
    bool sendToClient(const char message[], int);
    std::string listenFromServer();
    std::string listenFromClient();

private:
    int port;                    // this TCP port
    int clientsd;                      // this TCP socket descriptor
    int serversd;    
    struct sockaddr_in acceptSockAddr;   // my socket address for internet
    struct sockaddr_in sendSockAddr; // a destination socket address for internet
    int player1sock;
    int player2sock;
    // bool turn; //true if player 1, false if player 2;
    // string players[]; //2 slots for usernames
    // char matrix[]; //2D game board
};