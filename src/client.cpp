#include <iostream>
#include <string>
#include <time.h>
//#include <NetworkAPI>

using namespace std;

enum Player {p0, p1};
enum Button {b0, b1, empty};

class Client {
public:
	Client();
	void registerUser(string username);		// saves username and sends the username to the server
	bool chooseMove(int r, int c);			// sends the moves to the server if they are valid
	bool isGameOver();						// returns whether the game is over
	bool isMyTurn();						// returns whether it is this player's turn
	bool listenForServer();					// accepts messages from the server
	void endGame();							// displays ending messages

private:
	string username;
	string pid_str;							// player id either "p0" or "p1"
	int pid;
	bool isGameOver;
	bool iWon;
	bool myTurn;
	
	int row;
	int col;
	int board[6][7];		

	NetworkAPI nAPI;
};

Client::Client() {

	row = 6;
	col = 7;

	// initialize an empty board
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < col; c++) {
			board[r][c] = empty;
		}
	}

	char hostname [] = "placeholder";
	
	nAPI = NetworkAPI();
	nAPI.setup4Client(hostname);

	isGameOver = false;
}

// saves user name and announce it to the server
void Client::registerUser(string username) {
	username = username;
	const char message[] = "user " + username;
	nAPI.sendToServer(message, pid); 
}

// the message from server can either be "start", "gameover", or "button" message
bool Client::listenForServer() {
	string message = nAPI.listenFromServer();

	if (message.length() > 5 && message.substr(0,5) == "start") {		
		
		// check if this player is starting first
		myTurn = message.substr(6,3) == "yes";

		// register player id
		pid_str = message.substr(message.length() - 2);
		if (pid_str == "p0") pid = p0;
		else pid = p1;

	} else if (message.length() > 8 && message.substr(0,8) == "gameover") {
		// check if this player won the game
		isGameOver = true;
		iWon = (message.substr(9,2) == pid_str);

	} else if (message.length() > 6 && message.substr(0,6) == "button") {
		// update my board with the new move ack'ed by the server
		string player = message.substr(7,2);
		int r = (int)message[10] - 48;
		int c = (int)message[12] - 48;

		if (player == pid_str) {	// this player's recent move got ACK'd
			// update my board
			board[r][c] = pid;

		} else {					// other player has made a move
			board[r][c] = (1 - pid);
			myTurn = true;
		}
		
	}
}

// check and return whether r, c are valid moves -- do not update board here. update it when the server ACK's it 
bool Client::chooseMove(int r, int c) {
	
	// check if r, c are valid
	if (r < 0 || r >= row || c < 0 || c >= col) return false;
	if (board[r][c] != empty) return false;

	// send the moves to the server
	const char message = "move " + pid_str + " " + r + " " + c;
	nAPI.sendToServer(message, pid);
	myTurn = false;

}

// returns whether it is the player's turn
bool Client::isMyTurn() {
	return myTurn;
}

// returns whether the game is over
bool Client::isGameOver() {
	return isGameOver;
}

// display gameover message and show the winner of the game
void Client::endGame() {
	string message = "GAME OVER";
	if (iWon) message += "\n You won!\n";
	else message += "\n You lost.\n";
}

int main(){
	
	Client client = Client();
	
	// FIX: accept user input for user name
	string myName = "Donghee"; // placeholder 
	client.registerUser(myName);

	whlie (!client.isGameOver()) {
		client.listenForServer(); 
		if (client.isMyTurn()) {

			// FIX: accept user input r, c

			while (!client.chooseMove(r, c)) {	// keep asking for r, c if they were invalid choices
				// prompt for user input again
			}
		}
	}

	client.endGame();
	
}
