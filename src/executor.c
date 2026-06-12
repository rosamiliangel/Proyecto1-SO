/*
executor.c / executor.h: Interacción con el Kernel (resolución de la variable $PATH,
llamadas a fork(), execvp(), waitpid() y redirección de descriptores)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include "../include/jobs.h"

//Buscar binarios
int search_in_path(const char *cmd, char *full_path) {
    
    if (cmd[0] == '/' || cmd[0] == '.') {
        // Si el comando ya es una ruta absoluta o relativa, verificamos
        if (access(cmd, X_OK) == 0) {strcpy(full_path, cmd); return 1;}
        return 0; // Comando no encontrado
    }

    //Obtener la variable de entorno PATH
    char *path_env = getenv("PATH");
    if (path_env == NULL) {return -1;} //Si el path es null retornar

    //Clonar el PATH
    char path_local[1024];
    strncpy(path_local, path_env, sizeof(path_local) - 1);

    //Puntero para operadores
    char *separador;

    //Separar los directorios usando el delimitador ':'
    char *directory = strtok_r(path_local, ":", &separador);
    while (directory != NULL) {
        char ruta_alterna[1024];
        //Concatenar el directorio con el comando
        snprintf(ruta_alterna, sizeof(ruta_alterna), "%s/%s", directory, cmd);
        //Comprobar si el comando existe y permisos de ejecución
        if (access(ruta_alterna, X_OK) == 0) {
            //Copiar la ruta completa al buffer full_path
            strcpy(full_path, ruta_alterna);
            return 1;
        }
        //Siguiente directorio
        directory = strtok_r(NULL, ":", &separador);
    }

    //Retornar si no se encontro el comando
    return 0;
}

// Revisar si en los argumentos viene un '>' para redirigir
void redireccion(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            // El nombre del archivo es el siguiente argumento
            char *archivo = args[i + 1];

            // Abrimos el archivo: Escritura, Crear si no existe, Truncar si ya existe
            int fd = open(archivo, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("ucvsh: error al abrir archivo de redirección");
                exit(EXIT_FAILURE);
            }

            // Clonamos el descriptor sobre la salida estándar (stdout)
            dup2(fd, STDOUT_FILENO);
            close(fd); // Cerramos el descriptor original

            // Limpiar el '>' y el nombre del archivo para que execv no intente leerlos
            args[i] = NULL;
            break; 
        }
    }
}

//Ejecutar comando
int launch_external_command(char **args, int in_background) {
    char ruta_comando[1024];

    //Verificar si el comando existe en el PATH
    if (!search_in_path(args[0], ruta_comando)) {return -1;
    } 
    // Aislar la ejecución:
    pid_t pid = fork();
    if (pid < 0) {perror("Error al crear hilo"); return -1;} //salir si fork falla
    
    if (pid == 0) {
        // Proceso hijo
        signal(SIGINT, SIG_DFL); //Restaurar el comportamiento por defecto de Ctrl+C en el hijo
        signal(SIGTSTP, SIG_DFL); //Restaurar el comportamiento por defecto de Ctrl+Z en el hijo

        // Redirección de salida si es necesario
        redireccion(args);

        //Remplazar la imegn en memoria del proceso por el nuevo binario (execvp)
        //Proceso hijo
        if (execvp(ruta_comando, args) == -1) {
            perror("ucvsh error de ejecucion"); 
            exit(EXIT_FAILURE); //Cerrar el hijo si execvp falla
        }
    } else {
        // ------ PROCESO PADRE ------
        if (!in_background) {
            //Antes del bloqueo
            signal(SIGINT, SIG_IGN); //Ignorar Ctrl+C en el padre mientras espera al hijo
            // Bloqueamos y ceder el control de la terminal al hijo
            int status;
            // Detener al padre de forma segura hasta que el PID del hijo específico notifique su estado
            waitpid(pid, &status, 0);

            // Restaurar el comportamiento por defecto de Ctrl+C en el padre
            signal(SIGINT, SIG_DFL);

            if (WIFEXITED(status)) {
                return WEXITSTATUS(status); // Retorna el código de salida real
            }
        return -1;
                 
        } else {
            // EJECUCIÓN ASÍNCRONA (Background) 
            add_job(pid, args[0]);
            printf("[%d] %d lanzado en segundo plano\n", job_count, pid);
            return 0;
        }
    }
    return -1;
}

//Ejecuta dos comandos interconectados por una tubería
void ejecutar_pipe(char **args_izq, char **args_der) {
    int fd[2];
    pid_t pid1, pid2;

    // Crear la tubería en el Kernel
    if (pipe(fd) < 0) {
        perror("ucvsh: error al crear el pipe");
        return;
    }

    // Primer Fork: Crear el Hijo Izquierdo
    pid1 = fork();
    if (pid1 == 0) {
        // Redirigir la Salida Estándar al extremo de escritura del pipe
        dup2(fd[1], STDOUT_FILENO);
        
        //El hijo izquierdo no lee del tubo, cierra lectura
        close(fd[0]);
        // Cerrar el descriptor original ya duplicado
        close(fd[1]);

        // Buscar la ruta ejecutable
        char ruta[1024];
        if (search_in_path(args_izq[0], ruta)) {
            execv(ruta, args_izq);
        }
        perror("ucvsh: error en comando izquierdo");
        exit(EXIT_FAILURE);
    }

    // Segundo Fork: Crear el Hijo Derecho
    pid2 = fork();
    if (pid2 == 0) {
        // Redirigir la Entrada Estándar al extremo de lectura del pipe
        dup2(fd[0], STDIN_FILENO);
        
        // El hijo derecho no escribe en el tubo, cierra escritura
        close(fd[1]);
        close(fd[0]);

        redireccion(args_der); // Redirección de salida

        char ruta[1024];
        if (search_in_path(args_der[0], ruta)) {
            execv(ruta, args_der);
        }
        perror("ucvsh: error en comando derecho");
        exit(EXIT_FAILURE);
    }

    // El padre no lee ni escribe en este tubo. Si no cierra AMBOS 
    // descriptores aquí, los hijos se quedarán colgados esperando eternamente.
    close(fd[0]);
    close(fd[1]);

    // Esperamos síncronamente que ambos hijos terminen
    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
}