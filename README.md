# Online game: Connect 4
Connect 4 is a two-player online board game where the players take turns dropping tiles on a seven-column, six-row grid.
Users can play this game with the opponent on terminal by running the client program.

<p align="center">
  <img src="https://github.com/heedong0612/OnlineGame-Connect4/blob/main/res/merged_screenshot.png">
</p>

## Instruction
The server needs to be running to host the game. Compile and run server 
```
g++ server.cpp NetworkAPI.cpp -o server
./server
```
The clients can now join the game by compiling and running
```
g++ client.cpp NetworkAPI.cpp -o client
./client
```

## architecture
client-server architecture

## Network Protocol
TCP/IP, NetworkAPI.cpp

