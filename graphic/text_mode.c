#include <signal.h>
#include <assert.h>
#include "libft.h"
#include "lemipc.h"

static volatile sig_atomic_t	sig_running = 1;

const char	*get_game_state(e_game_state state);

static char	*safe_strjoin(char *s1, char *s2)
{
	size_t	s1_len = ft_strlen(s1);
	size_t	s2_len = ft_strlen(s2);
	char	*output = malloc(sizeof(char) * (s1_len + s2_len) + 1);
	if (!output) {
		ft_dprintf(STDERR_FILENO, "%s: %s malloc: %s\n", PROGRAM_NAME, __func__, strerror(errno));
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < s1_len; i++)
		output[i] = s1[i];
	for (size_t i = 0; i < s2_len; i++, s1_len++)
		output[s1_len] = s2[i];
	output[s1_len] = '\0';
	free(s1);
	return output;
}

static void	print_cell(int cell, char **buffer)
{
	char	*tmp = NULL;
	
	*buffer = safe_strjoin(*buffer, "[");
	if (cell == EMPTY_CELL) {
		*buffer = safe_strjoin(*buffer, "  ");
	} else if (cell >= 10) {
		tmp = ft_itoa(cell);
		*buffer = safe_strjoin(*buffer, tmp);
	} else {
		*buffer = safe_strjoin(*buffer, " ");
		tmp = ft_itoa(cell);
		*buffer = safe_strjoin(*buffer, tmp);
	}
	*buffer = safe_strjoin(*buffer, "]");
	
	free(tmp);
	tmp = NULL;
}

static void	display_game(t_ipc *ipc)
{
	char	*buffer = NULL;
	char	*tmp = NULL;
	int	player_count = 0;

	sem_lock(ipc);
	e_game_state	state = ipc->board->state;
	const int	*map = ipc->board->map;
	sem_unlock(ipc);

	char	*header = "\033[H\033[2J"
	"========================================\n"
	"              LEMIPC GAME               \n"
	"========================================\n\n";

	buffer = safe_strjoin(buffer, header);

	for (size_t y = 0; y < BOARD_HEIGHT; y++) {
		for (size_t x = 0; x < BOARD_WIDTH; x++) {
			int	cell = map[x + y * BOARD_WIDTH];
			if (cell != EMPTY_CELL)
				player_count++;
			print_cell(cell, &buffer);
		}
		buffer = safe_strjoin(buffer, "\n");
	}

	buffer = safe_strjoin(buffer, "\n----------------------------------------\n");
	buffer = safe_strjoin(buffer, "Game State: ");
	buffer = safe_strjoin(buffer, (char *)get_game_state(state));
	buffer = safe_strjoin(buffer, "\n");
	buffer = safe_strjoin(buffer, "Player Count: ");
	tmp = ft_itoa(player_count);
	buffer = safe_strjoin(buffer, tmp);
	buffer = safe_strjoin(buffer, "\n");

	write(STDOUT_FILENO, buffer, ft_strlen(buffer));
	
	free(buffer);
	free(tmp);
	buffer = NULL;
	tmp = NULL;
}

static void	display_end_game(t_ipc *ipc)
{
	sem_lock(ipc);
	e_game_state	final_state = ipc->board->state;
	int	winner = ipc->board->winner_team;
	sem_unlock(ipc);

	char	*tmp = NULL;
	char	*buffer = NULL;
	char	*end_header = "\033[H\033[2J"
	"========================================\n"
	"             GAME  OVER                 \n"
	"========================================\n";
	
	buffer = safe_strjoin(buffer, end_header);

	if (final_state == GAME_DRAW) {
		buffer = safe_strjoin(buffer, "The Game ended in a Draw !\n");
	} else if (final_state == ONE_TEAM_REMAINING) {
		buffer = safe_strjoin(buffer, "Team ");
		tmp = ft_itoa(winner);
		buffer = safe_strjoin(buffer, tmp);
		buffer = safe_strjoin(buffer, " won the game\n");
	}
	buffer = safe_strjoin(buffer, "Exiting in 5 seconds...\n");
	
	write(STDOUT_FILENO, buffer, ft_strlen(buffer));
	
	free(buffer);
	free(tmp);
	buffer = NULL;
	tmp = NULL;
	sleep(5);
}

static void	handle_sigint(int sig)
{
    (void)sig;
    sig_running = 0;
}

void	text_mode(t_ipc *ipc)
{
	signal(SIGINT, handle_sigint);
	while (sig_running) {
		if (ipc->first_process) {
			ft_printf("Unavailable board, quitting the game\n");
			sleep(2);
			break;
		}

		display_game(ipc);

		sem_lock(ipc);
		e_game_state	current_state = ipc->board->state;
		if (current_state == GAME_DRAW || current_state == ONE_TEAM_REMAINING) {
			sem_unlock(ipc);
			display_end_game(ipc);
			break;
		}
		sem_unlock(ipc);
		usleep(100000);
	}
	if (!sig_running)
		ft_printf("Visualizer stopped by user with Ctrl-C");
}