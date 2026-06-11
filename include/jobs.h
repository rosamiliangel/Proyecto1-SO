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
    pid_t pid;                          // Identificador único del proceso asignado por el Kernel de Linux
    int id;                             // Identificador secuencial interno de la shell para el usuario
    char command[MAX_LINE_LEN_Jobs];    // Almacena la cadena de texto original que lanzó el proceso
    char status[20];                    // Estado actual del trabajo: "Running", "Suspended", "Done"
} Job;

// Tabla global de trabajos. En una shell madura podría ser una lista enlazada, 
// pero un arreglo dinámico o estático grande cumple perfectamente con el requerimiento[cite: 44].
extern Job job_table[100];
extern int job_count;

void add_job(pid_t pid, const char* com);

void list_jobs();

void check_jobs();

#endif //