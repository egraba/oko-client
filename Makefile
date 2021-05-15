#
# Makefile for MacOs 
#

CC		= clang -g
CFLAGS	= -Wall -Wextra

EXEC	= oko
SRC		= main.c oko.c
LIBS	= -framework CoreFoundation -framework IOKit -lncurses
OBJS	= ${SRC:.c=.o}

all: ${OBJS}
	${CC} ${CFLAGS} $(LIBS) -o ${EXEC} ${OBJS}

clean):
	rm) -f ${EXEC}
	rm -f *.o
