#include "libft.h"
#include "lemipc.h"

inline void	sem_lock(t_ipc *ipc)
{
	struct sembuf	op = {0, +1, 0};
	if (semop(ipc->sem_id, &op, 1) < 0) {
		ft_dprintf(STDERR_FILENO, "%s: sem_lock semop: %s\n", PROGRAM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

inline void	sem_unlock(t_ipc *ipc)
{
	struct sembuf	op = {0, -1, 0};
	if (semop(ipc->sem_id, &op, 1) < 0) {
		ft_dprintf(STDERR_FILENO, "%s: sem_unlock semop: %s\n", PROGRAM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}
}