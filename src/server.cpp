#include <iostream>
#include <string>
#include <time.h>
//#include <NetworkAPI>

using namespace std;

enum Player {p0, p1};
enum Button {b0, b1, empty};

class Server {
public:
	Server();
	void acceptUser();
	void startGame();
	void acceptMove();
	bool isGameOver();
	void endGame(); 

private:
	void updateBoard(int turn, int x, int y);

	int row;
	int col;
	int board[6][7]; 
	int turn; // indicates who's turn it is
	int winner;

	string player_names[2];
	string ip_addresses[2];

	NetworkAPI nAPI;

};

Server::Server() {
	row = 6;
	col = 7;

	nAPI = NetworkAPI();
	nAPI.setup4Server();

}

void Server::acceptUser() {
	
	string message;

	// accept 2 players
	for (int i = 0; i < 2; i++) {
		while (true) {
			message = nAPI.listenFromClient();

			if (message.length() >= 4 && message.substr(0,4) == "user") {
				names[i] = message.substr(5);
				break;
			}
		}
	}
	
}

void Server::startGame() {

	// a random int between 0 and 1
	srand( time(NULL) );
	turn = rand()%2; 

	// a message indicating which player starts
	string message_yes = "start yes";
	string message_no = "start no";

	if (turn == p0) {
		message_yes += " p0";
		message_no += " p1";
	} else {
		message_yes += " p1";
		message_no += " p0";
	}

	const char buffer_yes[] = message_yes;
	const char buffer_no[] = message_no;

	// CHECK: should I wrap these in while loops to guarantee a send?
	nAPI.sendToClient(buffer_yes, turn); 
	nAPI.sendToClient(buffer_no, 1 - turn); 

}

// accepts a move from the player and send the updated information to both players
void Server::acceptMove() {
	
	string message = listenFromClient();

	string turn_str = "p0";
	if (turn == p1) turn_str = "p1";

	while (true) {
		message = nAPI.listenFromClient();

		// only accept a "move" message from the player with the current turn
		if (message.length() == 11 && message.substr(0,4) == "move" && message.substr(5,2) == turn_str) {
			int x = (int)message[8] - 48;
			int y = (int)message[10] - 48;

			updateBoard(turn, x, y);

			// change player turn
			turn = 1 - turn;
			break;
		}
	}

}

// x and y are assumed to be valid (not duplicate, not out of bound)
void Server::updateBoard(int player, int x, int y) {
	board[x][y] = player;
}

// check if there are any vertical, horizontal, or diagonal line of 4 consecutive buttons
bool Server::isGameOver() {
	// vertical 

	// horizontal 

	// diagonal

	// winner = p0;

}

// announces the end of the game and the winner of the game
void Server::endGame() {
	
	string message = "gameover";
	if (winner == p0) {
		message += " p0";
	} else {
		message += " p1";
	}

	nAPI.sendToClient(message, p0); 
	nAPI.sendToClient(message, p1); 
}

int main(){
	Server server = Server();

	// accept 2 users to start the game
	server.acceptUser();
	server.startGame();

	// continue the game until there is a winner
	while (!server.isGameOver()) {
		server.acceptMove();
	}
	
	// Game over. announce the winner and the end of the Game
	server.endGame();

}
