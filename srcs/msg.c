#include "libft.h"
#include "lemipc.h"

void	send_target_to_team(t_ipc *ipc, t_player *target, int player_team_id)
{
	t_msg	msg = {0};
	msg.team_id = player_team_id;
	msg.target_x = target->pos_x;
	msg.target_y = target->pos_y;
	msg.target_id = target->team_id;

	if (msgsnd(ipc->msgq_id, &msg, sizeof(msg) - sizeof(long), IPC_NOWAIT) < 0) {
		ft_dprintf(STDERR_FILENO, "%s: msgsnd: %s \n", PROGRAM_NAME, strerror(errno));
		return;
	}
}

int	receive_target_from_team(t_ipc *ipc, t_msg *msg, int team_id)
{
	if (msgrcv(ipc->msgq_id, msg, sizeof(*msg) - sizeof(long), team_id, IPC_NOWAIT) != -1)
		return 0;

	if (errno != EAGAIN && errno != ENOMSG)
		ft_dprintf(STDERR_FILENO, "%s: msgrcv: %s\n", PROGRAM_NAME, strerror(errno));
	return 1;
}