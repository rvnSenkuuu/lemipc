#include <raylib.h>
#include <raymath.h>
#include "libft.h"
#include "lemipc.h"

#define InitIpc init_ipc
#define CleanIpc clean_ipc

void	TextMode(t_ipc *ipc);
void	GraphicalMode(t_ipc *ipc, int team_count);

static bool	CheckArgs(int argc, char **argv, int *team_count, bool *text_mode)
{
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "%s: Usage: ./glemipc <team_count> [optional]--text", PROGRAM_NAME);
		return false;
	}
	if (argc == 3) {
		if (ft_strncmp(argv[2], "--text", ft_strlen(argv[2]))) {
			fprintf(stderr, "%s: Unknown argument '%s'\n", PROGRAM_NAME, argv[2]);
			return false;
		}
		*text_mode = true;
	}
	*team_count = ft_atoi(argv[1]);
	if (*team_count < 2) {
		fprintf(stderr, "%s: Minimum 2 team is required\n", PROGRAM_NAME);
		return false;
	}
	return true;
}

int	main(int argc, char **argv)
{
	bool	text_mode = false;
	int	team_count = 0;
	if (!CheckArgs(argc, argv, &team_count, &text_mode))
		return 1;

	t_ipc	ipc = {0};
	if (InitIpc(&ipc))
		return 1;

	if (text_mode)
		TextMode(&ipc);
	else
		GraphicalMode(&ipc, team_count);

	CleanIpc(&ipc);
	return 0;
}