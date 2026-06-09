/*
jobs.c / jobs.h: Estructura de datos de la job table y funciones para agregar, vigilar y 
listar procesos asíncronos en segundo plano
*/

#ifndef jobs_h
#define jobd_h

void add_job(pid_t pid, const char* com);

void list_jobs();

#endif //