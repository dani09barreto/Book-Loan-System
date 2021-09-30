#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include "book.h"

void createPipe(char *receptorPipe , book bookRequest){

  int fd;
  int create = 0;
  int verify;
  mode_t fifo_mode = S_IRUSR | S_IWUSR;

  do{
    fd = open(receptorPipe,O_RDONLY);
    if(fd==-1){
      perror("[!] Error en la lectura del pipe");
      sleep(10);
    }else
      create = 1;
  }while(create != 1);

  unlink(bookRequest.secondpipe); 

  if (mkfifo (bookRequest.secondpipe, fifo_mode) == -1) {
    perror("[!] Request mkfifo");
    exit(1);
  }  

  write(fd, &bookRequest, sizeof(book));
  create = 0;

  do { 
    if ((fd = open(bookRequest.secondpipe, O_RDONLY)) == -1) {
      perror("[!] Cliente  Abriendo el segundo pipe. Se volvera a intentar ");
      sleep(5);
    } else create = 1; 
  } while (create !=1);
  
  if(read(fd, &create, 10) == -1){
    perror ("[!] No se pudo leer la respuesta del PS receptor");
    exit(0);
  }
  if(create == 1){
    printf("Su peticion fue exitosa ");
  }else{
    printf("Su peticion no fue exitosa");
  }

}
int main(int argc, char *argv[]) {

  if (argc != 5){
    perror("Numero de argumentos invalidos\n");
    printf("ej: ./center –i file –p fileReceptor \n");
    exit (0);
  }
  book bookRequest;

  bookRequest.ISBN = 1234;
  bookRequest.operation = 'P';
  stpcpy(bookRequest.name, "hola");
  stpcpy(bookRequest.secondpipe, "pipepersonal");

  //abre el pipe verde
  createPipe(argv[1],bookRequest);
  //crea el pipe personal y manda la info de la solicitud
  //manda info y espera la respuesta

  return 0;
}