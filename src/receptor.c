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
   printf("Se abre %s para enviar la respuesta a la solicitud\n", fileSecondpipe);
   printf("---------------------\n");
   
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
   write(fd, "1", 10);
}

void returnBook (char *fileSecondpipe, int fd){

   int make = 0;

   printf("Se abre %s para enviar la respuesta a la solicitud\n", fileSecondpipe);
   printf("---------------------\n");
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
   write(fd, "1", 10);
}

void renovateBook (char *fileSecondpipe, int fd){
    
   int make = 0;

   printf("Se abre %s para enviar la respuesta a la solicitud\n", fileSecondpipe);
   printf("---------------------\n");
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
   write(fd, "1", 10);
}

int main (int argc, char *argv[]){

   int  fd, fd1, bytes,create = 0;
   book bookRequest;
   mode_t fifo_mode = S_IRUSR | S_IWUSR;
  
   if (argc != 7){
      perror("Numero de argumentos invalidos\n");
      printf("ej: ./receptor –p nombrepipe –f filedatos –s filesalida\n");
      exit (0);
   }

   printf("Se crea el pipe %s para recibir solicitud\n", argv[2]);
   printf("---------------------\n");
   unlink(argv[2]);
   if (mkfifo (argv[2], fifo_mode) == -1) {
      perror("Receptor mkfifo");
      exit(1);
   }
   
   printf("Se esta abriendo el pipe %s para recibir solicitud\n", argv[2]);
   printf("---------------------\n");
   if ((fd = open(argv[2], O_RDONLY)) == -1){
      perror("Error al abrir el pipe\n");
      exit (0);
   }

   printf("Se abrio %s \n", argv[2]);
   printf("---------------------\n");
   printf("Se esta leyendo la solicitud que manda el PS\n"); 

   bytes = read (fd, &bookRequest, sizeof(bookRequest));
   if (bytes == -1) {
      perror("proceso lector:");
      exit(1);
   }

   printf("Se lee %s %i %c %s en %s\n", bookRequest.name, bookRequest.ISBN, bookRequest.operation, bookRequest.secondpipe, argv[2]);
   printf("---------------------\n");
   
   do { 
      if ((fd1 = open(bookRequest.secondpipe, O_WRONLY)) == -1) {
         perror("Receptor Abriendo el segundo pipe\n");
         printf("Se volvera a intentar despues\n");
         sleep(5);
      } else create = 1; 
   }  while (create == 0);

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
   
   exit(0);
}





