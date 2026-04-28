#include "lemipc.h"

int	put_player_on_board(t_ipc *ipc, t_player *player)
{
	srand(0);
	sem_lock(ipc);
	if (ipc->board->player_count == BOARD_SIZE) { 
		sem_unlock(ipc);
		return 1;
	}
	
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		int	x = rand() % BOARD_WIDTH;
		int	y = rand() % BOARD_HEIGHT;

		if (ipc->board->map[x + y * BOARD_HEIGHT] == EMPTY_SLOT) {
			player->pos_x = x;
			player->pos_y = y;
			ipc->board->map[x + y * BOARD_HEIGHT] = player->team_id;
			break;
		}
	}

	ipc->board->player_count++;
	sem_unlock(ipc);
	return 0;
}

void	remove_player_from_board(t_board *board, t_player *player)
{
	player->alive = false;
	board->map[player->pos_x + player->pos_y * BOARD_HEIGHT] = EMPTY_SLOT;
	board->player_count--;
}

int	check_player_around(t_board *board, t_player *player)
{
	int	teams[128] = {0};
	for (int x = player->pos_x - 1; x <= player->pos_x + 1; x++) {
		for (int y = player->pos_y - 1; y <= player->pos_y + 1; y++) {
			if (x == player->pos_x && y == player->pos_y)
				continue;
			if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT)
				continue;
			int	team_id = board->map[x + y * BOARD_HEIGHT];
			if (team_id > EMPTY_SLOT && team_id != player->team_id)
				teams[team_id]++;
		}
	}

	for (size_t i = 0; i < ARRAY_LEN(teams); i++)
		if (teams[i] >= 2)
			return 1;

	return 0;
}