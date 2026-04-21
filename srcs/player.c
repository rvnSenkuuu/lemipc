#include "lemipc.h"

int	put_player_on_board(t_ipc *ipc, t_player *player)
{
	sem_lock(ipc);
	if (ipc->board->player_count == BOARD_SIZE) { 
		sem_unlock(ipc);
		return 1;
	}
	
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		int	x = rand() % BOARD_HEIGHT;
		int	y = rand() % BOARD_WIDTH;

		if (ipc->board->map[x + y * BOARD_WIDTH] == EMPTY_SLOT) {
			player->pos_x = x;
			player->pos_y = y;
			ipc->board->map[x + y * BOARD_WIDTH] = player->team_id;
			break;
		}
	}

	ipc->board->player_count++;
	sem_unlock(ipc);
	return 0;
}