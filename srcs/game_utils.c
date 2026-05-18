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

static int	find_team_index(const t_team_info *teams, const size_t team_count, const int player_id)
{
	for (size_t i = 0; i < team_count; i++) {
		if (teams[i].id == player_id)
			return i;
	}
	return -1;
}

static void	get_team_and_player_count(t_board *board, size_t *team_count, size_t *killer_team_count)
{
	size_t	team_nb = 0;
	const int	*map = board->map;
	t_team_info	*teams = NULL;

	for (size_t i = 0; i < BOARD_SIZE; i++) {
		int	player_id = map[i];
		if (player_id == EMPTY_SLOT)
			continue;

		int	team_index = find_team_index(teams, team_nb, player_id);
		if (team_index == -1) {
			t_team_info	*tmp = realloc(teams, sizeof(*teams) * (team_nb + 1));
			if (!tmp) {
				free(teams);
				teams = NULL;
				ft_dprintf(STDERR_FILENO, "%s: realloc: %s\n", PROGRAM_NAME, strerror(errno));
				exit(EXIT_FAILURE);
			}
			teams = tmp;
			teams[team_nb].id = player_id;
			teams[team_nb].player_count = 1;
			team_nb++;
		} else {
			teams[team_index].player_count++;
		}
	}

	for (size_t i = 0; i < team_nb; i++)
		if (teams[i].player_count >= 2)
			*killer_team_count += 1;
	*team_count = team_nb;
	free(teams);
	teams = NULL;
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

inline bool	is_empty_cell(t_board *board, int x, int y)
{
	return board->map[x + y * BOARD_WIDTH] == EMPTY_SLOT;
}