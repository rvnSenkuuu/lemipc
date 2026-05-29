#include <signal.h>
#include "libft.h"
#include "lemipc.h"

#define FtPrintf ft_printf
#define SemLock sem_lock
#define SemUnlock sem_unlock

static volatile sig_atomic_t	sig_running = 1;

const char	*GetGameState(e_game_state state);

static void	PrintCell(int cell)
{
	FtPrintf("[");
	if (cell == EMPTY_CELL)
		FtPrintf("  ");
	else if (cell >= 10)
		FtPrintf("%d", cell);
	else
		FtPrintf(" %d", cell);
	FtPrintf("]");
}

static void	DisplayGame(t_ipc *ipc)
{
	int	player_count = 0;

	SemLock(ipc);
	e_game_state	state = ipc->board->state;
	const int	*map = ipc->board->map;
	SemUnlock(ipc);

	FtPrintf("\033[H\033[2J");
	FtPrintf("========================================\n");
	FtPrintf("              LEMIPC GAME               \n");
	FtPrintf("========================================\n\n");

	for (size_t y = 0; y < BOARD_HEIGHT; y++) {
		for (size_t x = 0; x < BOARD_WIDTH; x++) {
			int	cell = map[x + y * BOARD_WIDTH];
			if (cell != EMPTY_CELL)
				player_count++;
			PrintCell(cell);
		}
		FtPrintf("\n");
	}

	FtPrintf("\n----------------------------------------\n");
	FtPrintf("Game State: %s\n", GetGameState(state));
	FtPrintf("Player Count: %d\n", player_count);
}

static void	DisplayEndGame(t_ipc *ipc)
{
	SemLock(ipc);
	e_game_state	final_state = ipc->board->state;
	int	winner = ipc->board->winner_team;
	SemUnlock(ipc);

	FtPrintf("\033[H\033[2J");
	FtPrintf("========================================\n");
	FtPrintf("             GAME  OVER                 \n");
	FtPrintf("========================================\n");

	if (final_state == GAME_DRAW)
		FtPrintf("The Game ended in a Draw !\n");
	else if (final_state == ONE_TEAM_REMAINING)
		FtPrintf("Team %d won the game\n", winner);
	FtPrintf("Exiting in 5 seconds...\n");
	sleep(5);
}

static void	handle_sigint(int sig)
{
    (void)sig;
    sig_running = 0;
}

void	TextMode(t_ipc *ipc)
{
	signal(SIGINT, handle_sigint);
	while (sig_running) {
		if (ipc->first_process) {
			FtPrintf("Unavailable board, quitting the game\n");
			sleep(2);
			break;
		}

		DisplayGame(ipc);

		SemLock(ipc);
		e_game_state	current_state = ipc->board->state;
		if (current_state == GAME_DRAW || current_state == ONE_TEAM_REMAINING) {
			SemUnlock(ipc);
			DisplayEndGame(ipc);
			break;
		}
		SemUnlock(ipc);
		usleep(100000);
	}
	if (!sig_running)
		FtPrintf("Visualizer stopped by user with Ctrl-C\n");
}