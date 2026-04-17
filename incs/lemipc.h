#ifndef LEMIPC_H
#define LEMIPC_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define PROGRAM_NAME "lemipc"

#define IPC_KEY_PATH "./lemipc_key"
#define IPC_KEY_ID 0x4242

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10
#define BOARD_SIZE BOARD_WIDTH * BOARD_HEIGHT

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

int	init_ipc(t_ipc *ipc);

#endif