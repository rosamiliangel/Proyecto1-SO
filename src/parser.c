/*
parser.c / parser.h: Manipulación de cadenas de texto (parse line, tokenización,
detección de operadores y pipes)
*/

#include <string.h>
#include <stdio.h>

#define MAX_ARGS 64  

int parse_line(char *line, char **args) {
    int i = 0;
    
    char *token = strtok(line, " \t");

    while (token != NULL && i < MAX_ARGS - 1) {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t");
    }
    args[i] = NULL;
    // REQUERIMIENTO SINTÁCTICO:
    // args[0] será siempre el comando principal (ej: "gcc")
    // args[1] ... args[n] serán los argumentos del comando
    // args[n+1] DEBE ser NULL para que funciones como execvp sepan dónde termina el arreglo.
    
    // DETECCIÓN DE OPERADORES:
    // Durante el recorrido, si encuentras ';', '&&', '||' o '|', debes romper la estructura 
    // para procesar las órdenes de forma secuencial, condicional o mediante canales.
    
    return 0; // Cambiar según si se detectó ejecución asíncrona '&'
}