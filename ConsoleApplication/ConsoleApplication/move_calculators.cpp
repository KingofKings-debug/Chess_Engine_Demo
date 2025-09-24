
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <stack>



using namespace std;


sf::Vector2i getcord(int n);
int getnumber(int x, int y);
int get_number_from_localpos(int x, int y);
void cheak_king(int t, vector<int>& p_moves, struct board_total_info &game);
void get_bishop_moves(int t, vector<int>& ans, int peice, bool is_for_king, struct board_total_info &game);
void get_pawn_moves_black(int t, vector<int>& ans, int peice, bool atack_only, struct board_total_info &game);
void get_rook_moves(int t, vector<int>& ans, int peice, bool is_for_king, struct board_total_info &game);
void get_pawn_moves_white(int t, vector<int>& ans, int peice, bool atack_only, struct board_total_info &game);
void get_Knight_moves(int t, vector<int>& ans, int peice, struct board_total_info &game);
void get_queen_moves(int t, vector<int>& ans, int peice, struct board_total_info &game);
void get_king_moves(int t, vector<int>& ans, int peice, struct board_total_info &game);
bool king_in_cheak_bool_only(int t, int peice, struct board_total_info &game);


struct board_total_info {
	vector<int> board;
	vector<vector<bool>> castle_rights;
	int white_facing;
	vector<int> live_king_position;
	int active_player;
};


void getmoves(int t, vector<int>& p_moves, struct board_total_info &game) {
	int peice = game.board[t];
	vector<int> ans;
	if (peice <= 8 && peice >= 1) {
		get_pawn_moves_white(t, ans, peice, false, game);


		for (int i = 0;i < ans.size();i++) {
			if (game.board[ans[i]] * peice <= 0) {
				p_moves.push_back(ans[i]);
			}
		}
	}

	if (peice <= -1 && peice >= -8) {
		get_pawn_moves_black(t, ans, peice, false, game);

		for (int i = 0;i < ans.size();i++) {
			if (game.board[ans[i]] * peice <= 0) {
				p_moves.push_back(ans[i]);
			}
		}
	}
	if (abs(peice) == abs(12) || abs(peice) == abs(11)) {
		get_Knight_moves(t, ans, peice, game);
		for (int i = 0;i < ans.size();i++) {
			if (game.board[ans[i]] * peice <= 0) {
				p_moves.push_back(ans[i]);
			}
		}
	}
	if (abs(peice) == abs(13) || abs(peice) == abs(14)) {
		get_bishop_moves(t, p_moves, peice, false, game);
	}
	if (abs(peice) == abs(10) || abs(peice) == abs(9)) {
		get_rook_moves(t, p_moves, peice, false, game);
	}
	if (abs(peice) == abs(15)) {
		get_queen_moves(t, p_moves, peice, game);

	}
	if (abs(peice) == abs(16)) {
		get_king_moves(t, p_moves, peice, game);
	}
	cheak_king(t, p_moves,game);


}

