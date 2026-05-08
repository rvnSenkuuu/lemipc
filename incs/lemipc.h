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

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 7
#define BOARD_SIZE BOARD_WIDTH * BOARD_HEIGHT
#define EMPTY_SLOT 0

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))
#define TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	__dir_count,
} e_dir;

typedef struct {
	long	team_id;
	int	target_x;
	int	target_y;
	int	target_id;
} t_msg;

typedef struct {
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
void	random_move_player(t_ipc *ipc, t_player *player);
void	find_nearest_target(t_board *board, t_player *player, t_player *target);
int	put_player_on_board(t_ipc *ipc, t_player *player);
int	check_player_around(t_board *board, t_player *player);

void	send_target_to_team(t_ipc *ipc, t_player *target, int player_team_id);
int	receive_target_from_team(t_ipc *ipc, t_msg *msg, int team_id);

#endif