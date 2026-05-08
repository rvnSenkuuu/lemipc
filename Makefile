NAME    = lemipc
CC      = cc
CFLAGS  = -Wall -Werror -Wextra -I$(INCS_DIR) -I$(LIBFT_HEADER_PATH) -MMD -MP
RM      = rm -rf

INCS_DIR    = ./incs/
INCS    = $(INCS_DIR)/lemipc.h

SRCS    = srcs/main.c \
			srcs/ipc.c \
			srcs/board_lock.c \
			srcs/player.c \
			srcs/msg.c

OBJS_DIR = .objs/
OBJS    = $(patsubst %.c, $(OBJS_DIR)%.o, $(SRCS))
D_FILES = $(OBJS:.o=.d)

LIBFT_PATH = ./lib/libft/
LIBFT_HEADER_PATH = $(LIBFT_PATH)includes/
LIBFT      = $(LIBFT_PATH)libft.a

all: $(NAME)

$(NAME): $(OBJS) $(INCS) $(LIBFT) 
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

$(OBJS_DIR)%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_PATH)

clean:
	$(RM) $(OBJS_DIR) ./lemipc_key
	$(MAKE) clean -C $(LIBFT_PATH)

fclean: clean
	$(RM) $(NAME)
	$(MAKE) fclean -C $(LIBFT_PATH)

re: fclean
	$(MAKE) all

sinclude $(D_FILES)

.PHONY: all clean fclean re