#include "lemipc.h"

int	put_player_on_board(t_ipc *ipc, t_player *player)
{
	sem_lock(ipc);
	if (ipc->board->player_count == BOARD_SIZE) {
		sem_unlock(ipc);
		return 1;
	}

	int	count = 0;
	int	empty_cell[BOARD_SIZE][2];
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (ipc->board->map[i] == EMPTY_CELL) {
			empty_cell[count][__X_POS] = i % BOARD_WIDTH;
			empty_cell[count][__Y_POS] = i / BOARD_WIDTH;
			count++;
		}
	}

	if (count == 0) {
		sem_unlock(ipc);
		return 1;
	}

	int	cell = rand() % count;
	player->pos_x = empty_cell[cell][__X_POS];
	player->pos_y = empty_cell[cell][__Y_POS];
	ipc->board->map[player->pos_x + player->pos_y * BOARD_WIDTH] = player->team_id;
	ipc->board->player_count++;
	sem_unlock(ipc);
	return 0;
}

static inline bool	search_team_index(int cells[9], int count, int search_id)
{
	for (int i = 0; i < count; i++)
		if (cells[i] == search_id)
			return true;
	return false;
}

int	check_player_around(t_board *board, t_player *player)
{
	int	cells[9] = {0};
	int	count = 0;
	for (int x = player->pos_x - 1; x <= player->pos_x + 1; x++) {
		for (int y = player->pos_y - 1; y <= player->pos_y + 1; y++) {
			if ((x == player->pos_x && y == player->pos_y) || check_map_bound(x, y))
				continue;
			int	team_id = board->map[x + y * BOARD_WIDTH];
			if (team_id > EMPTY_CELL && team_id != player->team_id) {
				if (search_team_index(cells, count, team_id))
					return 1;
				cells[count] = team_id;
				count++;
			}
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
			if (cell == EMPTY_CELL || cell == player->team_id)
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

void	update_target(t_ipc *ipc, t_player *player, t_player *target)
{
	t_msg	msg = {0};
	
	if (!receive_target_from_team(ipc, &msg, player->team_id)) {
		t_player	team_target = {
			.pos_x = msg.target_x,
			.pos_y = msg.target_y,
			.team_id = msg.target_id};
		if (is_valid_target(ipc->board, &team_target)) {
			*target = team_target;
			return;
		}
	}
 
	target->pos_x = -1;
	target->pos_y = -1;
	target->team_id = -1;
	find_nearest_target(ipc->board, player, target);
}

void	update_player_pos(t_board *board, t_player *player, int new_x, int new_y)
{
	if (check_map_bound(new_x, new_y) || !is_empty_cell(board, new_x, new_y))
		return;
	board->map[player->pos_x + player->pos_y * BOARD_WIDTH] = EMPTY_CELL;
	board->map[new_x + new_y * BOARD_WIDTH] = player->team_id;
	player->pos_x = new_x;
	player->pos_y = new_y;
}

void	random_move(t_board *board, t_player *player)
{
	int	dirs[__dir_count] = {UP, DOWN, LEFT, RIGHT};
	for (int i = __dir_count - 1; i > 0; i--) {
		int	j = rand() % (i + 1);
		swap(&dirs[i], &dirs[j]);
	}

	for (int i = 0; i < __dir_count; i++) {
		int	new_x = player->pos_x;
		int	new_y = player->pos_y;

		if (dirs[i] == UP)
			new_y--;
		else if (dirs[i] == DOWN)
			new_y++;
		else if (dirs[i] == LEFT)
			new_x--;
		else if (dirs[i] == RIGHT)
			new_x++;
		
		if (!check_map_bound(new_x, new_y) && is_empty_cell(board, new_x, new_y)) {
			update_player_pos(board, player, new_x, new_y);
			return;
		}
	}
}

void	move_player(t_board *board, t_player *player, t_player *target)
{
	if (target->pos_x == -1) {
		random_move(board, player);
		return;
	}

	int	new_x = -1;
	int	new_y = -1;
	int	current_dist = GET_DIST(player->pos_x, player->pos_y, target->pos_x, target->pos_y);
	int	best_dist = INT_MAX;

	int	directions[4][2] = {
		{player->pos_x, player->pos_y - 1},
		{player->pos_x, player->pos_y + 1},
		{player->pos_x + 1, player->pos_y},
		{player->pos_x - 1, player->pos_y}};

	for (int i = 3; i > 0; i--) {
		int	j = rand() % (i + 1);
		swap(&directions[i][__X_POS], &directions[j][__X_POS]);
		swap(&directions[i][__Y_POS], &directions[j][__Y_POS]);
	}

	for (size_t i = 0; i < ARRAY_LEN(directions); i++) {
		int	dx = directions[i][__X_POS];
		int	dy = directions[i][__Y_POS];

		if (check_map_bound(dx, dy)|| !is_empty_cell(board, dx, dy))
			continue;

		int	dist = GET_DIST(dx, dy, target->pos_x, target->pos_y);
		if (dist < current_dist) {
			update_player_pos(board, player, dx, dy);
			return;
		}
		if (dist < best_dist) {
			best_dist = dist;
			new_x = dx;
			new_y = dy;
		}
	}

	if (new_x != -1 && current_dist == best_dist) {
		update_player_pos(board, player, new_x, new_y);
		return;
	}
	random_move(board, player);
}