/*Jessica Nguyen + Donghee Lee
CSS432 Final Program
NetworkAPI.cpp
This program acts as the networking protocol interface for the TCP 
communication between 2 clients and server hosting the 4 in a row game.
The interface has the following functions: create TCP sockets between client/server;
send messages to client -> server (vice versa); listen to incoming messages
from client -> server (vice versa); and closing the TCP socket once done.
*/

#define PortNum 12345
#define NULL_SD -1 // means no socket descriptor
#define MAX_BUF 60000
#include "NetworkAPI.h"

//void signal_callback_handler(int signum)
//{
//    std::cout << "Caught signal " << signum << std::endl;
//    // Terminate program
//    exit(signum);
//}

//constructor
NetworkAPI::NetworkAPI()
{
    std::cout << " in networkapi constructor" << std::endl;
    // signal(SIGINT, signal_callback_handler);
    port = PortNum;
    clientsd = NULL_SD; //initallty is -1 but will change
    serversd = NULL_SD; //initallty is -1 but will change
    player1sock = NULL_SD;
    player2sock = NULL_SD;
}

//destructor
NetworkAPI::~NetworkAPI()
{
    // Close the socket being used
    if (clientsd != NULL_SD)
    {
        close(clientsd);
    }
    if (serversd != NULL_SD)
    {
        close(serversd);
    }
}

//helps the client connect to the server
bool NetworkAPI::setup4Client(char hostname[])
{
    //obtain host
    struct hostent *host = gethostbyname(hostname); //testing use 127.0.0.1
    if (host == nullptr)
    {
        std::cerr << "Error: Failed to get host with given server name: " << std::string(hostname) << std::endl;
        return false;
    }

    //set up socket connection
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));                                         // zero out the data structure
    sendSockAddr.sin_family = AF_INET;                                                          // using IP
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list)); // sets the address to the address we looked up
    sendSockAddr.sin_port = htons(port);                                                        // set the port to connect to

    std::cout << "Connecting to port: " << port << std::endl;
    //open & connect stream-oriented socket with internet address family
    clientsd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsd < 0)
    {

        std::cerr << gai_strerror(clientsd) << "\n"
                  << "Error: Failed to establish socket" << std::endl;
        close(clientsd);
        return false;
    }

    //connect to server
    int connectStatus = connect(clientsd, (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));
    if (connectStatus < 0)
    {
        std::cerr << gai_strerror(connectStatus) << "\n"
                  << "Error: Failed to connect to the server" << std::endl;
        close(clientsd);
        return false;
    }

    return true;
}

// opens up port for listening, etc
bool NetworkAPI::setup4Server()
{
    std::cout << " in setup4server" << std::endl;
    // Bind our local address

    bzero((char *)&acceptSockAddr, sizeof(acceptSockAddr)); // Zero-initialize myAddr
    acceptSockAddr.sin_family = AF_INET;                    // Use address family internet
    acceptSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);     // Receive from any addresses
    acceptSockAddr.sin_port = htons(port);                  // Set my socket port to same w server

    //open & connect stream-oriented socket with internet address family
    serversd = socket(AF_INET, SOCK_STREAM, 0); // creates a new socket for IP using TCP
    if (serversd < 0)
    {
        std::cerr << gai_strerror(serversd) << "\n"
                  << "Error: Failed to establish socket" << std::endl;
        close(serversd);
        return false;
    }

    //set SO_REUSEADDR option -- release server port as soon as your server process is terminated
    const int on = 1;
    setsockopt(serversd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int)); // this lets us reuse the socket without waiting for hte OS to recycle it

    // Bind the socket
    int bindResult = bind(serversd, (sockaddr *)&acceptSockAddr, sizeof(acceptSockAddr)); // bind the socket using the parameters we set earlier
    if (bindResult < 0)
    {
        std::cerr << gai_strerror(bindResult) << "\n"
                  << "Error: Failed to bind the socket" << std::endl;
        close(serversd);
        return false;
    }

    listenFromClient();
    return true;
}

// //listens to client move request then updates the board
// void NetworkAPI::registerMove(int x, int y)
// {
//     std::string move = listenFromClient();

//     sendToServer(move.c_str(), clientsd);
// }

//client -> server
bool NetworkAPI::sendToServer(const char message[], int sd)
{
    std::cout << " in sendToServer" << std::endl;
    std::cout << " message is " << message << std::endl;
    int sendResult = send(clientsd, message, strlen(message), 0);
    // std::cout << " after send(clientsd)" << std::endl;
    // Couldn't send the request.
    if (sendResult <= 0)
    {
        std::cerr << gai_strerror(sendResult) << "\n"
                  << "Client unable to send the request to server" << std::endl;
        return false;
    }

    return true;
}

//server -> client
bool NetworkAPI::sendToClient(const char message[], int sd)
{
    std::cout << " in sendToClient" << std::endl;
    //makes sure listens first for client
    int sendResult = send(sd, message, strlen(message), 0);
    // std::cout << "sendResult: " << sendResult << std::endl;
    // Couldn't send the request.
    if (sendResult <= 0)
    {
        std::cerr << gai_strerror(sendResult) << "\n"
                  << "Server unable to send the request to client" << std::endl;
        exit(-1);
    }

    return true;
}

std::string NetworkAPI::listenFromServer()
{
    std::cout << " in listenFromServer" << std::endl;
    char buffer[MAX_BUF];
    memset(buffer, 0, MAX_BUF);
    int read = recv(clientsd, buffer, MAX_BUF, 0); //READ from client
    std::cout << " response is " << buffer << std::endl;
    return buffer;
}

//return whatever client passed in, set up socketplayer1 & 2
std::string NetworkAPI::listenFromClient()
{
    std::cout << " in listenFromClient" << std::endl;
    // Listen on the socket
    int n = 5;                              //connection request size
    int listenResult = listen(serversd, n); // listen on the socket and allow up to n connections to wait.
    // std::cout << " after listen(serversd)" << std::endl;
    if (listenResult != 0)
    {
        std::cerr << gai_strerror(listenResult) << "\n"
                  << "Error: Unable to listen on the socket" << std::endl;
        return "";
    }

    sockaddr_in newsockAddr; // place to store parameters for the new connection
    socklen_t newsockSize = sizeof(newsockAddr);

    //loop back to the accept command and wait for a new connection
    int newSd;
    while (1)
    {
        // Accept the connection as a new socket
        newSd = accept(serversd, (sockaddr *)&newsockAddr, &newsockSize); // grabs the new connection and assigns it a temporary socket
        // std::cout << " after accept(serversd)" << std::endl;
        if (newSd == -1)
        {
            std::cerr << gai_strerror(newSd) << "\n"
                      << "Error: Unable to connect to client." << std::endl;
            close(newSd);
            return "";
        }

        //read from client

        char buffer[MAX_BUF];
        memset(buffer, 0, MAX_BUF);
        int read = recv(newSd, buffer, MAX_BUF, 0); //READ from client
        
        std::cout << " response is " << buffer << std::endl;
        std::string response(buffer);
        if (response.substr(0, 7) == "connect" && player1sock == NULL_SD)
        {
            std::cout << " setting player1sock" << std::endl;
            player1sock = newSd;
        }
        else if (response.substr(0, 7) == "connect" && player2sock == NULL_SD)
        {
            std::cout << " setting player2sock" << std::endl;
            player2sock = newSd;
        }

        sendToClient(buffer, newSd);
    }
    close(newSd);
    return "";
}
