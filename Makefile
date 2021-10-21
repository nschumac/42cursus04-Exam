NAME = microshell

SOURCES =	sources/microshell.c								\

FLAGS =  -Wall -Werror -Wextra

OBJECTS = $(SOURCES:.c=.o)

%.o: %.c
	@$(CC) $(CCFLAGS) -o $@ -c $<

all: $(NAME)

$(NAME): $(OBJECTS)
	@$(CC) $(FLAGS) $(OBJECTS) -o $(NAME)
	@echo microshell built!

clean:
	@rm -rf $(OBJECTS)

fclean: clean
	@rm -rf $(NAME)

re: fclean $(NAME)