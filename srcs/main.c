#include "libft.h"
#include "lemipc.h"

static void	wait_for_players(t_ipc *ipc)
{
	sem_lock(ipc);
	while (ipc->board->player_count < MIN_PLAYER && ipc->board->state == GAME_WAITING) {
		ft_dprintf(STDOUT_FILENO, "Waiting for player...\n");
		sleep(1);
	}
	if (ipc->board->state == PLAYER_DEAD
		|| ipc->board->state == ONE_TEAM_REMAINING
		|| ipc->board->state == GAME_DRAW) {
			sem_unlock(ipc);
			return;
		}

	ipc->board->state = GAME_RUNNING;
	sem_unlock(ipc);
}

static void	leave_game(t_ipc *ipc, t_player *player)
{
	remove_player_from_board(ipc->board, player);
	print_leave_reason(ipc->board->state, player->team_id);
}

static void	start_game(t_ipc *ipc, t_player *player)
{
	while (1) {
		sem_lock(ipc);
		ipc->board->state = check_end_condition(ipc->board, player);
		if (ipc->board->state != GAME_RUNNING) {
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
		send_target_to_team(ipc, &target, player->team_id);
		move_player(ipc->board, player, &target);
		sem_unlock(ipc);
		sleep(1);
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

	srand(time(NULL));
	wait_for_players(&ipc);
	start_game(&ipc, &player);
	clean_ipc(&ipc);
	return 0;
}