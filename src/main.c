//main.c: Cciclo REPL (imprimir prompt, leer, evaluar, ejecutar)

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


#define MAX_LINE_LEN 1024  // Longitud máxima de una línea de comandos leída del usuario [cite: 9]
#define MAX_ARGS 64  

int main() {
    char linea[MAX_LINE_LEN];
    char *args[MAX_ARGS];

    //Ciclo REPL (Read-Evaluate-Print-Loop)
    while(1) {
        printf("ucvsh> ");
        fflush(stdout); //Limpiar el buffer para asegurar que el prompt se vea
         
        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            break;
        }

        //Limpiar salto de linea de fgets
        linea[strcspn(linea, "\n")] = '\0';

        //Caso de cadena vacia
        if(strlen(linea) == 0){continue;}

        //Salir de la Shell
        if (strcmp(linea, "exit") == 0) {
            break;
        }

        int background_flag = parse_line(linea, args);

        launch_external_command(args, background_flag);

    }
}