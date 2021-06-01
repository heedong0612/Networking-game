/*Jessica Nguyen + Donghee Lee
CSS432 Final Program
server.cpp
This programs acts as a game server to host the 4 in a row game. The server 
will be able to: accept new players, start the game, accept player input, update
the game board, and end the game if there is a winner.
*/

#include <iostream>
#include <string>
#include <cstring>
#include <time.h>
#include "NetworkAPI.h"

using namespace std;

void signal_callback_handler(int signum)
{
    std::cout << "Caught signal " << signum << std::endl;
    // Terminate program
    exit(signum);
}

enum Player {p0, p1};
enum Button {b0, b1, empty};

class Server {
public:
	Server();				// initiate connections with Network API
	NetworkAPI nAPI;

private:


};

// initilizes the Server
Server::Server() {
	cout <<" 34566";

	signal(SIGINT, signal_callback_handler); //handle sudden CTRL-C 
	cout <<" 456789";
	nAPI = NetworkAPI();
	nAPI.setup4Server();

}

// Game flow
int main(){
	cout << "HELLO";
	Server server = Server();
	const char * msg = ("helllo client!");
	server.nAPI.sendToClient(msg,0);

	//while(1) {
	//	cout << "1";
	//}
	
}

