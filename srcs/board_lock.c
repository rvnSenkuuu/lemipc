#include "libft.h"
#include "lemipc.h"

inline void sem_lock(t_ipc *ipc)
{
	struct sembuf	op = {0, -1, SEM_UNDO};
	while (semop(ipc->sem_id, &op, 1) == -1) {
		if (errno == EINTR)
			continue;
		ft_dprintf(STDERR_FILENO, "%s: %s semop: %s\n", PROGRAM_NAME, __func__, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

inline void sem_unlock(t_ipc *ipc)
{
	struct sembuf	op = {0, +1, SEM_UNDO};
	while (semop(ipc->sem_id, &op, 1) == -1) {
		if (errno == EINTR)
			continue;
		ft_dprintf(STDERR_FILENO, "%s: %s semop: %s\n", PROGRAM_NAME, __func__, strerror(errno));
		exit(EXIT_FAILURE);
	}
}