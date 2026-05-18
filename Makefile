NAME = lemipc 
BONUS_NAME = glemipc 
CC = cc
CFLAGS = -Wall -Werror -Wextra -I$(INCS_DIR) -I$(LIBFT_HEADER_PATH) -I$(RAYLIB_HEADER_PATH) -MMD -MP
LFLAGS = -L$(RAYLIB_LIB) -lraylib -Wl,-rpath,$(abspath $(RAYLIB_LIB)) -lm -ldl -pthread
RM	= rm -rf

INCS_DIR = ./incs/
INCS = $(INCS_DIR)/lemipc.h

SRCS =  srcs/main.c \
		srcs/ipc.c \
		srcs/board_lock.c \
		srcs/player.c \
		srcs/msg.c \
		srcs/game_utils.c \

SRCS_GRAPHIC = graphic/main.c \
			   srcs/ipc.c \
			   srcs/board_lock.c \
			   srcs/player.c \
			   srcs/msg.c \
			   srcs/game_utils.c \

OBJS_DIR = .objs/
OBJS = $(patsubst %.c, $(OBJS_DIR)%.o, $(SRCS))
OBJS_GRAPHIC = $(patsubst %.c, $(OBJS_DIR)%.o, $(SRCS_GRAPHIC))
D_FILES = $(OBJS:.o=.d)

LIBFT_PATH = ./lib/libft/
LIBFT_HEADER_PATH = $(LIBFT_PATH)includes/
LIBFT = $(LIBFT_PATH)libft.a

RAYLIB_LIB = ./lib/raylib/lib/
RAYLIB_HEADER_PATH = ./lib/raylib/include/

all: $(NAME) $(BONUS_NAME)

$(NAME): $(OBJS) $(INCS) $(LIBFT) 
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

$(OBJS_DIR)%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_PATH)

$(BONUS_NAME): $(OBJS_GRAPHIC) $(INCS) $(LIBFT)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJS_GRAPHIC) $(LIBFT) -o $(BONUS_NAME)

clean:
	$(RM) $(OBJS_DIR) ./lemipc_key
	$(MAKE) clean -C $(LIBFT_PATH)

fclean: clean
	$(RM) $(NAME) $(BONUS_NAME)
	$(MAKE) fclean -C $(LIBFT_PATH)

re: fclean
	$(MAKE) all

sinclude $(D_FILES)

.PHONY: all clean fclean re