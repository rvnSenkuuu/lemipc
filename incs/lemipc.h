#ifndef LEMIPC_H
#define LEMIPC_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define PROGRAM_NAME "lemipc"
#define IPC_KEY_PATH "./lemipc_key"
#define IPC_KEY_ID 0x4242

#define BOARD_WIDTH 20
#define BOARD_HEIGHT 11
#define BOARD_SIZE BOARD_WIDTH * BOARD_HEIGHT
#define EMPTY_SLOT 0
#define MIN_PLAYER 4

#define __X_POS 0
#define __Y_POS 1

#define GET_DIST(x1, y1, x2, y2) (abs(x1 - x2) + abs(y1 - y2))
#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))
#define TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	__dir_count,
} e_dir;

typedef enum {
	GAME_RUNNING,
	GAME_WAITING,
	PLAYER_DEAD,
	ONE_TEAM_REMAINING,
	GAME_DRAW
} e_game_state;

typedef struct {
	long	team_id;
	int	target_x;
	int	target_y;
	int	target_id;
} t_msg;

typedef struct {
	e_game_state	state;
	int	client_count;
	int	player_count;
	int	map[BOARD_SIZE];
} t_board;

typedef struct {
	bool	alive;
	int	pos_x;
	int	pos_y;
	int	team_id;
} t_player;

typedef struct {
	bool	first_process;
	int	shm_id;
	int	sem_id;
	int	msgq_id;
	t_board	*board;
} t_ipc;

void	clean_ipc(t_ipc *ipc);
int	init_ipc(t_ipc *ipc);

void	sem_lock(t_ipc *ipc);
void	sem_unlock(t_ipc *ipc);

void	remove_player_from_board(t_board *board, t_player *player);
void	move_player(t_board *board, t_player *player, t_player *target);
void	random_move(t_board *board, t_player *player);
void	find_nearest_target(t_board *board, t_player *player, t_player *target);
int	put_player_on_board(t_ipc *ipc, t_player *player);
int	check_player_around(t_board *board, t_player *player);

void	send_target_to_team(t_ipc *ipc, t_player *target, int player_team_id);
int	receive_target_from_team(t_ipc *ipc, t_msg *msg, int team_id);

void	display_map(const int *map);
void	print_leave_reason(e_game_state state, int player_team_id);
int	check_end_condition(t_board *board, t_player *player);

#endif