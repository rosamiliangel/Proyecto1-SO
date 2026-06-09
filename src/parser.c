/*
parser.c / parser.h: Manipulación de cadenas de texto (parse line, tokenización,
detección de operadores y pipes)
*/

#include <string.h>
#include <stdio.h>

#define MAX_ARGS 64  
#define OP_NINGUNO 0
#define OP_SECUENCIAL 1
#define OP_AND 2
#define OP_OR 3

int parse_line(char *line, char **args) {
    int i = 0;
    int operador_detectado = OP_NINGUNO; // Ninguno por defecto
    
    char *token = strtok(line, " \t");

    while (token != NULL && i < MAX_ARGS - 1) {
        // 1. COMPROBACIÓN DE OPERADORES CONDICIONALES / SECUENCIALES
        if (strcmp(token, ";") == 0) {
            operador_detectado = OP_SECUENCIAL;
            break; // Rompemos el ciclo: el comando termina AQUÍ
        } 
        else if (strcmp(token, "&&") == 0) {
            operador_detectado = OP_AND;
            break; // Rompemos el ciclo
        } 
        else if (strcmp(token, "||") == 0) {
            operador_detectado = OP_OR;
            break; // Rompemos el ciclo
        }

        args[i] = token;
        i++;
        token = strtok(NULL, " \t");
    }
    args[i] = NULL;
    
    //Bandera que determina la ejecucion
    int in_background = 0;

    //Revisamos si el ultimo argumento es '&'
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        in_background = 1; // Activamos la bandera
        args[i - 1] = NULL; // Eliminar el operador '&' de los argumentos
    }
    
    return 0; // Cambiar según si se detectó ejecución asíncrona '&'
}