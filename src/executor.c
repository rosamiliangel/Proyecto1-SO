/*
executor.c / executor.h: Interacción con el Kernel (resolución de la variable $PATH,
llamadas a fork(), execvp(), waitpid() y redirección de descriptores)
*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int search_in_path(const char *cmd, char *full_path) {
    //Complementario
    if (cmd[0] == '/' || cmd[0] == '.') {
        // Si el comando ya es una ruta absoluta o relativa, verificamos
        if (access(cmd, X_OK) == 0) {
            strcpy(full_path, cmd);
            return 1; // Comando encontrado
        }
        return 0; // Comando no encontrado
    }

    //Obtener la variable de entorno PATH
    char *path_env = getenv("PATH");
    //El enunciado dice que de manera implicita el shell debe tener cargado como valor por
    //defecto las rutas normales y estandares de Linux
    if (path_env == NULL) {
        return -1; //No puede ser esto, hay que poner el estandar
    }

    //Clonamos la cadena de PATH para no modificar la original con strtok
    char path_local[1024];
    strncpy(path_local, path_env, sizeof(path_local) - 1);

    //Separar los directorios usando el delimitador ':'
    char *directory = strtok(path_local, ":");
    while (directory != NULL) {
        char ruta_alterna[1024];
        //Concatenar el directorio con el comando
        snprintf(ruta_alterna, sizeof(ruta_alterna), "%s/%s", directory, cmd);
        //Comprobar si el comando existe y permisos de ejecución
        if (access(ruta_alterna, X_OK) == 0) {
            //Si lo encontramos, copiamos la ruta completa al buffer full_path
            strcpy(full_path, ruta_alterna);
            return 1;
        }
        //Siguiente directorio
        directory = strtok(NULL, ":"); //Falta provar si evita un ciclo infinito o arruina el codigo (:)
;    }

    //Si ya recorrimos todo el PATH y no encontramos el comando, fin.
    return 0;}

void launch_external_command(char **args, int in_background) {
    char ruta_comando[1024];

    //Verificar si el comando existe en el PATH
    if (!search_in_path(args[0], ruta_comando)) {
        return;
    }
    
    // Una vez resuelta la ruta válida, procedemos a aislar la ejecución:
    pid_t pid = fork();
    if (pid < 0) {perror("Error al crear hilo");}
    
    if (pid == 0) {
        
        // MUTACIÓN DE MEMORIA:
        // execvp reemplaza por completo la imagen de memoria del proceso actual por el nuevo binario.
        // Si tiene éxito, esta línea NUNCA retorna; el código del hijo termina ahí.
        if (execvp(ruta_comando, args) == -1) {
            perror("ucvsh error de ejecucion");
            exit(EXIT_FAILURE); // Si execvp falla (ej: comando no encontrado), forzar la muerte del hijo [cite: 34]
        }
    } else if (pid > 0) {
        /* ------------------------------------------------------------------
         * PROCESO PADRE
         * ------------------------------------------------------------------
         */
        if (!in_background) {
            // Bloqueamos temporalmente la shell cediendo el control de la terminal al hijo[cite: 35, 36].
            int status;
            // waitpid detiene al padre de forma segura hasta que el PID del hijo específico notifique su estado[cite: 36].
            waitpid(pid, &status, 0);
            
            // Aquí puedes evaluar las macros WIFEXITED(status) y WEXITSTATUS(status) 
            // para saber si el hijo terminó con código 0 o error, vital para los operadores && y ||[cite: 23, 24].
            
        } else {
            // EJECUCIÓN ASÍNCRONA (Background) 
            add_job(pid, args[0]);
        }
    }else {
        // Si fork() retorna un valor negativo, el sistema operativo se quedó sin recursos para crear el proceso.
        perror("Error crítico en la llamada fork");
    }
}