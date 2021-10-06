#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "book.h"

char answer [TAMMENSAJE];
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
      token = strtok(NULL, ",");
      if(i == 1){
         strcpy(booksRequests[pos].name, token);
      }
      if(i == 2){
         booksRequests[pos].ISBN = atoi(token);
      }
   }
   sprintf(namePS, "debug/PS#%i", getpid());
   strcpy(booksRequests[pos].secondpipe, namePS);
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

void createRequest (char *namepipe, book bookRequest){

   mode_t fifo_mode = S_IRUSR | S_IWUSR;
   int  fd, pid, create = 0;

   printf("\tSe abre el %s para enviar proceso a realizar\n", namepipe);
   printf("\t---------------------\n");

   do { 
      if ((fd = open(namepipe, O_WRONLY))== -1) {
         perror("\t[!] Error en la lectura del pipe\n");
         printf("\tSe volvera a intentar despues\n");
	      sleep(5);        
      } else create = 1;
   } while (create == 0);

   printf("\tSe crea el pipe %s para recibir la respuesta del receptor\n", bookRequest.secondpipe);
   printf("\t---------------------\n");

   unlink(bookRequest.secondpipe); 
   if (mkfifo (bookRequest.secondpipe, fifo_mode) == -1) {
      perror("\t[!] Request mkfifo");
      exit(1);
   }

   printf("\tSe escribe %s\n", namepipe);
   printf("\tSe escribe\n");
   printf("\tSolicitud: %c\n", bookRequest.operation);
   printf("\tNombre: %s\n", bookRequest.name);
   printf("\tISBN: %d\n", bookRequest.ISBN);
   printf("\tPipe: %s\n", bookRequest.secondpipe);
   printf("\t---------------------\n");
      
   if (write(fd, &bookRequest , sizeof(bookRequest)) == -1){
      perror("");
      printf("\tNo se pudo enviar la solicitud\n");
      exit(0);
   }

   printf("\tSe abre %s para leer la respuesta del receptor\n", bookRequest.secondpipe);
   printf("\t---------------------\n");

   create = 0;
   do { 
      if ((fd = open(bookRequest.secondpipe, O_RDONLY)) == -1) {
         perror("\tCommand Center Abriendo el segundo pipe. Se volvera a intentar\n");
         sleep(5);
      } else create = 1; 
   } while (create == 0);
    
   if (read(fd, answer, 10) == -1){
      perror ("\t[!] No se pudo leer la respuesta del Proceso receptor");
      exit(0);
   }

   if (strcmp(answer, "1") == 0){
      printf("\tSu peticion fue exitosa\n");
   }
   else{
      printf("\tSu peticion no fue exitosa\n");
   }
}

int main (int argc, char *argv[]){


  if (argc != 5){
    perror("\tNumero de argumentos invalidos\n");
    printf("\tej: ./debug/center –i file/PS.txt –p debug/pipeReceptor \n");
    exit (0);
  }
   
   int option;
   int optionMenu;
   book bookRequest;
   printf("\n");
   printf("\t___Welcome to BookTime___\n");
   printf("\t1. Leer el archivo\n");
   printf("\t2. Digitar la informacion del libro\n");
   printf("\tEscoja una opcion: ");
   scanf("\t%d", &option);

   switch (option){

   case 1:
      readFile(argv[2]);
      printf("\tse leyo del archivo %s\n", argv[2]);
      for(int i = 0; i < pos; i++){
         printf("\t%c, %s, %d\n", booksRequests[i].operation, booksRequests[i].name, booksRequests[i].ISBN);
      }
          
      for(int i = 0; i < pos; i++){
         printf("\t---------------------\n");
         printf("\tse crea solicitud\n");
         printf("\tSolicitud: %c\n", booksRequests[i].operation);
         printf("\tNombre: %s\n", booksRequests[i].name);
         printf("\tISBN: %d\n", booksRequests[i].ISBN);
         printf("\tPipe: %s\n", booksRequests[i].secondpipe);
         createRequest(argv[4], booksRequests[i]);
      }
      break;
         case 2: 
            do{
               printf("\n");
               printf("\t_______BookTime______\n");
               printf("\n");
               printf("\t1. Pedir Libro\n");
               printf("\t2. Devolver Libro\n");
               printf("\t3. Renovar Libro\n");
               printf("\t0. Salir\n");
               printf("\tEscoja una opcion: ");
               scanf("\t%i", &optionMenu);

               switch (optionMenu){
               case 1:
                  printf("\n");
                  printf("\tInserte el nombre del libro\n");
                  scanf("%s", bookRequest.name);
                  printf("\tInserte el ISBN del libro\n");
                  scanf("\t%d", &bookRequest.ISBN);
                  bookRequest.operation = 'P';
                  sprintf(namePS, "PS#%i", getpid());
                  strcpy(bookRequest.secondpipe, namePS);
                  printf("\t---------------------\n");
                  printf("\tSe crea solicitud\n");
                  printf("\tSolicitud: %c\n", bookRequest.operation);
                  printf("\tNombre: %s\n", bookRequest.name);
                  printf("\tISBN: %d\n", bookRequest.ISBN);
                  printf("\tPipe: %s\n", bookRequest.secondpipe);
                  createRequest(argv[4], bookRequest);
                  break;
               case 2:
                  printf("\n");
                  printf("\tInserte el nombre del libro\n");
                  scanf("%s", bookRequest.name);
                  printf("\tInserte el ISBN del libro\n");
                  scanf("%d", &bookRequest.ISBN);
                  bookRequest.operation = 'D';
                  sprintf(namePS, "PS#%i", getpid());
                  strcpy(bookRequest.secondpipe, namePS);
                  printf("\t---------------------\n");
                  printf("\tSe crea solicitud\n");
                  printf("\tSolicitud: %c\n", bookRequest.operation);
                  printf("\tNombre: %s\n", bookRequest.name);
                  printf("\tISBN: %d\n", bookRequest.ISBN);
                  printf("\tPipe: %s\n", bookRequest.secondpipe);
                  createRequest(argv[4], bookRequest);
                  break;
               case 3:
                  printf("\n");
                  printf("\tInserte el nombre del libro\n");
                  scanf("%s", bookRequest.name);
                  printf("\tInserte el ISBN del libro\n");
                  scanf("%d", bookRequest.ISBN);
                  bookRequest.operation = 'R';
                  sprintf(namePS, "PS#%i", getpid());
                  strcpy(bookRequest.secondpipe, namePS);
                  printf("\t---------------------\n");
                  printf("\tSe crea solicitud\n");
                  printf("\tSolicitud: %c\n", bookRequest.operation);
                  printf("\tNombre: %s\n", bookRequest.name);
                  printf("\tISBN: %d\n", bookRequest.ISBN);
                  printf("\tPipe: %s\n", bookRequest.secondpipe);
                  createRequest(argv[4], bookRequest);
                  break;
               default:
                  printf("\n");
                  printf("\tOpcion no valida\n");
                  break;
               }
            }while(optionMenu != 0);
         break;
   default:
      break;
   }
   
   printf("\tEl proceso %s termina\n", namePS);
   exit(0);
}