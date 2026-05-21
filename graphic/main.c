#include <raylib.h>
#include <raymath.h>
#include "libft.h"
#include "lemipc.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_TITLE "Lemipc"

// const char	*get_game_state(e_game_state state)
// {
// 	const char	*game_state = NULL;
// 	switch (state) {
// 	case GAME_RUNNING:
// 		game_state = "Running";
// 		break;
// 	case GAME_WAITING:
// 		game_state = "Waiting";		
// 		break;
// 	default:
// 		break;
// 	}
// 	return game_state;
// }

// void	render_text(t_ipc *ipc)
// {
// 	sem_lock(ipc);
// 	int	pos_x = 15;
// 	int	pos_y = 15;
// 	const char	*game_state = get_game_state(ipc->board->state);
// 	DrawText(TextFormat("Game State: %s", game_state), pos_x, pos_y, 20, RAYWHITE);
// 	DrawText(TextFormat("Player Count: %d", ipc->board->player_count), pos_x + 250, pos_y, 20, RAYWHITE);
// 	sem_unlock(ipc);
// }

void	draw_board(t_ipc *ipc)
{
	sem_lock(ipc);
	int	board_width = SCREEN_WIDTH - 200;
	int	board_height = SCREEN_HEIGHT;
	float	cell_width = (float)board_width / BOARD_WIDTH;
	float	cell_height = (float)board_height / BOARD_HEIGHT;
	const int	*map = ipc->board->map;

    ClearBackground(BLACK);
	DrawRectangle(0, 0, board_width, board_height, DARKGRAY);
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

int	main(void)
{
	t_ipc	ipc = {0};
	if (init_ipc(&ipc))
		return 1;

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
	SetTargetFPS(60);
	while (!WindowShouldClose()) {

		BeginDrawing();
		if (ipc.first_process) {
			DrawText("Unavailable board, quitting the game", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, 20, RAYWHITE);
			EndDrawing();
			sleep(5);
			break;
		}

		draw_board(&ipc);
		EndDrawing();
	}

	clean_ipc(&ipc);
	CloseWindow();
	return 0;
}