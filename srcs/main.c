#include "libft.h"
#include "lemipc.h"

static void wait_for_players(t_ipc *ipc)
{
    while (1) {
		sem_lock(ipc);
		if (ipc->board->player_count >= MIN_PLAYER || ipc->board->state != GAME_WAITING)
			break;
		sem_unlock(ipc);
		ft_dprintf(STDOUT_FILENO, "Waiting for player...\n");
		sleep(1);
    }
	if (ipc->board->state == GAME_RUNNING || ipc->board->state == GAME_WAITING)
		ipc->board->state = GAME_RUNNING;
	sem_unlock(ipc);
}

static void	leave_game(t_ipc *ipc, t_player *player)
{
	sem_lock(ipc);
	e_game_state	state = ipc->board->state;
	if (state == ONE_TEAM_REMAINING && ipc->board->winner_team == 0)
		ipc->board->winner_team = find_winner_team(ipc->board);

	remove_player_from_board(ipc->board, player);
	if (0)
		print_leave_reason(state, player);
	sem_unlock(ipc);
}

void	display_map(const int *map)
{
	ft_dprintf(STDOUT_FILENO, "\033[H\033[2J"); //clear screen
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		ft_printf("%d ", map[i]);
		if ((i + 1) % BOARD_WIDTH == 0)
			ft_printf("\n");
	}
}

static void	start_game(t_ipc *ipc, t_player *player)
{
	while (1) {
		sem_lock(ipc);
		ipc->board->state = check_end_condition(ipc->board, player);
		if (ipc->board->state != GAME_RUNNING || player->alive == false) {
			leave_game(ipc, player);
			sem_unlock(ipc);
			break;
		}
		
		t_msg	msg;
		t_player	target = {
			.alive = false,
			.pos_x = -1,
			.pos_y = -1,
			.team_id = -1};
		if (!receive_target_from_team(ipc, &msg, player->team_id)) {
			target.pos_x = msg.target_x;
			target.pos_y = msg.target_y;
			target.team_id = msg.target_id;
		} else {
			find_nearest_target(ipc->board, player, &target);
		}
		if (0)
			display_map(ipc->board->map);
		move_player(ipc->board, player, &target);
		sem_unlock(ipc);
		sleep(1);
		// usleep(500000 / (1 * ipc->board->player_count));
		send_target_to_team(ipc, &target, player->team_id);
	}
}

int	main(int argc, char **argv)
{
	if (argc < 2) { 
		ft_dprintf(STDERR_FILENO, "Usage: %s <team_id>\n", PROGRAM_NAME);
		return 1;
	}

	t_player	player = {
		.alive = true, 
		.pos_x = 0,
		.pos_y = 0, 
		.team_id = ft_atoi(argv[1])};
	if (player.team_id <= 0) { 
		ft_dprintf(STDERR_FILENO, "%s: Invalid team_id '%s'\n", PROGRAM_NAME, argv[1]);
		return 1;
	}

	t_ipc	ipc;
	if (init_ipc(&ipc)) {
		ft_dprintf(STDERR_FILENO, "%s: Failed to init ipc\n", PROGRAM_NAME);
		return 1;
	}

	if (put_player_on_board(&ipc, &player)) {
		ft_dprintf(STDERR_FILENO, "%s: Could not put player on the board\n", PROGRAM_NAME);
		return 1;
	}

	srand(time(NULL) ^ getpid());
	wait_for_players(&ipc);
	start_game(&ipc, &player);
	clean_ipc(&ipc);
	return 0;
}