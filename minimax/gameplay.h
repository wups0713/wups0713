#define _Table_Size 8
#define Black 1
#define White -1
#include <iostream>

class Player
{
	public:
		Player (int);
		~Player ();
		
		bool start (short int[_Table_Size][_Table_Size]);
		void select_chess ();					//select a chess to move
		bool control (bool);					//control the chess to jump or move
		bool do_move (int, bool);
		bool print (short int[_Table_Size][_Table_Size], int, int);
		bool end_game ();
		
		bool can_move (int);
		bool can_jump (int);
		bool is_chess (int, int);
		bool in_bound (int, int);
		
	private:
		int round;
		int x_t, y_t;						//now position
		int b_or_w;							//player is Black or White
		bool move[4];						//if the chess can move(four direction)
		bool jump[4];						//if the chess can jump(four direction)
		short int board_t[_Table_Size][_Table_Size];
};

