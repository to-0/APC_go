
#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>
#include <sstream>

struct Board {
	std::vector<std::vector<char>> map;
	int size{0};
};
Board board;
std::vector<Board> history; 

std::vector<int> scores;

int compare_boards(Board board1, Board board2) { //return 1 if boards arent equal, returns 0 if boards are equal
	for (int i = 0; i < board1.size; i++) {
		for (int k = 0; k < board1.size; k++) {
			if (board1.map[i][k] != board2.map[i][k])
				return 1;
		}
	}
	return 0;
}

int free_places(char stone_type, int row, int column, int previous_row, int previous_column) { //check all frees near the played move
	if (previous_row == row && previous_column == column)
		return 0;
	int stones{ 0 };
	if (column + 1 < board.size) {
		if (board.map[row][column + 1] == '.')
			stones++;
		if (board.map[row][column + 1] == stone_type)
			stones += free_places(stone_type, row, column + 1, row, column);
	}
	if (column - 1 >= 0) {
		if (board.map[row][column - 1] == '.')
			stones++;
		if (board.map[row][column - 1] == stone_type)
			stones += free_places(stone_type, row, column - 1, row, column );
	}

	if (row + 1 < board.size) {
		if (board.map[row+1][column] == '.')
			stones++;
		if (board.map[row+1][column] == stone_type)
			stones += free_places(stone_type, row+1, column, row, column);
	}
	if (row - 1 >= 0) {
		if (board.map[row-1][column] == '.')
			stones++;
		if (board.map[row-1][column] == stone_type)
			stones += free_places(stone_type, row-1, column, row, column);
	}
	return stones;
}

int check_ko_suicide(uint8_t turn, int row, int column) {
	//check ko
	for (size_t i = 0; i < history.size(); i++) {
		if (compare_boards(board, history[i]) == 0) { //boards are equal, it would be ko
			return 1;
		}
		std::cout << row << column << turn;
	}
	int stones = free_places(turn, row, column, -1, -1);
	if (stones == 0)
		return 1;
	return 0;
}
int take_all_stones(char stone_type, int row, int column, int previous_row, int previous_column) {
	if (previous_row == row && previous_column == column)
		return 0;
	board.map[row][column] = '.';
	int stones{ 1 };
	if (column + 1 < board.size && (board.map[row][column + 1] == stone_type)) {
		return stones + take_all_stones(stone_type,row,column+1,row,column);
	}
	if (column - 1 >= 0 && board.map[row][column -1] == stone_type) {
		return stones + take_all_stones(stone_type, row, column-1, row, column);
	}

	if (row + 1 < board.size && board.map[row + 1][column] == stone_type) {
		return stones + take_all_stones(stone_type, row+1, column,row,column);
	}
	if (row - 1 >= 0 && board.map[row - 1][column] == stone_type) {
		return stones + take_all_stones(stone_type, row - 1, column, row, column);
	}
	return stones;
}

void play_move(uint8_t turn, int row, int column) { //after validation so we know there is an empty space in that row column
	char c = (turn == 0) ? 'X' : 'O';
	char opposite_c = (turn == 0) ? 'O' : 'X';
	board.map[row][column] = c; // play the move
	//look for opposite character next to you
	int fplaces{ 0 };
	if (column - 1 >= 0 && board.map[row][column - 1] == opposite_c) { //check left
		fplaces = free_places(opposite_c, row, column - 1, -1, -1); //count number of free spaces around the stone
		if (fplaces == 0)
			scores[static_cast<int>(turn)] += take_all_stones(opposite_c, row, column - 1, -1, -1);
	}
	if (column +1 < board.size && board.map[row][column + 1] == opposite_c) { //check right
		fplaces = free_places(opposite_c, row, column + 1, -1, -1); //count number of free spaces around the stone
		if (fplaces == 0)
			scores[static_cast<int>(turn)] += take_all_stones(opposite_c, row, column + 1, -1, -1);
	}
	if (row - 1 >= 0 && board.map[row-1][column] == opposite_c) { //check up
		fplaces = free_places(opposite_c, row-1, column, -1, -1); //count number of free spaces around the stone
		if (fplaces == 0)
			scores[static_cast<int>(turn)] += take_all_stones(opposite_c, row-1, column, -1, -1);
	}
	if (row + 1 < board.size && board.map[row + 1][column] == opposite_c) { //check down
		fplaces = free_places(opposite_c, row +1, column, -1, -1); //count number of free spaces around the stone
		if (fplaces == 0)
			scores[static_cast<int>(turn)] += take_all_stones(opposite_c, row + 1, column, -1, -1);
	}
}

void draw_map() {
	for (int i = 0; i < board.size; i++) {
		for (int k = 0; k < board.size; k++) {
			std::cout << board.map[i][k];
		}
		std::cout << '\n';
	}
}
int start_game(std::string argument) {
	std::string buffer;
	uint8_t turn{ 0 }; //which player's turn is it
	board.map.resize(board.size);
	for (int i = 0; i < board.size; i++) {
		board.map[i].resize(board.size);
		for (int j = 0; j < board.size; j++) {
			board.map[i][j] = '.';
		}
	}
	int row = 0;
	int column = 0;
	std::string p("");
	uint8_t pass_flag{0};
	char c{ 'X' };
	while (std::getline(std::cin, buffer)) {// read input
		std::stringstream s(buffer);
		while (!s.eof()) {
			if (s >> row && s >> column) {
				if (row < 0 || row >= board.size || column < 0 || column >= board.size) // check if we are out of map
					return 1;
				if (board.map[row][column] != '.') //invalid step
					continue;
				else {
					c = (turn == 0) ? 'X' : 'O';
					if (check_ko_suicide(turn, row, column) == 0) { //step doesnt cause ko or suicide
						play_move(turn, row, column);
						turn = (turn + 1) % 2;
					}
				}
			}
			else {
				s.clear();
				s >> p;
				if (p == "pass") {
					if (pass_flag == 1) { //2x pass 
						if(argument == "--board")
							draw_map();
						else {
							std::cout << scores[0] << " " << scores[1];
						}
						return 1;
					}
					else
						pass_flag = 1;
					turn = (turn + 1) % 2;
				}
				else
					return 1; // we read some bullshit
			}
		}
	} 
	
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
		return 1;
	long long a{ 0 };
	try {
		a = std::stoll(std::string(argv[1]));
	}
	catch (std::exception &e) {
		(void)e;
		return 1;
	}

	if (a < 0 || a > INT_MAX) {
		return 1;
	}
	board.size = static_cast<int>(a);
	if (board.size == 0) {
		return 1;
	}
	if (std::string(argv[2]) != "--board" && std::string(argv[2])!= "--score") {
		return 1;
	}
	scores.resize(2);
	scores[0] = 0; //player1
	scores[1] = 1; //player2
	return start_game(std::string(argv[2]));
}
