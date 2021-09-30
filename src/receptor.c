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


void requestBook (char *fileSecondpipe, int fd){
    int make = 0;
    do{
        if ((fd = open (fileSecondpipe, O_WRONLY)) == -1){
            perror("Receptor abriendo el pipe respuesta\n");
            perror("Se intentara mas tarde\n");
            sleep(5);
        }
        else{
            make = 1;
        }
    }while(make == 0);
    printf("Proceso receptor envia respuesta a pipe del proceso solicitud\n");
    write(fd, 1, sizeof(int));
    close (fd);
}

void returnBook (char *fileSecondpipe, int fd){

    int make = 0;
    do{
        if ((fd = open (fileSecondpipe, O_WRONLY)) == -1){
            perror("Receptor abriendo el pipe respuesta\n");
            perror("Se intentara mas tarde\n");
            sleep(5);
        }
        else{
            make = 1;
        }
    }while(make == 0);
    printf("Proceso receptor envia respuesta a pipe del proceso solicitud\n");
    write(fd, 1, sizeof(int));
    close (fd);
}

void renovateBook (char *fileSecondpipe, int fd){
    int make = 0;
    do{
        if ((fd = open (fileSecondpipe, O_WRONLY)) == -1){
            perror("Receptor abriendo el pipe respuesta\n");
            perror("Se intentara mas tarde\n");
            sleep(5);
        }
        else{
            make = 1;
        }
    }while(make == 0);
    printf("Proceso receptor envia respuesta a pipe del proceso solicitud\n");
    write(fd, 1, sizeof(int));
    close (fd);
}

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
    book bookRequest;
    
    //crea el pipe receptor y lo abre
    mode_t fifo_mode = S_IRUSR | S_IWUSR;

    unlink(filepipe);
    if (mkfifo(filepipe, fifo_mode) == -1){
        perror("Receptor mkfifo\n");
        exit (0);
    }

    if (fd = open(filepipe, O_RDONLY) == -1){
        perror("Error al abrir el pipe\n");
        exit (0);
    }
    printf("Se abrio el pipe receptor\n");

    // lee lo que commandCenter le manda y lo guarda en request
    bytes = read (fd, &bookRequest, sizeof(book));

    if (bytes == -1){
        perror ("Error al leer el pipe");
        exit (0);
    }
    printf("El proceso receptor recibio la solicitud\n");

    //Identifica cual es el proceso a realizar D: Devolver, P: Pedir, R: Renovar

    switch (bookRequest.operation){

    case 'P':
        requestBook(&bookRequest.secondpipe, fd1);
        break;
    case 'D':
        returnBook(&bookRequest.secondpipe, fd1);
        break;
    case 'R':
        returnBook(&bookRequest.secondpipe, fd1);
        break;
    default:
        printf("Accion no se puede realizar\n");
        break;
    }

    printf("receptor a termindado");

    return 0;
}
