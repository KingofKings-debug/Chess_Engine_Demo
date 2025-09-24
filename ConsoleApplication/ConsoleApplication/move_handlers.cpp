#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <stack>
using namespace std;



struct board_total_info {
	vector<int> board;
	vector<vector<bool>> castle_rights;
	int white_facing;
	vector<int> live_king_position;
	int active_player;
};


//refrences to funtions / predefinations
sf::Vector2i getcord(int n);
int getnumber(int x, int y);
int get_number_from_localpos(int x, int y);
vector<vector<int>> ai_move_handler(struct board_total_info& game);

void get_bishop_moves(int t, vector<int>& ans, int peice, bool is_for_king, struct board_total_info& game);
void get_pawn_moves_black(int t, vector<int>& ans, int peice, bool atack_only, struct board_total_info& game);
void get_rook_moves(int t, vector<int>& ans, int peice, bool is_for_king, struct board_total_info& game);
void get_pawn_moves_white(int t, vector<int>& ans, int peice, bool atack_only, struct board_total_info& game);
void get_Knight_moves(int t, vector<int>& ans, int peice, struct board_total_info& game);
void get_queen_moves(int t, vector<int>& ans, int peice, struct board_total_info& game);
void get_king_moves(int t, vector<int>& ans, int peice, struct board_total_info& game);
bool king_in_cheak_bool_only(int t, int peice, struct board_total_info &game);


void handle_king_move(int from_pos, int newloc,struct board_total_info &game) {
	game.board[newloc] = game.board[from_pos];
	game.board[from_pos] = 0;
}
void handle_pawn_move(int from_pos, int newloc, struct board_total_info& game) {
	game.board[newloc] = game.board[from_pos];
	game.board[from_pos] = 0;

}

//if the selected peice is king then already its move are goona be trimmed
//this function checks if the move is legal or not and if it is then it adds that to p_moves
void is_move_legal(int t, vector<int>& p_moves, int peice,struct board_total_info &game) {
	vector<int> local(p_moves);
	p_moves.clear();
	for (auto i : local) {
		int delected_peice = game.board[i];
		int moves_peice = game.board[t];

		game.board[i] = game.board[t];
		game.board[t] = 0;

		if (abs(moves_peice) == 16) {

			game.live_king_position[game.active_player + 1] = i;
		}

		if (!king_in_cheak_bool_only(game.live_king_position[game.active_player + 1], peice,game)) {
			p_moves.push_back(i);
		}
		game.board[t] = moves_peice;
		game.board[i] = delected_peice;
		if (abs(moves_peice) == 16) {

			game.live_king_position[game.active_player + 1] = t;
		}
	}
}

////this function checks if the king is in cheak and if it is then it trims the p_moves to only those which are legal
void cheak_king(int t, vector<int>& p_moves,struct board_total_info &game) {
	if (game.active_player == -1) {
		is_move_legal(t, p_moves, -16,game);
	}
	else {
		is_move_legal(t, p_moves, 16, game);
	}

}

void ai_move_doer(int from_loc, int to_loc, struct board_total_info& game) {
	
	
	if (abs(game.board[from_loc]) <= 8){
		handle_pawn_move(from_loc, to_loc, game);
	}
	else if (abs(game.board[from_loc]) != 16){
		game.board[to_loc] = game.board[from_loc];
		game.board[from_loc] = 0;
	}
	else if (abs(game.board[from_loc]) == 16){
		handle_king_move(from_loc, to_loc, game);
		game.live_king_position[game.active_player + 1] = to_loc;
	}
	game.active_player *= (-1);
}


bool handle_move(int lastloc, int newloc, vector<int>& p_moves,struct board_total_info &game) {

	if (count(p_moves.begin(), p_moves.end(), newloc) > 0) {
		//runs when the selected loc is availabe on p_moves indicating a succefull 

		if (abs(game.board[lastloc]) <= 8) {
			handle_pawn_move(lastloc, newloc, game);
		}
		else if (abs(game.board[lastloc]) != 16) {
			game.board[newloc] = game.board[lastloc];
			game.board[lastloc] = 0;
		}
		else if (abs(game.board[lastloc]) == 16) {
			handle_king_move(lastloc, newloc, game);
			game.live_king_position[game.active_player + 1] = newloc;
		}
		game.active_player *= (-1);
		return true;

	}

	return false;

}

//cheaks if the king is in cheak or not(return true if in cheak)
bool king_in_cheak_bool_only(int t, int peice, struct board_total_info &game) {
	vector<int> knights;
	vector<int> bishop;
	vector<int> rook;
	vector<int> pawn;

	get_Knight_moves(t, knights, game.board[t], game);
	get_bishop_moves(t, bishop, game.board[t], true, game);
	get_rook_moves(t, rook, game.board[t], true, game);


	if (game.active_player == 1) {
		get_pawn_moves_white(t, pawn, game.board[t], true, game);
		for (auto i : pawn) {
			if (game.board[i] >= (-1) * game.active_player * abs(8) && game.board[i] <= (-1) * game.active_player * abs(1)) {

				return true;
			}
		}
	}
	else {
		get_pawn_moves_black(t, pawn, game.board[t], true, game);
		for (auto i : pawn) {
			if (game.board[i] <= (-1) * game.active_player * abs(8) && game.board[i] >= (-1) * game.active_player * abs(1)) {
				return true;
			}
		}
	}

	for (auto i : knights) {
		if (game.board[i] == (-1) * game.active_player * abs(11) || game.board[i] == (-1) * game.active_player * abs(12)) {

			return true;
		}
	}
	for (int i : bishop) {
		if ((game.board[i]) == (-1) * game.active_player * abs(13) || (game.board[i]) == (-1) * game.active_player * abs(14) || (game.board[i]) == (-1) * game.active_player * abs(15)) {

			return true;
		}
		if (game.board[i] == -1 * game.active_player * 16 && abs(getcord(i).x - getcord(t).x) <= 1 && abs(getcord(i).y - getcord(t).y) <= 1) {
			return true;
		}

	}
	for (int i : rook) {
		if ((game.board[i]) == (-1) * game.active_player * abs(9) || (game.board[i]) == (-1) * game.active_player * abs(10) || (game.board[i]) == (-1) * game.active_player * abs(15)) {

			return true;
		}
		if (game.board[i] == -1 * game.active_player * 16 && abs(getcord(i).x - getcord(t).x) <= 1 && abs(getcord(i).y - getcord(t).y) <= 1) {
			return true;
		}
	}
	return false;
}