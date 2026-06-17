#include <raylib.h>
#include <raymath.h>
#include "lemipc.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_TITLE "Lemipc"

const char	*get_game_state(e_game_state state)
{
	if (state == GAME_RUNNING)
		return "Running";
	else if (state == GAME_WAITING)
		return "Waiting";
	else if (state == GAME_DRAW)
		return "Draw";
	else
		return "Finished";
}

static void	draw_game(t_ipc *ipc, int team_count, Color *team_color)
{
	int	player_count = 0;
	int	board_width = SCREEN_WIDTH - 200;
	int	board_height = SCREEN_HEIGHT;
	float	cell_width = (float)board_width / BOARD_WIDTH;
	float	cell_height = (float)board_height / BOARD_HEIGHT;
	float	slice = 360.0f / team_count;

	sem_lock(ipc);
	e_game_state	state = ipc->board->state;
	const int	*map = ipc->board->map;
	sem_unlock(ipc);

    ClearBackground(BLACK);
	DrawRectangle(0, 0, board_width, board_height, GetColor(0x181818FF));
	for (int x = 0; x < BOARD_WIDTH; x++) {
		for (int y = 0; y < BOARD_HEIGHT; y++) {
			int	cell = map[x + y * BOARD_WIDTH];
			if (cell == EMPTY_CELL)
				continue;

			player_count++;
			Rectangle	player = {
				.x = x * cell_width,
				.y = y * cell_height,
				.width = cell_width - 2,
				.height = cell_height - 2};
			float	hue = fmod((cell - 1) * slice, 360.0f);
			Color	player_color = ColorFromHSV(hue, 0.85f, 0.95f);
			*team_color = player_color;
			DrawRectangleRounded(player, 0.2f, 0, player_color);
		}
	}

	int	hud_pos_x = (SCREEN_WIDTH - 200) + 15;
	int	hud_pos_y = 15;
	int	font_size = 18;
	DrawText(TextFormat("Game State: %s", get_game_state(state)), hud_pos_x, hud_pos_y, font_size, RAYWHITE);
	DrawText(TextFormat("Player Count: %d", player_count), hud_pos_x, hud_pos_y + 40, font_size, RAYWHITE);
}

static void	draw_end_game(t_ipc *ipc, Color *winner_team_color)
{
	size_t	font_size = 20;

	sem_lock(ipc);
	e_game_state	final_state = ipc->board->state;
	int	winner = ipc->board->winner_team;
	sem_unlock(ipc);

	BeginDrawing();
	ClearBackground(BLACK);
	if (final_state == GAME_DRAW) {
		DrawText("The game is draw", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, font_size, RAYWHITE);
	} else if (final_state == ONE_TEAM_REMAINING) {
		DrawText(TextFormat("Team %d won !", winner), SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2, font_size, RAYWHITE);
		Rectangle	rec = {
			.x = SCREEN_WIDTH / 2 - 45,
			.y = SCREEN_HEIGHT / 2 + 50, 
			.width = 80,
			.height = 80};
		DrawRectangleRounded(rec, 0.2f, 0, *winner_team_color);
	}
	EndDrawing();
}

void	graphical_mode(t_ipc *ipc, int team_count)
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		
		if (ipc->first_process) {
			DrawText("Unavailable board, quitting the game", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, 20, RAYWHITE);
			EndDrawing();
			WaitTime(2.0);
			break;
		}

		Color	team_color;
		draw_game(ipc, team_count, &team_color);

		sem_lock(ipc);
		e_game_state	current_state = ipc->board->state;
		if (current_state == GAME_DRAW || current_state == ONE_TEAM_REMAINING) {
			sem_unlock(ipc);
			EndDrawing();
			draw_end_game(ipc, &team_color);
			WaitTime(3.0);
			break;
		}
		sem_unlock(ipc);
		EndDrawing();
	}
	CloseWindow();
}