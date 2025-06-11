#include <iostream>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <unordered_map>
#include <map>
#include <minmax.h>
using namespace std;

void getmoves(int t, vector<int>& p_moves);
int blackplayer(int board[], int player, int depth);
int whiteplayer(int board[], int player, int depth);


int global_depth = 4;

int get_peice_value(int t) {
	if (abs(t) >= 1 && abs(t) <= 8) {
		return 100;
	}
	else if (abs(t) == 9 || abs(t) == 10) {
		return 500;
	}
	else if (abs(t) == 11 || abs(t) == 12 || abs(t) == 13 || abs(t) == 14) {
		return 300;
	}
	else if (abs(t) == 15) {
		return 900;
	}
	else if (abs(t) == 16) {
		return 10000;
	}
}

int get_total_count(int b[]) {
	int whitesum = 0;
	int blacksum = 0;
	for (int i = 0;i < 64;i++) {
		if (b[i] > 0) {
			whitesum += get_peice_value(b[i]);
		}
		else if (b[i] < 0) {
			blacksum += get_peice_value(b[i]);
		}
	}
	return whitesum-blacksum;
}
map<int, vector<int>> mymap;
vector<vector<int>> give_moves(int b[]) {
	

	int g= whiteplayer(b, 1,global_depth);
	vector<int>  moves_for_this_peice;
	getmoves(mymap[g][0], moves_for_this_peice);
	return { {mymap[g][0],mymap[g][1]},moves_for_this_peice};
	
	
}



void print_board(int b[]) {
	for (int i = 0;i < 64;i++) {
		if (i % 8 == 0) {
			cout << endl;
		}
		cout << b[i] << " ";
	}
	cout << endl;
}


int whiteplayer(int board[], int player, int depth) {
	if (depth == 0){
		print_board(board);
		int x;
		cin >> x;
		return get_total_count(board);
	}

	int max_got_from_all_moves = INT_MIN;
	for (int i = 0;i < 64;i++) {
		if (board[i] > 0) {
			vector<int> moves_for_this_peice;
			getmoves(i, moves_for_this_peice);
			int peice_moved = board[i];
			if (moves_for_this_peice.size() == 0) {
				continue;
			}
			for (int j : moves_for_this_peice) {
				int peice_to_moved = board[j];
				board[j] = peice_moved;
				board[i] = 0;
				
				if (blackplayer(board, -1 * player, depth - 1) > max_got_from_all_moves) {
					max_got_from_all_moves = max(max_got_from_all_moves, blackplayer(board, -1 * player, depth - 1));
					if(depth==global_depth)
					{
						
						mymap[max_got_from_all_moves] = { i,j };
						
					}
				}
				
				board[i] = peice_moved;
				board[j] = peice_to_moved;
			}
		}
	}
	return max_got_from_all_moves;
}
int blackplayer(int board[], int player, int depth) {
	if (depth == 0) {
		print_board(board);
		
		return -1*get_total_count(board);
	}
	int max_got_from_all_moves = INT_MIN;
	for (int i = 0;i < 64;i++) {
		if (board[i] < 0) {
			vector<int> moves_for_this_peice;
			getmoves(i, moves_for_this_peice);
			if (moves_for_this_peice.size() == 0){
				continue;
			}
			int peice_moved = board[i];
			for (int j : moves_for_this_peice) {
				int peice_to_moved = board[j];
				board[j] = peice_moved;
				board[i] = 0;
				
			//	cout << get_total_count(board) <<"\n";
				max_got_from_all_moves = max(max_got_from_all_moves,-1* whiteplayer(board, player, depth - 1));
				board[i] = peice_moved;
				board[j] = peice_to_moved;
			}
		}
	}
	return -1*max_got_from_all_moves;
}

	





