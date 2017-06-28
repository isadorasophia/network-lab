CC			= gcc
SOURCES		= $(shell find $(SOURCEDIR) -maxdepth 1 -name '*.c')
OBJS		= $(SOURCES:.c=) 
CFLAGS		=
MAKEFLAGS   = --no-print-directory

%: %.c
	$(CC) $(CFLAGS) $< -o $@ -lm -lncurses

all:
	@make --no-print-directory clean
	@make --no-print-directory $(OBJS);

clean:
	rm -rf $(OBJS)
