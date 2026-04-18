#include "libft.h"
#include "lemipc.h"

int	main(int argc, char **argv)
{
	if (argc < 2) { 
		ft_dprintf(STDERR_FILENO, "Usage: ./%s <team_id>\n", PROGRAM_NAME);
		return 1;
	}

	t_player	player = {
		.alive = true, 
		.pos_x = 0,
		.pos_y = 0, 
		.team_id = ft_atoi(argv[1])
	};
	if (player.team_id <= 0) { 
		ft_dprintf(STDERR_FILENO, "%s: Invalid team_id '%s'\n", PROGRAM_NAME, argv[1]);
		return 1;
	}

	t_ipc	ipc;
	if (init_ipc(&ipc)) {
		ft_dprintf(STDERR_FILENO, "%s: Failed to init ipc\n", PROGRAM_NAME);
		return 1;
	}

	for (size_t i = 0; i < (size_t)BOARD_SIZE; i++) {
		ft_printf("%d ", ipc.board->map[i]);
		if ((i + 1) % BOARD_WIDTH == 0)
		ft_printf("\n");
	}
	return 0;
}