#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "book.h"

char request [TAMMENSAJE];
int numPS = 0;
char namePS [TAMMENSAJE];
book booksRequests [MAXBOOK];
int pos = 0;

void tokLine(char *line){

   char *token;
   int i = 0;

   token = strtok(line, ",");
   

   while(token){
      if(i == 0){
         booksRequests[pos].operation  = token[0];
      }
      i++;
      printf("%s\n", token);
      token = strtok(NULL, ",");
      if(i == 1){
         strcpy(booksRequests[pos].name, token);
      }
      if(i == 2){
         booksRequests[pos].ISBN = atoi(token);
      }
   }
   pos ++;
}

void readFile(char *nameFile){

   FILE *fd;
   char line [MAXLINE];

   fd = fopen(nameFile, "r");

   while(fgets(line, sizeof(line), fd)){
      tokLine(line);
   } 

}

void createpipe (char *namepipe, book bookRequest){

   mode_t fifo_mode = S_IRUSR | S_IWUSR;
   int  fd, pid, create = 0;

   printf("Se abre el %s para enviar proceso a realizar\n", namepipe);
   printf("---------------------\n");

   do { 
      if ((fd = open(namepipe, O_WRONLY))== -1) {
         perror("[!] Error en la lectura del pipe\n");
         printf(" Se volvera a intentar despues\n");
	      sleep(5);        
      } else create = 1;
   } while (create == 0);

   printf("Se crea el pipe %s para recibir la respuesta del receptor\n", bookRequest.secondpipe);
   printf("---------------------\n");

   unlink(bookRequest.secondpipe); 
   if (mkfifo (bookRequest.secondpipe, fifo_mode) == -1) {
      perror("[!] Request mkfifo");
      exit(1);
   }

   printf("Se escribe %s %i %c %s en %s\n", bookRequest.name, bookRequest.ISBN, bookRequest.operation, bookRequest.secondpipe, namepipe);
   printf("---------------------\n");
      
   if (write(fd, &bookRequest , sizeof(bookRequest)) == -1){
      perror("");
      printf("No se pudo enviar la solicitud\n");
      exit(0);
   }

   printf("Se abre %s para leer la respuesta del receptor\n", bookRequest.secondpipe);
   printf("---------------------\n");

   create = 0;
   do { 
      if ((fd = open(bookRequest.secondpipe, O_RDONLY)) == -1) {
         perror("Command Center Abriendo el segundo pipe. Se volvera a intentar\n");
         sleep(5);
      } else create = 1; 
   } while (create == 0);
    
   if (read(fd, request, 10) == -1){
      perror ("[!] No se pudo leer la respuesta del PS receptor");
      exit(0);
   }

   printf("El proceso %s termina\n", namePS);

   if (strcmp(request, "1") == 0){
      printf("Su peticion fue exitosa\n");
   }
   else{
      printf("Su peticion no fue exitosa\n");
   }
}

int main (int argc, char *argv[]){


  if (argc != 5){
    perror("Numero de argumentos invalidos\n");
    printf("ej: ./center –i file –p fileReceptor \n");
    exit (0);
  }
   
   int opcion;

   printf("    Welcome to BookTime\n");
   printf("1. leer el archivo\n");
   printf("2. digitar la informacion del libro\n");
   printf("Escoja una opcion: ");
   scanf("%d", &opcion);


   
   switch (opcion)
   {
   case 1:
      readFile(argv[2]);
      printf("se leyo del archivo %s\n", argv[2]);
      for(int i = 0; i < pos; i++){
         printf(" %c, %s, %d\n", booksRequests[i].operation, booksRequests[i].name, booksRequests[i].ISBN);
      }
          
      for(int i = 0; i < pos; i++){
         printf("se crea solicitud\n");
         printf(" %c, %s, %d\n", booksRequests[i].operation, booksRequests[i].name, booksRequests[i].ISBN); 
         createpipe(argv[4], booksRequests[i]);
      }
      break;
         case 2: 
         printf("digite la operacion valida");
         break;
   default:
      break;
   }
   

   
   /*
   for(int i = 0; i < pos; i++){
      printf(" %c, %s, %d\n", booksRequests[i].operation, booksRequests[i].name, booksRequests[i].ISBN);
   }
   */
   /*
   for(int i = 0; i < pos; i++){
      createpipe(argv[4], booksRequests[i]);
   }
   */
   //LLenado de la instancia del libro
   //se debe hacer esto cada vez que se quiera crear una nueva solicitud y aumentar numPS
   //ya que lleva el num de proceso ej. PS1 PS2 y asi
   

   

   exit(0);
}