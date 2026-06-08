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
    
    //Bandera que determina la ejecucion
    int in_background = 0;

    //Revisamos si el ultimo argumento es '&'
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        in_background = 1; // Activamos la bandera
        args[i - 1] = NULL; // Eliminar el operador '&' de los argumentos
    }
    
    // DETECCIÓN DE OPERADORES:
    // Durante el recorrido, si encuentras ';', '&&', '||' o '|', debes romper la estructura 
    // para procesar las órdenes de forma secuencial, condicional o mediante canales.
    
    return 0; // Cambiar según si se detectó ejecución asíncrona '&'
}