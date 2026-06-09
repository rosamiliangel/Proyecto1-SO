/*
jobs.c / jobs.h: Estructura de datos de la job table y funciones para agregar, vigilar y
listar procesos asíncronos en segundo plano
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <termios.h>

#define MAX_LINE_LEN 1024
#define MAX_ARGS 64    

typedef struct {
    pid_t pid;                 // Identificador único del proceso asignado por el Kernel de Linux
    int id;                    // Identificador secuencial interno de la shell para el usuario
    char command[MAX_LINE_LEN];// Almacena la cadena de texto original que lanzó el proceso
    char status[20];           // Estado actual del trabajo: "Running", "Suspended", "Done"
} Job;

// Tabla global de trabajos. En una shell madura podría ser una lista enlazada, 
// pero un arreglo dinámico o estático grande cumple perfectamente con el requerimiento[cite: 44].
Job job_table[100];
int job_count = 0;

void add_job(pid_t pid, const char* com) {
    
    //Evitar desbordamiento de memoria
    if (job_count >= 100) {
        fprintf(stderr, "ucvsh> Tabla de jobs llena\n");
    }else {
        job_table[job_count].pid = pid;
        job_table[job_count].id = job_count + 1;
        
        //copiar el comando de forma segura
        strncpy(job_table[job_count].command, com, sizeof(job_table[job_count].command) -1);

        //Estado inicial por defecto
        strcpy(job_table[job_count].status, "Running");

        job_count++;

    }
}

void list_jobs() {
    for (int i = 0; i < job_count; i++) {
        // Imprimir el ID, PID, comando y estado de cada job
        printf("[%d] PID: %d - %s - %s\n", job_table[i].id, job_table[i].pid, job_table[i].command, job_table[i].status);
    }
}