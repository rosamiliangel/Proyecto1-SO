#Directorios
ObjDir = ./obj
SrcDir = ./src
IncDir = ./include

#Variables
CFLAGS = -Wall -I$(IncDir) -Wextra -g -MMD
SRCS = main.c parser.c executor.c jobs.c builtins.c history.c
Objs = $(ObjDir)/builtins.o $(ObjDir)/executor.o $(ObjDir)/jobs.o $(ObjDir)/parser.o $(ObjDir)/history.o $(ObjDir)/main.o
Target = ucvsh

$(Target): $(Objs)
	gcc $(CFLAGS) $(Objs) -o $(Target)

#Compilar archivos .o
$(ObjDir)/%.o : $(SrcDir)/%.c
	gcc -c $(CFLAGS) $< -o $@

##Incluir archivos .h para compilar los archivos .o
-include $(ObjDir)/*.d

.PHONY: clean
clean:
	rm -rf $(Target)
	rm -rf $(ObjDir)/*.o
	rm -rf $(ObjDir)/*.d

