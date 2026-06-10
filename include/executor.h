/*
executor.c / executor.h: Interacción con el Kernel (resolución de la variable $PATH,
llamadas a fork(), execvp(), waitpid() y redirección de descriptores)
*/

#ifndef executor_h
#define executor_h

//agregada funcion search
int search_in_path(const char *cmd, char *full_path);

int launch_external_command(char **args, int in_background);

//Funcionalidad de pipes
void ejecutar_pipe(char **args_izq, char **args_der);

#endif