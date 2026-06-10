/*
history.c / history.h: Configuración del modo no canónico (raw mode) de la terminal,
lectura carácter por carácter y navegación del historial.
*/

#ifndef history_h
#define history_h

void ActivarModoRaw();
void DesactivarModoRaw();
int LeerLinea(char *buffer, int max_len);

#endif