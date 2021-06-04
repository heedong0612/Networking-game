# Online game: Connect 4
Connect 4 is a two-player online board game where the players take turns dropping tiles on a seven-column, six-row grid.
Users can play this game with the opponent on terminal by running the client program.


<p align="center">
  <img src="https://github.com/heedong0612/OnlineGame-Connect4/blob/main/res/waitingRoom.png" width="300" height="200">>
</p>

<p align="center">
  <img src="https://github.com/heedong0612/OnlineGame-Connect4/blob/main/res/inGame1.png">
</p>

<p align="center">
  <img src="https://github.com/heedong0612/OnlineGame-Connect4/blob/main/res/inGame2.png">
</p>

## Instruction
The server needs to be running to host the game. Compile and run server 
```
g++ server.cpp NetworkAPI.cpp -o server
./server
```
The clients can now join the game by compiling and running with the server hostname
```
g++ client.cpp NetworkAPI.cpp -o client
./client {hostname}
```

For example, if the server is hosted on csslab1.uwb.edu, run
```
./client csslab1.uwb.edu
```

## architecture
client-server architecture

## Network Protocol
TCP/IP, NetworkAPI.cpp

