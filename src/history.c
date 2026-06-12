/*
history.c / history.h: Configuración del modo no canónico (raw mode) de la terminal,
lectura carácter por carácter y navegación del historial.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include "../include/history.h"

#define MaxComandosHistorial 100
#define MaxTamanoLinea 1024

//Struct para almacenar el estado original de la shell
struct termios EstadoOriginal;

//Variables globales
char HistorialMem[MaxComandosHistorial][MaxTamanoLinea];    // Almacenar historial en memoria
int TotalHistorial = 0;                                     // Contador de comandos almacenados
int PosicionCursor = 0;                                     // Posición del cursor al usar flechas
char RutaHistorial[1024];                                   // Ruta de .ucvsh_history

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

//Leer entrada
int LeerLinea(char *buffer, int max_len) {
    int pos = 0;
    char c;
    buffer[0] = '\0';

    //Igualar el cursor al final del historial
    PosicionCursor = TotalHistorial;

    ActivarModoRaw();

    //Leer caracter a caracter
    while (read(STDIN_FILENO, &c, 1) == 1) {
        
        //Caso de que se precione enter, se cierra la cadena
        if (c == '\n') {
            printf("\n");
            buffer[pos] = '\0';
            if (pos > 0) {AgregarComando(buffer);}
            break;
        } 
        
        //Caso donde se precione Del (ascii 127)
        //Visualmente: Mover cursor atrás, imprimir espacio en blanco, mover atrás
        if (c == 127 && pos > 0) {pos--; printf("\b \b"); fflush(stdout);} 
        
        // Flechas direccionales
        if (c == '\033') { 
            char seq[3];
            // Si el escape no viene seguido de '[' y una letra, lo ignoramos
            if (read(STDIN_FILENO, &seq[0], 1) == 0) {continue;}
            if (read(STDIN_FILENO, &seq[1], 1) == 0) {continue;}

            if (seq[0] == '[') {

                //Flecha Arriba
                if (seq[1] == 'A') {
                    if (PosicionCursor > 0) {
                        PosicionCursor--;
                        strcpy(buffer, HistorialMem[PosicionCursor]);
                        pos = strlen(buffer);
                        // \33[2K borra toda la línea actual y \r mueve el cursor al inicio
                        printf("\33[2K\rucvsh> %s", buffer);
                        fflush(stdout);
                    }
                    continue;
                }
                
                // Flecha Abajo
                if (seq[1] == 'B') {
                    if (PosicionCursor < TotalHistorial - 1) {
                        PosicionCursor++;
                        strcpy(buffer, HistorialMem[PosicionCursor]);
                        pos = strlen(buffer);
                        printf("\33[2K\rucvsh> %s", buffer);
                        fflush(stdout);
                    } else if (PosicionCursor == TotalHistorial - 1) {
                        //Si esta al final del historial se limpia la linea
                        PosicionCursor++;
                        buffer[0] = '\0';
                        pos = 0;
                        printf("\33[2K\rucvsh> ");
                        fflush(stdout);
                    }
                    continue;
                }

                // Ignorar las flechas izquierda y derecha
                if (seq[1] == 'C' || seq[1] == 'D'){continue;}
            }
        } 
        
        // Demas caracteres ascii del 32 al 126
        if (c >= 32 && c <= 126 && pos < max_len - 1) {
            buffer[pos++] = c; 
            printf("%c", c); 
            fflush(stdout);
        }
    }
    
    DesactivarModoRaw();
    
    // Si pos es 0 entonces la entrada era vacia (\n)
    return pos; 
}

void Historial(){
    //Obtener ruta de .ucvshHistorial
    char *DirBase = getenv("HOME"); //Obtener el directorio base de la cuenta actual
    
    if (DirBase != NULL) {
        //Concatenar la ruta con el nombre del archivo
        snprintf(RutaHistorial, sizeof(RutaHistorial), "%s/.ucvshHistorial", DirBase);
        //Crear ucvshHistorial en la carpeta base del proyecto en lugar de /home/usuario
        //snprintf(RutaHistorial, sizeof(RutaHistorial), "./.ucvshHistorial");

        //Abrir archivo en modo lectura
        FILE *lectura = fopen(RutaHistorial, "r");

        if (lectura != NULL) {
            //Arreglo para almacenar un comando 
            char LineaTemp[MaxTamanoLinea];
            while (fgets(LineaTemp, sizeof(LineaTemp), lectura) != NULL && TotalHistorial < MaxComandosHistorial) {
                //Limpiar salto de linea de fgets
                LineaTemp[strcspn(LineaTemp, "\n")] = '\0';
                //Copiar conteniddo de LineaTemp a HistorialMem
                strcpy(HistorialMem[TotalHistorial], LineaTemp);
                TotalHistorial++;
            }
            //Cerrar archivo
            fclose(lectura);
        }
    }
}

//Agrega un comando a HistorialMem y a .ucvshHistorial
void AgregarComando(const char *buffer) {
    if (strlen(buffer) == 0) {return;}

    //Almacenar en HistorialMem
    if (TotalHistorial < MaxComandosHistorial) {
        strcpy(HistorialMem[TotalHistorial], buffer); 
        TotalHistorial++;
    } else {
        // Si HistorialMem se llena se desplazan los comando hacia arriba perdiendo el primero
        for(int i = 1; i < MaxComandosHistorial; i++){strcpy(HistorialMem[i-1], HistorialMem[i]);}
        strcpy(HistorialMem[MaxComandosHistorial-1], buffer);
    }

    //Almacenar en .ucvshHistorial
    FILE *escribir = fopen(RutaHistorial, "a");
    if (escribir != NULL) {fprintf(escribir, "%s\n", buffer); fclose(escribir);}
}

