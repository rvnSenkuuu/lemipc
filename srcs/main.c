#include "libft.h"
#include "lemipc.h"

void	display_map(int *map)
{
	ft_dprintf(STDOUT_FILENO, "\033[H\033[2J"); //clear screen
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		ft_printf("%d ", map[i]);
		if ((i + 1) % BOARD_WIDTH == 0)
			ft_printf("\n");
	}
}

void	start_game(t_ipc *ipc, t_player *player)
{
	int i = 0;
	while (player->alive) {
		sem_lock(ipc);
		
		t_msg	msg;
		t_player	target = {.pos_x = -1, .pos_y = -1};
		
		if (!receive_target_from_team(ipc, &msg, player->team_id)) {
			target.pos_x = msg.target_x;
			target.pos_y = msg.target_y;
			target.team_id = msg.target_id;
		}
		
		//TODO: more game end check condition
		if (check_player_around(ipc->board, player) || i == 30) {
			remove_player_from_board(ipc->board, player);
			sem_unlock(ipc);
			ft_dprintf(STDOUT_FILENO, "Player is dead\n");
			break;
		}
		display_map(ipc->board->map);
		
		if (target.pos_x == -1)
			find_nearest_target(ipc->board, player, &target);
		send_target_to_team(ipc, &target, player->team_id);
		
		//TODO: move player to the target if we cannot do a random move
		random_move_player(ipc, player);
		sem_unlock(ipc);
		sleep(1);
		i++;
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
	start_game(&ipc, &player);
	clean_ipc(&ipc);
	return 0;
}