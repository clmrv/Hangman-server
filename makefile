all:
	c++ -std=c++17 -o server Hangman-server/*.cpp Hangman-server/Connection/*.cpp
