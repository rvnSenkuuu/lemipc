#include <raylib.h>
#include <raymath.h>
#include "libft.h"
#include "lemipc.h"

void	text_mode(t_ipc *ipc);
void	graphical_mode(t_ipc *ipc, int team_count);

static bool	check_args(int argc, char **argv, int *team_count, bool *text_mode)
{
	if (argc < 2 || argc > 3) {
		ft_dprintf(STDERR_FILENO, "%s: Usage: ./glemipc <team_count> [optional]--text", PROGRAM_NAME);
		return false;
	}
	if (argc == 3) {
		if (ft_strncmp(argv[2], "--text", ft_strlen(argv[2]))) {
			ft_dprintf(STDERR_FILENO, "%s: Unknown argument '%s'\n", PROGRAM_NAME, argv[2]);
			return false;
		}
		*text_mode = true;
	}
	*team_count = ft_atoi(argv[1]);
	if (*team_count < 2) {
		ft_dprintf(STDERR_FILENO, "%s: Minimum 2 team is required\n", PROGRAM_NAME);
		return false;
	}
	return true;
}

int	main(int argc, char **argv)
{
	bool	text_mode_opt = false;
	int	team_count = 0;
	if (!check_args(argc, argv, &team_count, &text_mode_opt))
		return 1;

	t_ipc	ipc = {0};
	if (init_ipc(&ipc))
		return 1;

	if (text_mode_opt)
		text_mode(&ipc);
	else
		graphical_mode(&ipc, team_count);

	clean_ipc(&ipc);
	return 0;
}