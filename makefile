#Variables
CFLAGS = -Wall -Wextra -g -I./include -I../include
Target = ucvsh
SRCS = src/main.c src/parser.c src/executor.c src/jobs.c src/builtins.c src/history.c
Objs = $(SRCS:.c=.o)

all: $(Target)

$(Target): $(Objs)
	gcc $(CFLAGS) -o $(Target) $(Objs)

#Compilar archivos .o
src/%.o:.o: src/%	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(Target)

.PHONY: all clean