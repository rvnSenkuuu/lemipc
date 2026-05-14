#include "lemipc.h"

int	put_player_on_board(t_ipc *ipc, t_player *player)
{
	sem_lock(ipc);
	if (ipc->board->player_count == BOARD_SIZE) { 
		sem_unlock(ipc);
		return 1;
	}
	
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		int	x = rand() % BOARD_WIDTH;
		int	y = rand() % BOARD_HEIGHT;

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

void	remove_player_from_board(t_board *board, t_player *player)
{
	player->alive = false;
	board->map[player->pos_x + player->pos_y * BOARD_WIDTH] = EMPTY_SLOT;
	board->player_count--;
}

int	check_player_around(t_board *board, t_player *player)
{
	int	cells[9];
	int	count = 0;
	for (int x = player->pos_x - 1; x <= player->pos_x + 1; x++) {
		for (int y = player->pos_y - 1; y <= player->pos_y + 1; y++) {
			if (x == player->pos_x && y == player->pos_y)
				continue;
			if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT)
				continue;
			int	team_id = board->map[x + y * BOARD_WIDTH];
			if (team_id > EMPTY_SLOT && team_id != player->team_id)
				cells[count++] = team_id;
		}
	}

	for (int i = 0; i < count; i++) {
		for (int j = i + 1; j < count; j++) {
			if (cells[i] == cells[j])
				return 1;
		}
	}

	return 0;
}

void	find_nearest_target(t_board *board, t_player *player, t_player *target)
{
	int	dist = BOARD_SIZE;
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			int	cell = board->map[x + y * BOARD_WIDTH];
			if (cell == EMPTY_SLOT || cell == player->team_id)
				continue;
			int	current_dist = GET_DIST(player->pos_x, player->pos_y, x, y);
			if (current_dist < dist) {
				dist = current_dist;
				target->pos_x = x;
				target->pos_y = y;
				target->team_id = cell;
			}
		}
	}
}

void	update_player_pos(t_board *board, t_player *player, int new_x, int new_y)
{
	if (new_x < 0 || new_x >= BOARD_WIDTH || new_y < 0 || new_y >= BOARD_HEIGHT)
		return;
	board->map[player->pos_x + player->pos_y * BOARD_WIDTH] = EMPTY_SLOT;
	board->map[new_x + new_y * BOARD_WIDTH] = player->team_id;
	player->pos_x = new_x;
	player->pos_y = new_y;
}

void	random_move(t_board *board, t_player *player)
{
	int	tried_dir[__dir_count] = {0};
	for (int tries = 0; tries < __dir_count; tries++) {
		int	new_x = player->pos_x;
		int	new_y = player->pos_y;
		e_dir	dir = rand() % __dir_count;
		
		if (tried_dir[dir])
			continue;

		if (dir == UP)
			new_y--;
		else if (dir == DOWN)
			new_y++;
		else if (dir == LEFT)
			new_x--;
		else if (dir == RIGHT)
			new_x++;
		
		if (board->map[new_x + new_y * BOARD_WIDTH] == EMPTY_SLOT) {
			update_player_pos(board, player, new_x, new_y);
			break;
		}

		tried_dir[dir] = 1;
	}
}

static inline bool	is_empty_cell(t_board *board, int x, int y)
{
	return board->map[x + y * BOARD_WIDTH] == EMPTY_SLOT;
}

void	move_player(t_board *board, t_player *player, t_player *target)
{
	if (target->pos_x == -1) {
		random_move(board, player);
		return;
	}

	int	new_x = -1;
	int	new_y = -1;
	int	distance = GET_DIST(player->pos_x, player->pos_y, target->pos_x, target->pos_y);
	int	directions[4][2] = {
		{player->pos_x, player->pos_y - 1},
		{player->pos_x, player->pos_y + 1},
		{player->pos_x + 1, player->pos_y},
		{player->pos_x - 1, player->pos_y}};

	for (size_t i = 0; i < ARRAY_LEN(directions); i++) {
		int	dx = directions[i][__X_POS];
		int	dy = directions[i][__Y_POS];

		if (dx < 0 || dx >= BOARD_WIDTH || dy < 0 || dy >= BOARD_HEIGHT || !is_empty_cell(board, dx, dy))
			continue;

		int	current_dist = GET_DIST(dx, dy, target->pos_x, target->pos_y);
		if (current_dist < distance) {
			distance = current_dist;
			new_x = dx;
			new_y = dy;
		}
	}

	if (new_x != -1)
		update_player_pos(board, player, new_x, new_y);
	else
		random_move(board, player);
}