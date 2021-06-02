/*Jessica Nguyen + Donghee Lee
CSS432 Final Program
client.cpp
This program acts as clients connecting to a game server to play 4 in a row.
The client will be able to register their username, choose their move, take turns
playing, and disconnect from the game.
*/

#include <iostream>
#include <string>
#include <time.h>
#include "NetworkAPI.h"

using namespace std;

void signal_callback_handler(int signum)
{
	std::cout << "Caught signal " << signum << std::endl;
	// Terminate program
	exit(signum);
}

enum Player
{
	p0,
	p1
};
enum Button
{
	b0,
	b1,
	empty
};

class Client
{
public:
	Client();
	NetworkAPI nAPI;
private:

	
};


Client::Client()
{
	signal(SIGINT, signal_callback_handler); //handle sudden CTRL-C 

	nAPI = NetworkAPI();
	char hostname []= "127.0.0.1";
	nAPI.setup4Client(hostname);

}

int main()
{
	cout << "HEL:O";
	Client client = Client();
	const char *intro = "connect";
	client.nAPI.sendToServer(intro);
	string msg = client.nAPI.listenFromServer();
	cout << "Received: " << msg << endl;

	intro = "hello from client to server";
	client.nAPI.sendToServer(intro);
}

