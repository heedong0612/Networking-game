/*Jessica Nguyen + Donghee Lee
CSS432 Final Program
server.cpp
This programs acts as a game server to host the 4 in a row game. The server 
will be able to: accept new players, start the game, accept player input, update
the game board, and end the game if there is a winner.
*/

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
	string player_names[2];

private:
	void updateBoard(int turn, int x, int y);

	int row;
	int col;
	int board[6][7];		
	int turn; // indicates who's turn it is
	int winner;

	
	string ip_addresses[2];

	NetworkAPI nAPI;

};

// void signal_callback_handler(int signum)
// {
//     std::cout << "Caught signal " << signum << std::endl;
//     // Terminate program
//     exit(signum);
// }

// initilizes the Server
Server::Server() {
	// signal(SIGINT, signal_callback_handler); //handle sudden CTRL-C 
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
		while (true) { // while loop for grabbing only "user ..." message
			message = nAPI.listenFromClient(i);
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
	
//	string message = nAPI.listenFromClient(turn);

	string turn_str = "p0";
	if (turn == p1) turn_str = "p1";

	while (true) {
		cout << "message from client " << endl;
		string message = nAPI.listenFromClient(turn);
		cout << message << endl;
		// only accept a "move" message from the player with the current turn
		if (message.length() == 11 && message.substr(0,4) == "move" && message.substr(5,2) == turn_str) {
			int r = (int)message[8] - 48;
			int c = (int)message[10] - 48;

			updateBoard(turn, r, c);

			// send the recent valid move  to both players
			string moveMessage = "button " + turn_str + " " + message[8] + " "  + message[10];
			if (isGameOver()) {
				if (winner == p0) moveMessage += " gameover p0";
				else moveMessage += " gameover p1";
			}
			const char *recent_move_message = moveMessage.c_str();
			nAPI.sendToClient(recent_move_message, p0);
			nAPI.sendToClient(recent_move_message, p1);

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
				cout << board[r][c] << endl;
				cout << "win 1" << endl;
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
				cout << "win 2" << endl;
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
				cout << "win 3" << endl;
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
				cout << "win 4" << endl;
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
				cout << "win 5" << endl;
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
				cout << "win 6" << endl;
				return true;
			}
		}
	}

	return false;
}

// announces the end of the game and the winner of the game
void Server::endGame() {
	
	string message = "gameover";
	// cout << "winner message: " << message << endl;
	cout << "WINNER: " << winner << endl;
	if (winner == p0) {
		message += " p0";
	} else {
		message += " p1";
	}
	cout << "winner message: " << message << endl;

	nAPI.sendToClient(message.c_str(), p0); 
	nAPI.sendToClient(message.c_str(), p1); 
}

// Game flow
int main(){
	Server server = Server();

	// accept 2 users to start the game
	server.acceptUser();
	cout <<"[All players reigstered]" << endl;
	cout <<"Players: " << server.player_names[0] << ", " << server.player_names[1] << endl;
	server.startGame(); // 1 send to each client (about turns)

	// continue the game until there is a winner
	while (!server.isGameOver()) {
		server.acceptMove();
	}
	
	// // Game over. announce the winner and the end of the Game
	server.endGame();

}
