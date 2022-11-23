
#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>
#include <sstream>
#include <tuple>

struct Board {
	std::vector<std::vector<char>> map;
	int size{0};
};
Board board;
std::vector<Board> history; 
int take_all_stones(char stone_type, int row, int column);
std::vector<char> border_chars; // 

std::vector<int> scores;
std::vector<std::vector<int>> checked;
int compare_boards(Board board1, Board board2) { //return 1 if boards arent equal, returns 0 if boards are equal
	for (int i = 0; i < board1.size; i++) {
		for (int k = 0; k < board1.size; k++) {
			if (board1.map[i][k] != board2.map[i][k])
				return 1;
		}
	}
	return 0;
}
void draw_map() {
	for (int i = 0; i < board.size; i++) {
		for (int k = 0; k < board.size; k++) {
			std::cout << board.map[i][k];
		}
		std::cout << '\n';
	}
}
int calculate_area(int row, int column) {
	int area = 1;
	board.map[row][column] = 'A';

	if (row - 1 >= 0) { //check up
		if (board.map[row - 1][column] == '.')
			return area + calculate_area(row - 1, column);
		if (board.map[row - 1][column] == 'X' || board.map[row - 1][column] == 'O') {
			border_chars.push_back(board.map[row - 1][column]);
		}
	}
	if (row + 1 < board.size) { //check down
		if (board.map[row + 1][column] == '.')
			return area + calculate_area(row + 1, column);
		if (board.map[row + 1][column] == 'X' || board.map[row + 1][column] == 'O') {
			border_chars.push_back(board.map[row + 1][column]);
		}
	}
	if (column - 1 >= 0) { //check left
		if (board.map[row][column-1] == '.')
			return area + calculate_area(row, column-1);
		if (board.map[row][column-1] == 'X' || board.map[row][column-1] == 'O') {
			border_chars.push_back(board.map[row][column-1]);
		}
	}
	if (column + 1 < board.size) { //check right
		if (board.map[row][column + 1] == '.')
			return area + calculate_area(row, column + 1);
		if (board.map[row][column + 1] == 'X' || board.map[row][column + 1] == 'O') {
			border_chars.push_back(board.map[row][column + 1]);
		}
	}
	return area;
}
void areas() {
	for (int i = 0; i < board.size; i++) {
		for (int j = 0; j < board.size; j++) {
			if (board.map[i][j] == '.') {
				int area = calculate_area(i, j);
				//check if one player surounds the area
				char checked_char=' ';
				bool equal = true;
				if (border_chars.size() > 0)
					checked_char = border_chars[0];
				for (size_t index = 1; index < border_chars.size(); index++) {
					if (checked_char != border_chars[index]) {
						equal = false;
						break;
					}
				}
				if (equal) {
					int player_index = checked_char == 'X' ? 0 : 1;
					std::cout << area << "\n";
					scores[player_index] += area;
				}
				border_chars.clear(); // empty the list
			}
		}
	}
}

int free_places(char stone_type, int row, int column) { //check all frees near the played move
	int stones{ 0 };
	char c = stone_type == 'X'? 'x': 'o';
	board.map[row][column] = c; //convert to lowercase to avoid infinite recursion or counting some symbols more than once
	if ((column + 1) < board.size) {
		if (board.map[row][column + 1] == '.')
			stones++;
		if (board.map[row][column + 1] == stone_type) {
			stones += free_places(stone_type, row, column + 1);
		}
	}
	if (column - 1 >= 0) {
		if (board.map[row][column - 1] == '.') {
			stones++;
		}
		if (board.map[row][column - 1] == stone_type) {
			stones += free_places(stone_type, row, column - 1);
		}
			
	}

	if (row + 1 < board.size) {
		if (board.map[row + 1][column] == '.')
			stones++;
		if (board.map[row + 1][column] == stone_type) {
			stones += free_places(stone_type, row + 1, column);
		}
			
	}
	if (row - 1 >= 0) {
		if (board.map[row-1][column] == '.')
			stones++;
		if (board.map[row-1][column] == stone_type)
			stones += free_places(stone_type, row-1, column);
	}
	return stones;
}
void convert_uppercase(char lowercase_stone_type, char stone_type, int row, int column) {
	board.map[row][column] = stone_type;
	if (row - 1 >= 0 && board.map[row - 1][column] == lowercase_stone_type)
		convert_uppercase(lowercase_stone_type, stone_type, row-1, column);
	if (row + 1 < board.size && board.map[row + 1][column] == lowercase_stone_type)
		convert_uppercase(lowercase_stone_type, stone_type, row+1, column);
	if (column - 1 >= 0 && board.map[row][column-1] == lowercase_stone_type)
		convert_uppercase(lowercase_stone_type, stone_type, row, column-1);
	if (column+1 < board.size && board.map[row][column+1] == lowercase_stone_type)
		convert_uppercase(lowercase_stone_type, stone_type, row, column+1);

}
int check_and_take_opponents_stones(uint8_t turn, int row, int column) {
	char opposite_c = (turn == 0) ? 'O' : 'X';
	char lowercase_opposite = opposite_c == 'X' ? 'x' : 'o';
	//look for opposite character next to you to check if we didn't close them 
	int fplaces{ 0 };
	int taken_stones = 0;
	if (column - 1 >= 0 && board.map[row][column - 1] == opposite_c) { //check left
		fplaces = free_places(opposite_c, row, column - 1); //count number of free spaces around the stone
		convert_uppercase(lowercase_opposite, opposite_c, row, column - 1);
		if (fplaces == 0)
			taken_stones += take_all_stones(opposite_c, row, column - 1);
	}
	if (column + 1 < board.size && board.map[row][column + 1] == opposite_c) { //check right
		fplaces = free_places(opposite_c, row, column + 1); //count number of free spaces around the stone
		convert_uppercase(lowercase_opposite, opposite_c, row, column + 1);
		if (fplaces == 0)
			taken_stones += take_all_stones(opposite_c, row, column + 1);
	}
	if (row - 1 >= 0 && board.map[row - 1][column] == opposite_c) { //check up
		fplaces = free_places(opposite_c, row - 1, column); //count number of free spaces around the stone, tu mi skoci error
		convert_uppercase(lowercase_opposite, opposite_c, row - 1, column);
		if (fplaces == 0)
			taken_stones += take_all_stones(opposite_c, row - 1, column);
	}
	if (row + 1 < board.size && board.map[row + 1][column] == opposite_c) { //check down
		fplaces = free_places(opposite_c, row + 1, column); //count number of free spaces around the stone
		convert_uppercase(lowercase_opposite, opposite_c, row + 1, column);
		if (fplaces == 0)
			taken_stones += take_all_stones(opposite_c, row + 1, column);
	}
	return taken_stones;
}


