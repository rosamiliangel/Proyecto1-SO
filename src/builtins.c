/*
builtins.c / builtins.h: Funciones aisladas para los built-ins (cd, exit)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include "../include/jobs.h"
#include "../include/builtins.h"

//Revisar si el comando es un Builtin
int RevisarBuiltIn(char **args, int *UltimoEstado){

    if (strcmp(args[0], "exit") == 0) {exit(0);} //Salir de la shell
    
    //Listar jobs, actualizar estado y avisar que si es un BultIn
    if (strcmp(args[0], "jobs") == 0) {list_jobs(); *UltimoEstado = 0; return 1;}

    //Ejecuta el BultIn cd
    if (strcmp(args[0], "cd") == 0) {
        //Variable auxiliar
        int resultado;
        
        // Si no hay argumento se va a home, si lo hay se ingresa al directorio. Se guarda el resultado
        if (args[1] == NULL) {resultado = chdir(getenv("HOME"));} else {resultado = chdir(args[1]);}

        //Revisar si cd tuvo exito. Retorna 0 en caso de exito y otro valor en caso contrario. Actualizar UltimoEstado
        if (resultado != 0) {perror("ucvsh: cd");*UltimoEstado = 1;} else {*UltimoEstado = 0;}

        return 1;
    }

    // Caso fg: Traer un trabajo de background al primer plano
    if (strcmp(args[0], "fg") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "ucvsh: fg requiere el ID del trabajo (ej: fg 1)\n");
            *UltimoEstado = 1;
            return 1;
        }

        int target_id = atoi(args[1]);
        int encontrado = 0;

        for (int i = 0; i < job_count; i++) {
            // Buscar el ID que coincida y que siga corriendo
            if (job_table[i].id == target_id && strcmp(job_table[i].status, "Running") == 0) {
                encontrado = 1;
                printf("Trayendo a primer plano: %s (PID: %d)\n", job_table[i].command, job_table[i].pid);
                
                // Bloquear síncronamente hasta que el hijo termine de verdad
                int status;
                waitpid(job_table[i].pid, &status, 0);
                
                // Actualizar estado en la tabla interna
                strcpy(job_table[i].status, "Done");
                
                if (WIFEXITED(status)) {*UltimoEstado = WEXITSTATUS(status);} else {*UltimoEstado = -1;}

                break;
            }
        }

        //Imprimir si no se encuentra el trabajo
        if (!encontrado) {
            fprintf(stderr, "ucvsh: fg: trabajo [%d] no encontrado o ya finalizó\n", target_id);
            *UltimoEstado = 1;
        }
        return 1;
    }

    //Indica que no era un BuiltIn
    return 0; 
    
}