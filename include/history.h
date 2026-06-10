/*
history.c / history.h: Configuración del modo no canónico (raw mode) de la terminal,
lectura carácter por carácter y navegación del historial.
*/

#ifndef history_h
#define history_h

//Activa el modo Raw en la shell
void ActivarModoRaw();

//Apaga el modo Raw y enciende el modo canonico en la shell
void DesactivarModoRaw();

//Leer entrada
int LeerLinea(char *buffer, int max_len);

//Historial
void Historial();

//Agrega un comando a HistorialMem y a .ucvshHistorial
void AgregarComando(const char *buffer);

#endif