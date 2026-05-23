#include <raylib.h>
#include <raymath.h>
#include "libft.h"
#include "lemipc.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_TITLE "Lemipc"

const char	*get_game_state(e_game_state state)
{
	const char	*game_state = NULL;
	switch (state) {
	case GAME_RUNNING:
		game_state = "Running";
		break;
	case GAME_WAITING:
		game_state = "Waiting";		
		break;
	case GAME_DRAW:
	case ONE_TEAM_REMAINING:
		game_state = "Finished";
		break;
	default:
		break;
	}
	return game_state;
}

void	draw_board(t_ipc *ipc)
{
	int	board_width = SCREEN_WIDTH - 200;
	int	board_height = SCREEN_HEIGHT;
	float	cell_width = (float)board_width / BOARD_WIDTH;
	float	cell_height = (float)board_height / BOARD_HEIGHT;

	sem_lock(ipc);
	const int	*map = ipc->board->map;

    ClearBackground(BLACK);
	DrawRectangle(0, 0, board_width, board_height, GetColor(0x181818FF));
	for (int x = 0; x < BOARD_WIDTH; x++) {
		for (int y = 0; y < BOARD_HEIGHT; y++) {
			int	cell = map[x + y * BOARD_WIDTH];
			if (cell == EMPTY_SLOT)
				continue;

			Rectangle	player = {
				.x = x * cell_width,
				.y = y * cell_height,
				.width = cell_width - 2,
				.height = cell_height - 2};
			//TODO: Pick correct color according to team id
			Color	player_color = (cell == 1) ? RED : BLUE;
			DrawRectangleRec(player, player_color);
		}
	}
	sem_unlock(ipc);
}

void	draw_hud(t_ipc *ipc)
{
	int	pos_x = (SCREEN_WIDTH - 200) + 15;
	int	pos_y = 15;

	sem_lock(ipc);
	e_game_state	state = ipc->board->state;
	int	player_count = ipc->board->player_count;
	sem_unlock(ipc);

	const char	*game_state = get_game_state(state);
	DrawText(TextFormat("Game State: %s", game_state), pos_x, pos_y, 18, RAYWHITE);
	DrawText(TextFormat("Player Count: %d", player_count), pos_x, pos_y + 40, 18, RAYWHITE);
}

void	draw_end_game(t_ipc *ipc)
{
	sem_lock(ipc);
	e_game_state	final_state = ipc->board->state;
	int	winner = ipc->board->winner_team;
	sem_unlock(ipc);

	BeginDrawing();
	ClearBackground(BLACK);
	if (final_state == GAME_DRAW)
		DrawText("The game is draw", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 20, RAYWHITE);
	else if (final_state == ONE_TEAM_REMAINING)
		DrawText(TextFormat("Team %d won !", winner), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 20, RAYWHITE);
	EndDrawing();
}

int	main(void)
{
	t_ipc	ipc = {0};
	if (init_ipc(&ipc))
		return 1;

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		
		if (ipc.first_process) {
			DrawText("Unavailable board, quitting the game", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, 20, RAYWHITE);
			EndDrawing();
			WaitTime(2.0);
			break;
		}

		draw_board(&ipc);
		draw_hud(&ipc);

		sem_lock(&ipc);
		e_game_state	current_state = ipc.board->state;
		if (current_state == GAME_DRAW || current_state == ONE_TEAM_REMAINING) {
			sem_unlock(&ipc);
			EndDrawing();
			draw_end_game(&ipc);
			WaitTime(3.0);
			break;
		}
		sem_unlock(&ipc);
		EndDrawing();
	}

	clean_ipc(&ipc);
	CloseWindow();
	return 0;
}