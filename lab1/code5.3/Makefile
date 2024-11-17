CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = calc.c calc_logic.c
OBJ = $(SRC:.c=.o)
EXEC = calc

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

