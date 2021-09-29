#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "book.h"

int main(int argc, char *argv[]){

    //codigo del servidor

    //recibe los argumentos

    if (argc != 7){
        perror("Numero de argumentos invalidos\n");
        printf("ej: ./receptor –p nombrepipe –f filedatos –s filesalida\n");
        exit (0);
    }

    int fd, fd1, pid, n, bytes, res, make = 0;
    char *filepipe = argv[2];
    book peticion;
    
    //crea el pipe receptor lo abre
    mode_t fifo_mode = S_IRUSR | S_IWURS;

    unlink(filepipe);
    if (mkfifo(filepipe, fifo_mode) == -1){
        perror("Server mkfifo\n");
        exit (0);
    }

    if (fd = open(filepipe, O_RDONLY) == -1){
        perror("Error al abrir el pipe\n");
        exit (0);
    }
    printf("Se abrio el pipe receptor\n");

    bytes = read (fp, &peticion, sizeof(book));

    if (bytes == -1){
        perror ("Error al leer el pipe");
        exit (0);
    }
    printf("El proceso receptor recibio la solicitud\n");
    switch (peticion.operation){

    case 'P':
        do{
            if ((fd1 = open (peticion.secondpipe, O_WDONLY)) == -1){
                perror("Receptor abriendo el pipe respuesta\n");
                perror("Se intentara mas tarde\n");
                sleep(5);
            }
            else{
                make = 1;
            }
        }while(make == 0);
        printf("Proceso receptor envia respuesta\n");
        write(fd1, 1, sizeof(int));
        break;
    case 'D':
            do{
            if ((fd1 = open (peticion.secondpipe, O_WDONLY)) == -1){
                perror("Receptor abriendo el pipe respuesta\n");
                perror("Se intentara mas tarde\n");
                sleep(5);
            }
            else{
                make = 1;
            }
        }while(make == 0);
        printf("Proceso receptor envia respuesta\n");
        write(fd1, 1, sizeof(int));
        break;
    case 'R':
            do{
            if ((fd1 = open (peticion.secondpipe, O_WDONLY)) == -1){
                perror("Receptor abriendo el pipe respuesta\n");
                perror("Se intentara mas tarde\n");
                sleep(5);
            }
            else{
                make = 1;
            }
        }while(make == 0);
        printf("Proceso receptor envia respuesta\n");
        write(fd1, 1, sizeof(int));
        break;
    default:
        printf("Accion no se puede realizar\n");
        break;
    }

    return 0;
}