int validate_move_take_stones(uint8_t turn, char stone_type, int row, int column) {
	//check ko
	for (size_t i = 0; i < history.size(); i++) {
		if (compare_boards(board, history[i]) == 0) { // board are equal it would be ko move
			std::cout << "Equal boards";
			return 1;
		}
	}
	board.map[row][column] = stone_type;
	if (row == 2 && column == 3)
		std::cout << "Hey\n";
	int taken_stones = check_and_take_opponents_stones(turn, row, column);
	if (taken_stones != 0) {
		std::cout << "Taken stones " << taken_stones << "\n";
		scores[turn] += taken_stones;
		return 0;
	}

	// we didn't take any stones so we have to check freedoms
	int stones = free_places(stone_type, row, column);
	char c = stone_type == 'X' ? 'x' : 'o';
	convert_uppercase(c, stone_type, row, column);
	std::cout << "Slobody " << stones << "\n";
	if (stones == 0) {
		board.map[row][column] = '.'; //undo the step
		return 1;
	}
		
	return 0;
}
int take_all_stones(char stone_type, int row, int column) {
	if (board.map[row][column] == '.')
		return 0;
	board.map[row][column] = '.';
	int stones{ 1 };
	if (column + 1 < board.size && (board.map[row][column + 1] == stone_type)) {
		return stones + take_all_stones(stone_type,row,column+1);
	}
	if (column - 1 >= 0 && board.map[row][column -1] == stone_type) {
		return stones + take_all_stones(stone_type, row, column-1);
	}

	if (row + 1 < board.size && board.map[row + 1][column] == stone_type) {
		return stones + take_all_stones(stone_type, row+1, column);
	}
	if (row - 1 >= 0 && board.map[row - 1][column] == stone_type) {
		return stones + take_all_stones(stone_type, row - 1, column);
	}
	return stones;
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
				pass_flag = 0;
				if (row == 2 && column == 0)
					std::cout << '\n';
				if (row < 0 || row >= board.size || column < 0 || column >= board.size) // check if we are out of map
					return 1;
				if (board.map[row][column] != '.') //invalid step
					continue;
				else {
					c = (turn == 0) ? 'X' : 'O';
					if (validate_move_take_stones(turn, c, row, column) == 0) { //step doesnt cause ko or suicide , we also took stones, also does the step
						std::cout << row << " " << column << "\n";
						turn = (turn + 1) % 2;
					}
					draw_map();
					std::cout << "--------\n";
				}
			}
			else {
				s.clear();
				s >> p;
				if (p == "pass") {
					if (pass_flag == 1) { //2x pass 
						if(argument == "--board")
							draw_map();
						else if (argument == "--score") {
							areas();
							std::cout << scores[0] << " " << scores[1];
						}
						return 0;
					}
					else
						pass_flag = 1;
					turn = (turn + 1) % 2;
				}
				else {
					continue;
					//return 1; // we read some bullshit
				}
					
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
	scores[1] = 0; //player2
	return start_game(std::string(argv[2]));
}
