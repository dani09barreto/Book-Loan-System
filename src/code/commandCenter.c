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


/*
Name : tokLine
Parameters : char* line
Function : Esta funcion tokeniza la linea que recibe por parametro y la almacena en un arreglo de
libro. Almacena los datos de la informacion del libro que recibe:
Return value : void
*/

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
   strcpy(booksRequests[pos].secondpipe, namePS);
   pos ++;
}

/*
Name : readFile
Parameters : char* nameFile
Function : Esta funcion lee el archivo de las solicitudes y cada linea que lee la envia a la funcion
tokLine para poder guardar esa informacion en bookRequests.
Return value : void
*/

void readFile(char *nameFile){

   FILE *fd;
   char line [MAXLINE];

   fd = fopen(nameFile, "r");

   while(fgets(line, sizeof(line), fd)){
      tokLine(line);
   } 

}


/*
Name : createRequest
Parameters : char* namepipe, book bookRequest, int fd
Function : Esta funcioln crea las solicitudes en el pipe y espera por que el receptor analice la 
oeticion y le informe si es posible realizar la solicitud cargada.
Return value : void
*/

void createRequest (char *namepipe, book bookRequest, int fd){



   int fd1, pid, create = 0;
   printf("\t---------------------------\n");
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
   sleep (3);
   printf("\tSe abre %s para leer la respuesta del receptor\n", namePS);
   printf("\t---------------------\n");

   create = 0;
   do { 
      if ((fd1 = open(namePS, O_RDONLY)) == -1) {
         perror("\tCommand Center Abriendo el segundo pipe. Se volvera a intentar\n");
         sleep(5);
      } else create = 1; 
   } while (create == 0);
    
   if (read(fd1, answer, 10) == -1){
      perror ("\t[!] No se pudo leer la respuesta del Proceso receptor");
      exit(0);
   }

   if (strcmp(answer, "1") == 0){
      printf("\tSu peticion fue exitosa\n");
   }
   else{
      printf("\tSu peticion no fue exitosa\n");
   }
   close (fd1);
}

int main (int argc, char *argv[]){


  if (argc != 5){
    perror("\tNumero de argumentos invalidos\n");
    printf("\tej: ./debug/center –i files/PS.txt –p debug/pipeReceptor \n");
    exit (0);
  }
   
   int option;
   int optionMenu;
   int fd, create = 0;
   book bookRequest;
   mode_t fifo_mode = S_IRUSR | S_IWUSR;

   printf("\n");
   printf("\t___Welcome to BookTime___\n");
   printf("\t1. Leer el archivo\n");
   printf("\t2. Digitar la informacion del libro\n");
   printf("\tEscoja una opcion: ");
   scanf("\t%d", &option);

   printf("\tSe crea el pipe %s para recibir la respuesta del receptor\n", namePS);
   printf("\t---------------------\n");
   
   unlink(namePS); 
   if (mkfifo (namePS, fifo_mode) == -1) {
      perror("\t[!] Request mkfifo");
      exit(1);
   }

   printf("\tSe abre el %s para enviar proceso a realizar\n", argv[4]);
   printf("\t---------------------\n");

   do { 
      if ((fd = open(argv[4], O_WRONLY))== -1) {
         perror("\t[!] Error en la lectura del pipe\n");
         printf("\tSe volvera a intentar despues\n");
	      sleep(5);        
      } else create = 1;
   } while (create == 0);

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
         createRequest(argv[4], booksRequests[i], fd);
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
                  fflush(stdin);
                  scanf("%s",&bookRequest.name);
                  printf("\tInserte el ISBN del libro\n");
                  scanf("%d", &bookRequest.ISBN);
                  bookRequest.operation = 'P';
                  strcpy(bookRequest.secondpipe, namePS);
                  printf("\t---------------------\n");
                  printf("\tSe crea solicitud\n");
                  printf("\tSolicitud: %c\n", bookRequest.operation);
                  printf("\tNombre: %s\n", bookRequest.name);
                  printf("\tISBN: %d\n", bookRequest.ISBN);
                  printf("\tPipe: %s\n", bookRequest.secondpipe);
                  createRequest(argv[4], bookRequest, fd);
                  break;
               case 2:
                  printf("\n");
                  printf("\tInserte el nombre del libro\n");
                  fflush(stdin);
                  scanf("%s",&bookRequest.name);
                  printf("\tInserte el ISBN del libro\n");
                  scanf("%d", &bookRequest.ISBN);
                  bookRequest.operation = 'D';
                  strcpy(bookRequest.secondpipe, namePS);
                  printf("\t---------------------\n");
                  printf("\tSe crea solicitud\n");
                  printf("\tSolicitud: %c\n", bookRequest.operation);
                  printf("\tNombre: %s\n", bookRequest.name);
                  printf("\tISBN: %d\n", bookRequest.ISBN);
                  printf("\tPipe: %s\n", bookRequest.secondpipe);
                  createRequest(argv[4], bookRequest, fd);
                  break;
               case 3:
                  printf("\n");
                  printf("\tInserte el nombre del libro\n");
                  fflush(stdin);
                  scanf("%s",&bookRequest.name);
                  printf("\tInserte el ISBN del libro\n");
                  scanf("%d", &bookRequest.ISBN);
                  bookRequest.operation = 'R';
                  strcpy(bookRequest.secondpipe, namePS);
                  printf("\t---------------------\n");
                  printf("\tSe crea solicitud\n");
                  printf("\tSolicitud: %c\n", bookRequest.operation);
                  printf("\tNombre: %s\n", bookRequest.name);
                  printf("\tISBN: %d\n", bookRequest.ISBN);
                  printf("\tPipe: %s\n", bookRequest.secondpipe);
                  createRequest(argv[4], bookRequest, fd);
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
   close(fd);
   printf("\tEl proceso %s termina\n", namePS);
   exit(0);
}