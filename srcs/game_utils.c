#include "libft.h"
#include "lemipc.h"

static inline int	search_team_index(const t_team_info *teams, const size_t team_count, const int player_id)
{
	for (size_t i = 0; i < team_count; i++)
		if (teams[i].id == player_id)
			return i;
	return -1;
}

void	get_team_and_player_count(t_ipc *ipc, size_t *team_count, size_t *killer_team_count)
{
	size_t	team_nb = 0;
	const int	*map = ipc->board->map;
	t_team_info	*teams = NULL;
	
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		int	player_id = map[i];
		if (player_id == EMPTY_CELL)
			continue;

		int	team_index = search_team_index(teams, team_nb, player_id);
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

int find_winner_team(t_board *board)
{
    for (size_t i = 0; i < BOARD_SIZE; i++) {
        if (board->map[i] != EMPTY_CELL)
            return board->map[i];
    }
    return -1;
}

e_game_state	update_game_state(t_ipc *ipc)
{
	size_t	team_count = 0;
	size_t	killer_team_count = 0;
	get_team_and_player_count(ipc, &team_count, &killer_team_count);

	if (team_count == 1)
		return ONE_TEAM_REMAINING;
	if (team_count >= 2 && killer_team_count == 0)
		return GAME_DRAW;

	return GAME_RUNNING;
}

inline bool	is_empty_cell(t_board *board, int x, int y)
{
	return board->map[x + y * BOARD_WIDTH] == EMPTY_CELL;
}

inline bool	check_map_bound(int x, int y)
{
	return x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT;
}