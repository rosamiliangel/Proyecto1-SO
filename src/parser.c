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

int parse_line(char *line, char **args, int *in_background) {
    int i = 0;
    int operador_detectado = OP_NINGUNO; // Ninguno por defecto

    //Bandera que determina la ejecucion
    *in_background = 0; //Se inicializa en 0 para cada nueva lectura

    //Puntero estatico para almacenar la ubicación del operador en la cadena
    static char *operador;

    //Pimer corte de la cadena
    char *token = strtok_r(line, " \t\n\r", &operador);

    while (token != NULL && i < MAX_ARGS -1) {
        //Comprobación de operadores
        if(strcmp(token, ";") == 0) {operador_detectado = OP_SECUENCIAL; break;}
        if(strcmp(token, "&&") == 0) {operador_detectado = OP_AND; break;}
        if(strcmp(token, "||") == 0) {operador_detectado = OP_OR; break;}
        /* Se trata al & como un operador en casos borde de que el comando asincrono no sea
        el unico comando o no este al final de varios comandos.
        Revisar que el ultimo argumento es & -> ejecución asincrona.
        if(i > 0 && strcmp(args[i - 1], "&") == 0) {*in_background = 1; args[i - 1] = NULL;}
        se remplaza. */
        if(strcmp(token, "&") == 0){*in_background = 1;operador_detectado = OP_SECUENCIAL; break;}

        // Si la parte de la cadena evaluada no es operador se almacena en args
        args[i] = token;
        i++;

        // Se continua cortando la cadena
        token = strtok_r(NULL, " \t\n\r", &operador);
    }

    //Asegurar que args termine en NULL (importante para usar parse_line repetidimente
    //con una cadena)
    args[i] = NULL;

    return operador_detectado; // cambio para conservar el operador
}