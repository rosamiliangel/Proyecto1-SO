#Directorios
ObjDir = ./obj
SrcDir = ./src
IncDir = ./include

#Variables
CFLAGS = -Wall -I$(IncDir) -Wextra -g -MMD
SRCS = main.c parser.c executor.c jobs.c builtins.c history.c
Objs = $(ObjDir)/builtins.o $(ObjDir)/executor.o $(ObjDir)/jobs.o $(ObjDir)/parser.o $(ObjDir)/history.o $(ObjDir)/main.o
Target = ucvsh

#Regla para make all
all: $(Target)

#Compilar el binario
$(Target): $(Objs)
	gcc $(CFLAGS) $(Objs) -o $(Target)

#Crear directorio obj si no exite
$(ObjDir):
	mkdir -p $(ObjDir)

#Compilar archivos .o
$(ObjDir)/%.o : $(SrcDir)/%.c | $(ObjDir)
	gcc -c $(CFLAGS) $< -o $@

##Incluir archivos .d para rastrear los .h
-include $(ObjDir)/*.d

.PHONY: clean
clean:
	rm -rf $(Target)
	rm -rf $(ObjDir)

