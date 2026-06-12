/*
jobs.c / jobs.h: Estructura de datos de la job table y funciones para agregar, vigilar y
listar procesos asíncronos en segundo plano
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include "../include/jobs.h"

#define MAX_LINE_LEN 1024
#define MAX_ARGS 64

//Variables globales
Job job_table[100];
int job_count = 0;

//Agregar trabajo
void add_job(pid_t pid, const char* com) {
    
    //Evitar desbordamiento de memoria
    if (job_count >= 100) {
        fprintf(stderr, "ucvsh> Tabla de jobs llena\n");
    }else {
        job_table[job_count].pid = pid;
        job_table[job_count].id = job_count + 1;
        
        //copiar el comando de forma segura
        strncpy(job_table[job_count].command, com, sizeof(job_table[job_count].command) -1);

        //Estado por defecto
        strcpy(job_table[job_count].status, "Running");

        job_count++;
    }
}

//Limpiar procesos zombies
void check_jobs() {
    int status;
    for (int i = 0; i < job_count; i++) {
        if (strcmp(job_table[i].status, "Running") == 0) {
            // Verificar si el proceso ha terminado
            pid_t result = waitpid(job_table[i].pid, &status, WNOHANG);
            if (result == -1) {
                // Error al esperar el proceso
                perror("waitpid");
            } else if (result == job_table[i].pid) {
                // El proceso ha terminado
                strcpy(job_table[i].status, "Done");
            }
        }
    }
}

//Imprimir trabajos
void list_jobs() {
    for (int i = 0; i < job_count; i++) {
        // Imprimir el ID, PID, comando y estado de los job
        printf("[%d] PID: %d - %s - %s\n", job_table[i].id, job_table[i].pid, job_table[i].command, job_table[i].status);
    }
}