#include "libft.h"
#include "lemipc.h"

void	display_map(const int *map)
{
	ft_dprintf(STDOUT_FILENO, "\033[H\033[2J"); //clear screen
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		ft_printf("%d ", map[i]);
		if ((i + 1) % BOARD_WIDTH == 0)
			ft_printf("\n");
	}
}

static void	get_team_and_player_count(t_board *board, size_t *team_count, size_t *killer_team_count)
{
	int	teams[128] = {0};
	const int	*map = board->map;

	for (size_t i = 0; i < BOARD_SIZE; i++) {
		int	player = map[i];
		if (player > EMPTY_SLOT)
			teams[player]++;
	}

	for (size_t i = 0; i < ARRAY_LEN(teams); i++) {
		if (teams[i])
			*team_count += 1;
		if (teams[i] >= 2)
			*killer_team_count += 1;
	}
}

int	check_end_condition(t_board *board, t_player *player)
{
	if (check_player_around(board, player))
		return PLAYER_DEAD;

	size_t	team_count = 0;
	size_t	killer_team_count = 0;
	
	get_team_and_player_count(board, &team_count, &killer_team_count);
	if (team_count == 1)
		return ONE_TEAM_REMAINING;
	if (team_count >= 2 && killer_team_count == 0)
		return GAME_DRAW;

	return GAME_RUNNING;
}

void	print_leave_reason(e_game_state state, int player_team_id)
{
	switch (state) {
	case PLAYER_DEAD:
		ft_dprintf(STDOUT_FILENO, "Player (team %d) is dead\n", player_team_id);
		break;
	case ONE_TEAM_REMAINING:
		ft_dprintf(STDOUT_FILENO, "Team %d won the game\n", player_team_id);
		break;
	case GAME_DRAW:
		ft_dprintf(STDOUT_FILENO, "Game is draw\n");
		break;
	case GAME_RUNNING:
	case GAME_WAITING:
	default:
		break;
	}
}