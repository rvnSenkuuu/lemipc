#include <raylib.h>
#include <raymath.h>
#include "libft.h"
#include "lemipc.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_TITLE "Lemipc"

#define InitIpc init_ipc
#define CleanIpc clean_ipc
#define SemLock sem_lock
#define SemUnlock sem_unlock

static const char	*GetGameState(e_game_state state)
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

static void	DrawGame(t_ipc *ipc, int team_count, Color *team_color)
{
	int	board_width = SCREEN_WIDTH - 200;
	int	board_height = SCREEN_HEIGHT;
	float	cell_width = (float)board_width / BOARD_WIDTH;
	float	cell_height = (float)board_height / BOARD_HEIGHT;
	float	slice = 360.0f / team_count;

	SemLock(ipc);
	e_game_state	state = ipc->board->state;
	int	player_count = ipc->board->player_count;
	const int	*map = ipc->board->map;
	SemUnlock(ipc);

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
			float	hue = fmod((cell - 1) * slice, 360.0f);
			Color player_color = ColorFromHSV(hue, 0.85f, 0.95f);
			*team_color = player_color;
			DrawRectangleRec(player, player_color);
		}
	}

	int	hud_pos_x = (SCREEN_WIDTH - 200) + 15;
	int	hud_pos_y = 15;
	DrawText(TextFormat("Game State: %s", GetGameState(state)), hud_pos_x, hud_pos_y, 18, RAYWHITE);
	DrawText(TextFormat("Player Count: %d", player_count), hud_pos_x, hud_pos_y + 40, 18, RAYWHITE);
}

static void	DrawEndGame(t_ipc *ipc, Color *winner_team_color)
{
	SemLock(ipc);
	e_game_state	final_state = ipc->board->state;
	int	winner = ipc->board->winner_team;
	SemUnlock(ipc);

	BeginDrawing();
	ClearBackground(BLACK);
	if (final_state == GAME_DRAW) {
		DrawText("The game is draw", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 20, RAYWHITE);
	} else if (final_state == ONE_TEAM_REMAINING) {
		DrawText(TextFormat("Team %d won !", winner), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 20, RAYWHITE);
		DrawRectangle(SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 + 50, 80, 80, *winner_team_color);
	}
	EndDrawing();
}

static bool	CheckArgs(int argc, int team_count)
{
	if (argc < 2) {
		fprintf(stderr, "%s: Usage: ./glemipc <team_count>", PROGRAM_NAME);
		return false;
	}
	if (team_count < 2) {
		fprintf(stderr, "%s: Minimum 2 team is required\n", PROGRAM_NAME);
		return false;
	}
	return true;
}

int	main(int argc, char **argv)
{
	int	team_count = ft_atoi(argv[1]);
	if (!CheckArgs(argc, team_count))
		return 1;

	t_ipc	ipc = {0};
	if (InitIpc(&ipc))
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

		Color	team_color;
		DrawGame(&ipc, team_count, &team_color);

		SemLock(&ipc);
		e_game_state	current_state = ipc.board->state;
		if (current_state == GAME_DRAW || current_state == ONE_TEAM_REMAINING) {
			SemUnlock(&ipc);
			EndDrawing();
			DrawEndGame(&ipc, &team_color);
			WaitTime(3.0);
			break;
		}
		SemUnlock(&ipc);
		EndDrawing();
	}

	CleanIpc(&ipc);
	CloseWindow();
	return 0;
}