void get_king_moves(int t, vector<int>& ans, int peice, struct board_total_info &game) {
	int possible_loc[] = { 1,-1,8,-8,7,-7,9,-9 };
	int king_moves[8][2] = {
	{1, 0},  {-1, 0},   // left/right
	{0, 1},  {0, -1},   // up/down
	{1, 1},  {-1, -1},  // diagonals
	{1, -1}, {-1, 1}
	};

	sf::Vector2i loc = getcord(t); // x = file (0..7), y = rank (0..7)

	for (auto& move : king_moves) {
		int nx = loc.x + move[0];
		int ny = loc.y + move[1];

		// check board bounds
		if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
			int new_pos = ny * 8 + nx;  // convert back to 1D index

			// only add if empty or opponent piece
			if (game.board[new_pos] * game.active_player <= 0) {
				ans.push_back(new_pos);
			}
		}
	}

}
void get_queen_moves(int t, vector<int>& ans, int peice, struct board_total_info &game) {
	get_rook_moves(t, ans, peice, false, game);
	get_bishop_moves(t, ans, peice, false, game);
}
void get_Knight_moves(int t, vector<int>& ans, int peice, struct board_total_info &game) {
	sf::Vector2i a = getcord(t);
	vector<pair<int, int>> s;
	s.push_back({ a.x + 2,a.y + 1 });
	s.push_back({ a.x - 2,a.y + 1 });
	s.push_back({ a.x + 1,a.y + 2 });
	s.push_back({ a.x - 1,a.y + 2 });
	s.push_back({ a.x + 1,a.y - 2 });
	s.push_back({ a.x - 1,a.y - 2 });
	s.push_back({ a.x + 2,a.y - 1 });
	s.push_back({ a.x - 2,a.y - 1 });
	for (int i = 0;i < s.size();i++) {
		if (s[i].first < 0 || s[i].first > 7 || s[i].second < 0 || s[i].second > 7) {
			continue;
		}
		else {

			ans.push_back(getnumber(s[i].first, s[i].second));

		}
	}
}
void get_bishop_moves(int t, vector<int>& ans, int peice, bool is_for_king, struct board_total_info &game) {
	int u = t;
	while (u % 8 != 7 && u / 8 != 0) {
		u -= 7;
		if (game.board[u] * peice < 0) {
			ans.push_back(u);
			break;
		}
		else if (game.board[u] * peice == 0) {
			ans.push_back(u);
		}
		else {
			break;
		}
	}
	u = t;
	while (u % 8 != 0 && u / 8 != 0) {
		u -= 9;
		if (game.board[u] * peice < 0) {
			ans.push_back(u);
			break;
		}
		else if (game.board[u] * peice == 0) {
			ans.push_back(u);
		}
		else {
			break;
		}
	}
	u = t;

	while (u % 8 != 0 && u / 8 != 7) {
		u += 7;
		if (game.board[u] * peice < 0) {
			ans.push_back(u);
			break;
		}
		else if (game.board[u] * peice == 0) {
			ans.push_back(u);
		}
		else {
			break;
		}
	}
	u = t;
	while (u % 8 != 7 && u / 8 != 7) {
		u += 9;
		if (game.board[u] * peice < 0) {
			ans.push_back(u);
			break;
		}
		else if (game.board[u] * peice == 0) {
			ans.push_back(u);
		}
		else {
			break;
		}
	}
	return;
}
void get_pawn_moves_white(int t, vector<int>& ans, int peice, bool atack_only, struct board_total_info &game) {
	sf::Vector2i a = getcord(t);
	vector<pair<int, int>> f;
	vector<pair<int, int>> s;
	if (game.white_facing == 1) {
		f.push_back({ a.x,a.y - 1 });
		if (a.y == 6) {
			if (game.board[getnumber(a.x, a.y - 1)] == 0)
			{
				f.push_back({ a.x,a.y - 2 });
			}
		}
		s.push_back({ a.x + 1,a.y - 1 });
		s.push_back({ a.x - 1,a.y - 1 });
	}
	else {
		f.push_back({ a.x,a.y + 1 });
		if (a.y == 1) {
			if (game.board[getnumber(a.x, a.y + 1)] == 0)
			{
				f.push_back({ a.x,a.y + 2 });
			}
		}
		s.push_back({ a.x + 1,a.y + 1 });
		s.push_back({ a.x - 1,a.y + 1 });
	}
	if (atack_only) {
		for (int i = 0;i < s.size();i++) {
			if (s[i].first >= 0 && s[i].first <= 7 && s[i].second <= 7 && s[i].second >= 0) {
				ans.push_back(getnumber(s[i].first, s[i].second));
			}

		}
	}
	else
	{
		for (int i = 0;i < f.size();i++) {
			if (f[i].first < 0 || f[i].first > 7 || f[i].second < 0 || f[i].second > 7) {
				continue;
			}
			else {
				if (game.board[getnumber(f[i].first, f[i].second)] == 0) {
					ans.push_back(getnumber(f[i].first, f[i].second));
				}
			}
		}
		for (int i = 0;i < s.size();i++) {
			if (s[i].first < 0 || s[i].first > 7 || s[i].second < 0 || s[i].second > 7) {
				continue;
			}
			else {
				if (game.board[getnumber(s[i].first, s[i].second)] * peice < 0) {
					ans.push_back(getnumber(s[i].first, s[i].second));
				}
			}
		}
	}

}
void get_pawn_moves_black(int t, vector<int>& ans, int peice, bool atack_only, struct board_total_info &game) {
	sf::Vector2i a = getcord(t);
	vector<pair<int, int>> f;
	vector<pair<int, int>> s;
	if (game.white_facing == 0) {
		f.push_back({ a.x,a.y - 1 });
		if (a.y == 6) {
			if (game.board[getnumber(a.x, a.y - 1)] == 0)
			{
				f.push_back({ a.x,a.y - 2 });
			}
		}
		s.push_back({ a.x + 1,a.y - 1 });
		s.push_back({ a.x - 1,a.y - 1 });
	}
	else {
		f.push_back({ a.x,a.y + 1 });
		if (a.y == 1) {
			if (game.board[getnumber(a.x, a.y + 1)] == 0)
			{
				f.push_back({ a.x,a.y + -2 });
			}
		}
		s.push_back({ a.x + 1,a.y + 1 });
		s.push_back({ a.x - 1,a.y + 1 });
	}
	if (atack_only) {
		for (int i = 0;i < s.size();i++) {
			ans.push_back(getnumber(s[i].first, s[i].second));
		}
	}
	else
	{
		for (int i = 0;i < f.size();i++) {
			if (f[i].first < 0 || f[i].first > 7 || f[i].second < 0 || f[i].second > 7) {
				continue;
			}
			else {
				if (game.board[getnumber(f[i].first, f[i].second)] == 0) {
					ans.push_back(getnumber(f[i].first, f[i].second));
				}
			}
		}
		for (int i = 0;i < s.size();i++) {
			if (s[i].first < 0 || s[i].first > 7 || s[i].second < 0 || s[i].second > 7) {
				continue;
			}
			else {
				if (game.board[getnumber(s[i].first, s[i].second)] * peice < 0) {
					ans.push_back(getnumber(s[i].first, s[i].second));
				}
			}
		}
	}
}
void get_rook_moves(int t, vector<int>& ans, int peice, bool is_for_king, struct board_total_info &game) {

	int u = t;


	while (u % 8 != 7) {
		u += 1;
		if (game.board[u] * peice < 0) {
			ans.push_back(u);
			break;
		}
		else if (game.board[u] * peice == 0) {
			ans.push_back(u);
		}
		else {
			break;
		}
	}
	u = t;
	while (u % 8 != 0) {
		u -= 1;
		if (game.board[u] * peice < 0) {
			ans.push_back(u);
			break;
		}
		else if (game.board[u] * peice == 0) {
			ans.push_back(u);
		}
		else {
			break;
		}
	}
	u = t;

	while (u / 8 != 0) {
		u -= 8;
		if (game.board[u] * peice < 0) {
			ans.push_back(u);
			break;
		}
		else if (game.board[u] * peice == 0) {
			ans.push_back(u);
		}
		else {
			break;
		}
	}
	u = t;
	while (u / 8 != 7) {
		u += 8;
		if (game.board[u] * peice < 0) {
			ans.push_back(u);
			break;
		}
		else if (game.board[u] * peice == 0) {
			ans.push_back(u);
		}
		else {
			break;
		}
	}
	return;


}