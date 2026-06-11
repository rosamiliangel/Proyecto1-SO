/*
builtins.c / builtins.h: Funciones aisladas para los built-ins (cd, exit y lectura/escritura
del historial)
*/

#ifndef builtins_h
#define builtins_h

//Revisar si el comando es un BuiltIn
int RevisarBuiltIn(char **args, int *UltimoEstado);

#endif