#include "libft.h"
#include "lemipc.h"

static void	create_key_path(const char *file)
{
	int	fd = open(file, O_CREAT, 0644);
	if (fd < 0) {
		ft_dprintf(STDERR_FILENO, "%s: open: \n", PROGRAM_NAME, strerror(errno));
		return;
	}
	close(fd);
}

static int	init_shm(t_ipc *ipc, key_t key)
{
	size_t	shm_sz = sizeof(t_board);
	
	ipc->shm_id = shmget(key, shm_sz, IPC_CREAT | IPC_EXCL | 0644);
	if (ipc->shm_id < 0) {
		ipc->shm_id = shmget(key, shm_sz, 0644);
		ipc->first_process = false;
	}

	t_board	*board = shmat(ipc->shm_id, NULL, 0);
	if (!board) {
		ft_dprintf(STDERR_FILENO, "%s: shmat: %s\n", PROGRAM_NAME, strerror(errno));
		return 1;
	}

	ipc->board = board;
	if (ipc->first_process) {
		ft_memset(ipc->board->map, 0, BOARD_SIZE * sizeof(int));
		ipc->board->client_count = 0;
		ipc->board->player_count = 0;
	}
	
	return 0;
}

static int	init_sem(t_ipc *ipc, key_t key)
{
	ipc->sem_id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
	if (ipc->sem_id < 0) {
		ipc->sem_id = semget(key, 1, 0644);
		if (ipc->sem_id < 0) {
			ft_dprintf(STDERR_FILENO, "%s: semget: %s\n", PROGRAM_NAME, strerror(errno));
			return 1;
		}
		return 0;
	}

	if (semctl(ipc->sem_id, 0, SETVAL, 1) < 0) {
		ft_dprintf(STDERR_FILENO, "%s: semctl: %s\n", PROGRAM_NAME, strerror(errno));
		return 1;
	}

	return 0;
}

static int	init_msgq(t_ipc *ipc, key_t key)
{
	ipc->msgq_id = msgget(key, IPC_CREAT | IPC_EXCL | 0644);
	if (ipc->msgq_id < 0) {
		ipc->msgq_id = msgget(key, 0644);
		if (ipc->msgq_id < 0) {
			ft_dprintf(STDERR_FILENO, "%s: msgget: %s\n", PROGRAM_NAME, strerror(errno));
			return 1;
		}
	}
	return 0;
}

void	clean_ipc(t_ipc *ipc)
{
	sem_lock(ipc);
	ipc->board->client_count--;

	if (ipc->board->client_count == 0) {
		sem_unlock(ipc);
		if (shmdt(ipc->board) < 0)
			ft_dprintf(STDERR_FILENO, "%s: shmdt: %s\n", PROGRAM_NAME, strerror(errno));
		if (shmctl(ipc->shm_id, IPC_RMID, NULL) < 0)
			ft_dprintf(STDERR_FILENO, "%s: shmctl(IPC_RMID): %s\n", PROGRAM_NAME, strerror(errno));
		if (semctl(ipc->sem_id, 0, IPC_RMID) < 0)
			ft_dprintf(STDERR_FILENO, "%s: semctl(IPC_RMID): %s\n", PROGRAM_NAME, strerror(errno));
		if (msgctl(ipc->msgq_id, IPC_RMID, NULL) < 0)
			ft_dprintf(STDERR_FILENO, "%s: msgctl(IPC_RMID): %s\n", PROGRAM_NAME, strerror(errno));
	} else {
		sem_unlock(ipc);
	}
}

int	init_ipc(t_ipc *ipc)
{
	create_key_path(IPC_KEY_PATH);
	key_t	key = ftok(IPC_KEY_PATH, IPC_KEY_ID);
	if (key < 0) {
		ft_dprintf(STDERR_FILENO, "%s: ftok: %s\n", PROGRAM_NAME, strerror(errno));
		return 1;
	}

	ipc->first_process = true;
	ipc->shm_id = -1;
	ipc->sem_id = -1;
	ipc->msgq_id = -1;
	ipc->board = NULL;

	if (init_shm(ipc, key)) {
		ft_dprintf(STDERR_FILENO, "%s: Failed to init shared memory\n", PROGRAM_NAME);
		return 1;
	}

	if (init_sem(ipc, key)) {
		ipc->board = NULL;
		ft_dprintf(STDERR_FILENO, "%s: Failed to init semaphore\n", PROGRAM_NAME);
		return 1;
	}

	if (init_msgq(ipc, key)) {
		ipc->board = NULL;
		ft_dprintf(STDERR_FILENO, "%s: Failed to init message queue\n", PROGRAM_NAME);
		return 1;
	}

	ipc->board->client_count++;
	return 0;
}