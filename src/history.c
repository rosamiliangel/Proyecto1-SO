/*
history.c / history.h: Configuración del modo no canónico (raw mode) de la terminal,
lectura carácter por carácter y navegación del historial.
*/

//Incluir todas las librerias mientras se implementa la logica
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

//Struct para almacenar el estado original de la shell
struct termios EstadoOriginal;

//Activa el modo Raw en la shell
void ActivarModoRaw() {
    tcgetattr(STDIN_FILENO, &EstadoOriginal);
    atexit(DesactivarModoRaw); // restaurar al cerrar la shell

    struct termios raw = EstadoOriginal;
    raw.c_lflag &= ~(ICANON | ECHO); // Apagar modo canónico y eco
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

//Apaga el modo Raw y enciende el modo canonico en la shell
void DesactivarModoRaw() {tcsetattr(STDIN_FILENO, TCSAFLUSH, &EstadoOriginal);}

//
int LeerLinea(char *buffer, int max_len) {
    int pos = 0;
    char c;
    buffer[0] = '\0';

    ActivarModoRaw();

    //Leer caracter a caracter
    while (read(STDIN_FILENO, &c, 1) == 1) {
        
        //Caso de que se precione entre, se cierra la cadena
        if (c == '\n') {printf("\n"); buffer[pos] = '\0'; break;} 
        
        //Caso donde se precione del (ascii 127)
        //Visualmente: Mover cursor atrás, imprimir espacio en blanco, mover atrás
        if (c == 127 && pos > 0) {pos--; printf("\b \b"); fflush(stdout);} 
        
        // 3. SECUENCIAS DE ESCAPE (Flechas direccionales)
        if (c == '\033') { 
            char seq[3];
            // Si el escape no viene seguido de '[' y una letra, lo ignoramos
            if (read(STDIN_FILENO, &seq[0], 1) == 0) continue;
            if (read(STDIN_FILENO, &seq[1], 1) == 0) continue;

            if (seq[0] == '[') {
                if (seq[1] == 'A') {
                    // TODO: Programar Flecha Arriba
                    printf("\n[FLECHA ARRIBA DETECTADA - Falta logica]\n");
                    break;
                } else if (seq[1] == 'B') {
                    // TODO: Programar Flecha Abajo
                    printf("\n[FLECHA ABAJO DETECTADA - Falta logica]\n");
                    break;
                }
            }
        } 
        
        // Resto de cracateres
        // printf("%c", c) -> Imprimir el carácter (ECHO está apagado)
        // fflush(stdout) --> Forzar el dibujado
        if (pos < max_len - 1) {buffer[pos++] = c; printf("%c", c); fflush(stdout);}
    }
    
    DesactivarModoRaw();
    
    // Si pos es 0 entonces la entrada era vacia (\n)
    return pos; 
}