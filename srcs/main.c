#include "libft.h"
#include "lemipc.h"

void	create_key_path(const char *file)
{
	int	fd = open(file, O_CREAT, 0644);
	if (fd < 0) {
		ft_dprintf(STDERR_FILENO, "%s: open: \n", PROGRAM_NAME, strerror(errno));
		return;
	}
	close(fd);
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
		.team_id = ft_atoi(argv[1])
	};
	if (player.team_id <= 0) { 
		ft_dprintf(STDERR_FILENO, "%s: Invalid team_id '%s'\n", PROGRAM_NAME, argv[1]);
		return 1;
	}

	create_key_path(IPC_KEY_PATH);
	key_t	key = ftok(IPC_KEY_PATH, IPC_KEY_ID);
	if (key < 0) {
		ft_dprintf(STDERR_FILENO, "%s: ftok: %s\n", PROGRAM_NAME, strerror(errno));
		return 1;
	}

	

	return 0;
}