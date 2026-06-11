/*
jobs.c / jobs.h: Estructura de datos de la job table y funciones para agregar, vigilar y 
listar procesos asíncronos en segundo plano
*/

#ifndef jobs_h
#define jobs_h
#include <sys/types.h>

#define MAX_LINE_LEN_Jobs 1024
#define MAX_ARGS 64    

typedef struct {
    pid_t pid;                          // Id asignado por el Kernel
    int id;                             // Id interno de la shell
    char command[MAX_LINE_LEN_Jobs];    // Almacenar la cadena original
    char status[20];                    // Estado actual del trabajo: "Running", "Suspended", "Done"
} Job;


extern Job job_table[100]; // Tabla de trabajos
extern int job_count;      // Contador de trabajos

//Agregar trabajo
void add_job(pid_t pid, const char* com);

//Imprimir trabajos
void list_jobs();

//Limpiar procesos zombies
void check_jobs();

#endif