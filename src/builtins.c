/*
builtins.c / builtins.h: Funciones aisladas para los built-ins (cd, exit y lectura/escritura
del historial)
*/

//Librerias completas de forma provisional
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <termios.h>
#include "../include/parser.h"
#include "../include/jobs.h"
#include "../include/builtins.h"
#include "../include/executor.h"

int RevisarBuiltIn(char **args, int *UltimoEstado){

    if (strcmp(args[0], "exit") == 0) {exit(0);} //Salir de la Shell
    
    //Listar jobs, actualizar estado y avisar que si es un BultIn
    if (strcmp(args[0], "jobs") == 0) {list_jobs(); *UltimoEstado = 0; return 1;}

    //Caso cd meramente por el caso de ejemplo en el pdf
    if (strcmp(args[0], "cd") == 0) {

        // cd manda a home
        if (args[1] == NULL) {chdir(getenv("HOME"));} else {if (chdir(args[1]) != 0) {perror("ucvsh: cd");}}
        
        *UltimoEstado = 0; 
        
        return 1;

    }
    
    return 0; //Se retona indicando que no era un BuiltIn
    
}