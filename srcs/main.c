#include "libft.h"
#include "lemipc.h"

static void wait_for_players(t_ipc *ipc, int player_count)
{
    while (1) {
		sem_lock(ipc);
		if (ipc->board->player_count >= player_count || ipc->board->state != GAME_WAITING)
			break;
		sem_unlock(ipc);
		usleep(100000);
    }
	if (ipc->board->state == GAME_RUNNING || ipc->board->state == GAME_WAITING)
		ipc->board->state = GAME_RUNNING;
	sem_unlock(ipc);
}

static void	leave_game(t_ipc *ipc, t_player *player)
{
	e_game_state	state = ipc->board->state;
	if (state == ONE_TEAM_REMAINING && ipc->board->winner_team == 0)
		ipc->board->winner_team = find_winner_team(ipc->board);

	player->alive = false;
	if (ipc->board->map[player->pos_x + player->pos_y * BOARD_WIDTH] == player->team_id)
		ipc->board->map[player->pos_x + player->pos_y * BOARD_WIDTH] = EMPTY_CELL;
	ipc->board->player_count--;
}

static void	start_game(t_ipc *ipc, t_player *player)
{
	t_player	target = {
			.alive = false,
			.pos_x = -1,
			.pos_y = -1,
			.team_id = -1};
	while (1) {
		sem_lock(ipc);
		e_game_state	current_state = ipc->board->state;
		if (current_state == ONE_TEAM_REMAINING || current_state == GAME_DRAW) {
			leave_game(ipc, player);
			sem_unlock(ipc);
			break;
		}
		
		if (check_player_around(ipc->board, player)) {
			leave_game(ipc, player);
			sem_unlock(ipc);
			break;
		}

		ipc->board->state = update_game_state(ipc);
		if (ipc->board->state != GAME_RUNNING) {
			leave_game(ipc, player);
			sem_unlock(ipc);
			break;
		}

		if (!is_valid_target(ipc->board, &target))
			update_target(ipc, player, &target);

		move_player(ipc->board, player, &target);
		sem_unlock(ipc);
		usleep(500000);
		if (target.pos_x != -1)
			send_target_to_team(ipc, &target, player->team_id);

	}
}

static bool	check_args(int argc, int team_id, int player_count)
{
	if (argc < 3) { 
		ft_dprintf(STDERR_FILENO, "Usage: %s <team_id> <player_count>\n", PROGRAM_NAME);
		return false;
	}
	if (team_id <= 0) {
		ft_dprintf(STDERR_FILENO, "%s: Invalid team_id '%d'\n", PROGRAM_NAME, team_id);
		return false;
	}
	if (player_count < 3) {
		ft_dprintf(STDERR_FILENO, "%s: Required minimum 3 player to put on the board\n", PROGRAM_NAME);
		return false;
	}
	return true;
}

int	main(int argc, char **argv)
{
	srand(time(NULL) ^ getpid());
	t_player	player = {
		.alive = true, 
		.pos_x = 0,
		.pos_y = 0, 
		.team_id = ft_atoi(argv[1])};
	int	player_count = ft_atoi(argv[2]);
	if (!check_args(argc, player.team_id, player_count))
		return 1;

	t_ipc	ipc = {0};
	if (init_ipc(&ipc)) {
		ft_dprintf(STDERR_FILENO, "%s: Failed to init ipc\n", PROGRAM_NAME);
		return 1;
	}
	if (put_player_on_board(&ipc, &player)) {
		ft_dprintf(STDERR_FILENO, "%s: Could not put player on the board\n", PROGRAM_NAME);
		return 1;
	}

	wait_for_players(&ipc, player_count);
	start_game(&ipc, &player);
	clean_ipc(&ipc);
	return 0;
}