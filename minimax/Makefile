all: game_project

game_project: Bot.o main.o gameplay.o

Bot.o: Bot.cpp
	g++ -c Bot.cpp -o Bot.o

main.o: main.cpp
	g++ -c main.cpp -o main.o

gameplay.o: gameplay.cpp
	g++ -c gameplay.cpp -o gameplay.o