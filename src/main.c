#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <termios.h>
#include <signal.h>
#include "../include/parser.h"
#include "../include/jobs.h"
#include "../include/builtins.h"
#include "../include/executor.h"
#include "../include/history.h"


#define MAX_LINE_LEN 1024  // Longitud máxima de una línea de comandos leída del usuario [cite: 9]
#define MAX_ARGS 64

//Colocados provisionalmente solo para evitar problemas con parser.c
#define OP_NINGUNO 0
#define OP_SECUENCIAL 1
#define OP_AND 2
#define OP_OR 3

void signal_handler(int sig) {
    // Manejar señales como SIGINT (Ctrl+C) para no cerrar la shell
    if (sig == SIGINT) {
        printf("\nucvsh> ");
        fflush(stdout);
    }
    // Manejar SIGTSTP (Ctrl+Z) para suspender el proceso en primer plano
    if (sig == SIGTSTP) {
        printf("\nucvsh> ");
        fflush(stdout);
    }
}

int main() {
    // Configurar el manejador de señales para SIGINT
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);

    char linea[MAX_LINE_LEN];
    char *args[MAX_ARGS];

    // Cargar .ucvshHistorial a HistorialMem (memoria)
    Historial();

    //Ciclo REPL (Read-Evaluate-Print-Loop)
    while(1) {
        //Limpieza de zombies
        check_jobs();

        //Variables
        char *PunteroLinea = linea; //Puntero para rastrear cortes a linea
        int UltimoEstado = 0; //Almacena el estado de la ultima ejecucion/hijo
        int operador_detectado = OP_NINGUNO; //Almacenar operador

        //Prompt inicial y limpieza del buffer para asegurar visibilidad del prompt
        printf("ucvsh> ");
        fflush(stdout);
        /*
        //Anterior lectura de la entrada

        if (fgets(linea, sizeof(linea), stdin) == NULL) {break;}

        //Limpiar salto de linea de fgets
        linea[strcspn(linea, "\n")] = '\0';

        //Caso de cadena vacia
        if(strlen(linea) == 0){continue;}
        */

        //Lectura con historial
        //Si retorna 0 la entrada es vacia (\n)
        if (LeerLinea(linea, sizeof(linea)) == 0) {continue;}

        //Se agrego strstr(linea, "||") == NULL para que en revisar operadores || no pase por |
        if (strchr(linea, '|') != NULL && strstr(linea, "||") == NULL) {
            // Si la línea tiene un pipe, extraemos los comandos izquierdo y derecho
            char *args_izq[MAX_ARGS];
            char *args_der[MAX_ARGS];
            
            // Se pica la línea en dos partes usando el '|' como delimitador
            char *parte_izq = strtok(linea, "|");
            char *parte_der = strtok(NULL, ""); // El resto de la línea
            
            if (parte_izq != NULL && parte_der != NULL) {
                //Banderas para cada trozo
                int in_background_izq = 0;
                int in_background_der = 0;

                // Parseamos cada trozo de manera independiente usando el parse_line
                parse_line(parte_izq, args_izq, &in_background_izq);
                parse_line(parte_der, args_der, &in_background_der);
                
                if (args_izq[0] != NULL && args_der[0] != NULL) {
                    // Invocamos a la función de tuberías que se agrego
                    ejecutar_pipe(args_izq, args_der);
                }
            }
            // Saltamos el resto del bucle para volver a pedir otro prompt ucvsh>
            continue; 
        }

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
            if (operador_detectado == OP_SECUENCIAL) {continue;}
            // Operador &&. Se ejecuta el siguiente comando si el anterior retorno 0 (exito)
            if (operador_detectado == OP_AND && UltimoEstado != 0) {break;}
            // Operador ||. Se ejecuta el siguiente comando si el anterior retorno 1)
            if (operador_detectado == OP_OR && UltimoEstado == 0) {break;}

        }

        //int background_flag = parse_line(linea, args);
        //if (UltimoEstado == 0) {launch_external_command(args, background_flag);}
        
    }

    return 0;
    
}