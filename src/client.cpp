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
	void registerUser(string username); // saves username and sends the username to the server
	bool chooseMove(int c);				// sends the moves to the server if they are valid
	bool isGameOver();					// returns whether the game is over
	bool isMyTurn();					// returns whether it is this player's turn
	bool listenForServer();				// accepts messages from the server
	void endGame();						// displays ending messages
	void drawBoard();					// draws a board

private:
	string username;
	string pid_str; // player id either "p0" or "p1"
	int pid;
	bool gameOver;
	bool iWon;
	bool myTurn;

	int row;
	int col;
	int board[6][7];

	NetworkAPI nAPI;
};

// void signal_callback_handler(int signum)
// {
// 	std::cout << "Caught signal " << signum << std::endl;
// 	// Terminate program
// 	exit(signum);
// }

Client::Client()
{
	// signal(SIGINT, signal_callback_handler); //handle sudden CTRL-C 
	row = 6;
	col = 7;

	// initialize an empty board
	for (int r = 0; r < row; r++)
	{
		for (int c = 0; c < col; c++)
		{
			board[r][c] = Button::empty;
		}
	}

	char hostname[] = "csslab2.uwb.edu"; //testing use 127.0.0.1 or CSSlabs

	nAPI = NetworkAPI();
	nAPI.setup4Client(hostname);

	gameOver = false;
}

// saves user name and announce it to the server
void Client::registerUser(string username)
{
	username = username;
	const char *message = ("user " + username).c_str();
	nAPI.sendToServer(message);
}

// the message from server can either be "start", "gameover", or "button" message
bool Client::listenForServer()
{
	string message = nAPI.listenFromServer();

	if (message.length() > 5 && message.substr(0, 5) == "start")
	{

		// check if this player is starting first
		myTurn = message.substr(6, 3) == "yes";

		// register player id
		pid_str = message.substr(message.length() - 2);
		if (pid_str == "p0")
			pid = p0;
		else
			pid = p1;
	}
	else if (message.length() > 8 && message.substr(0, 8) == "gameover")
	{
		// check if this player won the game
		gameOver = true;
		iWon = (message.substr(9, 2) == pid_str);
	}
	else if (message.length() > 6 && message.substr(0, 6) == "button")
	{
		// update my board with the new move ack'ed by the server
		string player = message.substr(7, 2);
		int r = (int)message[10] - 48;
		int c = (int)message[12] - 48;

		if (player == pid_str)
		{ // this player's recent move got ACK'd
			// update my board
			board[r][c] = pid;
		}
		else
		{ // other player has made a move
			board[r][c] = (1 - pid);
			myTurn = true;
		}
	}

	return true;
}

// check and return whether r, c are valid moves -- do not update board here. update it when the server ACK's it
bool Client::chooseMove(int c)
{

	// check if r, c are valid
	if (c < 0 || c >= col)
		return false;

	// the column is already full
	if (board[0][c] != Button::empty)
		return false;

	// find the right place for the button
	const char *message;

	for (int r = row-1; r >= 0; r--) {
		if (board[r][c] == Button::empty) {
			message = ("move " + pid_str + " " + to_string(r) + " " + to_string(c)).c_str();
			break;
		}
	}

	
	nAPI.sendToServer(message);
	myTurn = false;

	return true;
}

// returns whether it is the player's turn
bool Client::isMyTurn()
{
	return myTurn;
}

// returns whether the game is over
bool Client::isGameOver()
{
	return gameOver;
}

// display gameover message and show the winner of the game
void Client::endGame()
{
	string message = "GAME OVER";
	if (iWon)
		message += "\n You won!\n";
	else
		message += "\n You lost.\n";
}

void Client::drawBoard() {
	cout << "" << endl;

	for (int r = 0; r < row; r++) {
		cout << "|";
		for (int c = 0; c < col; c++) {
			if (board[r][c] == b0) {
				cout << " x |";
			} else if (board[r][c] == b1){
				cout << " o |";
			} else {
				cout << "   |";
			}
		}
		cout << "" << endl;
	}
	cout << "|___________________________|" << endl;
	cout << "  1   2   3   4   5   6   7  " << endl;
}
int main()
{

	Client client = Client();

	// FIX: accept user input for user name

	string myName; // placeholder
	bool legalName = false;
	while (legalName == false)
	{
		cout << "Enter username (max 10 alphanumeric characters): ";
		cin >> myName;

		if (myName.length() > 10)
		{
			cerr << "Please only enter max 10 characters." << endl;
			continue;
		}
		for (int i = 0; i < myName.length(); i++)
		{
			if (std::isalnum(myName[i]) == 0)
			{
				cerr << "Illegal character: " << myName[i] << ". Please only enter alphanumeric characters." << endl;
				continue;
			}
		}

		legalName = true;
	}

	client.registerUser(myName);
	cout << "GAME START" << endl;

	client.drawBoard();
	// while (!client.isGameOver())
	// {
	// 	cout << "IN GAME " << endl;
	// 	client.listenForServer();
	// 	if (client.isMyTurn())
	// 	{

	// 		// accept column number to drop the tile to
	// 		int c;
	// 		cout << "\nEnter column number: ";
	// 		cin >> c;
	// 		cout << endl;
	// 		while (!client.chooseMove(c))
	// 		{	// keep asking for r, c if they were invalid choices
	// 			// prompt for user input again
	// 			cout << "\nEnter column number: ";
	// 			cin >> c;
	// 			cout << endl;
	// 		}
	// 	}
	// }

	// client.endGame();
}
