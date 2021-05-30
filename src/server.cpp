#include <iostream>
#include <string>
#include <time.h>
#include "NetworkAPI.h"

using namespace std;

enum Player {p0, p1};
enum Button {b0, b1, empty};

class Server {
public:
	Server();				// initiate connections with Network API
	void acceptUser();		// waits for and accepts 2 players
	void startGame();		// starts the game with one random player as the starter
	void acceptMove();		// accept a move from a player and updates and broadcast the board
	bool isGameOver();		// returns whether the game is over 
	void endGame();			// broadcasts the end of the game and the winner

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

// initilizes the Server
Server::Server() {
	row = 6;
	col = 7;

	// initialize an empty board
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			board[r][c] = Button::empty;
		}
	}

	nAPI = NetworkAPI();
	nAPI.setup4Server();

}

// waits for and accept 2 players for the game
void Server::acceptUser() {
	
	string message;

	// accept 2 players
	for (int i = 0; i < 2; i++) {
		while (true) {
			message = nAPI.listenFromClient();

			if (message.length() >= 4 && message.substr(0,4) == "user") {
				player_names[i] = message.substr(5);
				break;
			}
		}
	}
	
}

// announces the start of the game and the player who starts first
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

	const char *buffer_yes = (message_yes).c_str();
	const char *buffer_no = (message_no).c_str();

	// CHECK: should I wrap these in while loops to guarantee a send?
	nAPI.sendToClient(buffer_yes, turn); 
	nAPI.sendToClient(buffer_no, 1 - turn); 

}

// accepts a move from the player and send the updated information to both players
void Server::acceptMove() {
	
	string message = nAPI.listenFromClient();

	string turn_str = "p0";
	if (turn == p1) turn_str = "p1";

	while (true) {
		message = nAPI.listenFromClient();

		// only accept a "move" message from the player with the current turn
		if (message.length() == 11 && message.substr(0,4) == "move" && message.substr(5,2) == turn_str) {
			int r = (int)message[8] - 48;
			int c = (int)message[10] - 48;

			updateBoard(turn, r, c);

			// change player turn
			turn = 1 - turn;
			break;
		}
	}

}

// updates the current board with a new move
// x and y are assumed to be valid (not duplicate, not out of bound)
void Server::updateBoard(int player, int r, int c) {
	board[r][c] = player;
}

// check if there are any vertical, horizontal, or diagonal line of 4 consecutive buttons
bool Server::isGameOver() {

	int prev;
	int count;
	int winner;

	// vertical 
	for (int c = 0; c < col; c++) {
		prev = board[0][c];
		count = 1;

		for (int r = 1; r < row; r++) {
			if (board[r][c] == Button::empty) count = 0;
			else if (board[r][c] == prev) count += 1;
			else  count = 1;

			prev = board[r][c];

			// found a winner
			if (count == 4) {
				winner = board[r][c];
				return true;
			}
		}
	}

	// horizontal 
	for (int r = 0; r < row; r++) {
		prev = board[r][0];
		count = 1;

		for (int c = 1; c < col; c++) {
			if (board[r][c] == Button::empty) count = 0;
			else if (board[r][c] == prev) count += 1;
			else  count = 1;	

			prev = board[r][c];

			// found a winner
			if (count == 4) {
				winner = board[r][c];
				return true;
			}
		}
	}

	// diagonal up-right
	for (int r = 3; r < row; r++) {
		prev = board[r][0];
		count = 1;

		for (int c = 1; c <= r; c++) {
			if (board[r-c][c] == Button::empty) count = 0;
			else if (board[r-c][c] == prev) count += 1;
			else  count = 1;	
		
			prev = board[r-c][c];
			// found a winner
			if (count == 4) {
				winner = board[r-c][c];
				return true;
			}
		}
	}

	for (int c = 1; c <= 3; c++) {
		prev = board[5][c];
		count = 1;

		for (int r = 4; r >= (c-1); r--) {
			if (board[r][row-r+c-1] == Button::empty) count = 0;
			else if (board[r][row-r+c-1] == prev) count += 1;
			else  count = 1;	
		
			prev = board[r][row-r+c-1];
			// found a winner
			if (count == 4) {
				winner = board[r][row-r+c-1];
				return true;
			}
		}
	}

	// diganoal down-left
	for (int r = 0; r < 3; r++) {
		prev = board[r][0];
		count = 1;

		for (int c = 1; c < col-r-1; c++) {
			if (board[r+c][c] == Button::empty) count = 0;
			else if (board[r+c][c] == prev) count += 1;
			else  count = 1;	
		
			prev = board[r+c][c];
			// found a winner
			if (count == 4) {
				winner = board[r+c][c];
				return true;
			}
		}
	}


	for (int r = 1; r <= 3; r++) {
		prev = board[0][r];
		count = 1;

		for (int c = r+1; c < col; c++) {
			if (board[c-r][c] == Button::empty) count = 0;
			else if (board[c-r][c] == prev) count += 1;
			else  count = 1;	
		
			prev = board[c-r][c];
			// found a winner
			if (count == 4) {
				winner = board[c-r][c];
				return true;
			}
		}
	}

	return false;
}

// announces the end of the game and the winner of the game
void Server::endGame() {
	
	string message = "gameover";
	if (winner == p0) {
		message += " p0";
	} else {
		message += " p1";
	}

	nAPI.sendToClient(message.c_str(), p0); 
	nAPI.sendToClient(message.c_str(), p1); 
}

// Game flow
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
