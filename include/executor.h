/*
executor.c / executor.h: Interacción con el Kernel (resolución de la variable $PATH,
llamadas a fork(), execvp(), waitpid() y redirección de descriptores)
*/

#ifndef executor_h
#define executor_h

void launch_external_command(char **args, int in_background);

#endif