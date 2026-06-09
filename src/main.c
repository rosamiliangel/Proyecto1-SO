#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <termios.h>
#include "../include/parser.h"
#include "../include/jobs.h"
#include "../include/builtins.h"
#include "../include/executor.h"


#define MAX_LINE_LEN 1024  // Longitud máxima de una línea de comandos leída del usuario [cite: 9]
#define MAX_ARGS 64

//Colocados provisionalmente solo para evitar problemas con parser.c
#define OP_NINGUNO 0
#define OP_SECUENCIAL 1
#define OP_AND 2
#define OP_OR 3

int main() {
    char linea[MAX_LINE_LEN];
    char *args[MAX_ARGS];

    //Ciclo REPL (Read-Evaluate-Print-Loop)
    while(1) {
        printf("ucvsh> ");
        fflush(stdout); //Limpiar el buffer para asegurar que el prompt se vea
         
        if (fgets(linea, sizeof(linea), stdin) == NULL) {break;}

        //Limpiar salto de linea de fgets
        linea[strcspn(linea, "\n")] = '\0';

        //Caso de cadena vacia
        if(strlen(linea) == 0){continue;}

        char *PunteroLinea = linea; //Puntero para rastrear cortes a linea
        int UltimoEstado = 0; //Almacena el estado de la ultima ejecucion/hijo
        int operador_detectado = OP_NINGUNO; //Almacenar operador

        while(1){

            int in_background = 0; //Bandera de operador

            //Almacenar operador y actualizar PunteroLinea
            operador_detectado = parse_line(PunteroLinea, args, &in_background);
            PunteroLinea = NULL; //Fuerza a strtok de parse_line a usar el resto de la cadena

            if(args[0] == NULL) {break;} //Caso de operador sin comandos

            //Revisar BultIn
            if (RevisarBuiltIn(args, &UltimoEstado) == 0){
                //Como el comando no era BuiltIn se ejecuta
                //Se cambio launch de void a int en executor para capturar el valor
                UltimoEstado = launch_external_command(args, in_background);
            }
            
            //Revisar operadores
            // Sin operadores
            if (operador_detectado == OP_NINGUNO) {break;}
            // Operador ";". Se ejecuta el siguiente comando
            if (operador_detectado == OP_SECUENCIAL) { continue; }
            // Operador &&. Se ejecuta el siguiente comando si el anterior retorno 0 (exito)
            if (operador_detectado == OP_AND && UltimoEstado != 0) { break; }
            // Operador ||. Se ejecuta el siguiente comando si el anterior retorno 1)
            if (operador_detectado == OP_OR && UltimoEstado == 0) { break; }

        }

        //int background_flag = parse_line(linea, args);
        //if (UltimoEstado == 0) {launch_external_command(args, background_flag);}
        
    }

    return 0;

}