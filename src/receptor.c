#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "book.h"

data dataBase [MAXBOOK];
int posData = 0;
int posRequest = 0;

int tokDataRequest (char *line, int stoks){

   int i = 0;
   char *token;
   token = strtok(line, ",");
   printf("pos data %i, posrequest %i\n", posData-1, posRequest);
   while (token){
      if (i == 0){
         dataBase[posData - 1].requests[posRequest].stock = atoi(token);
         printf("%i\n", dataBase[posData - 1].requests[posRequest].stock);
      }
      i ++;
      token = strtok(NULL, ",-");
      if (i == 1){
         dataBase[posData - 1].requests[posRequest].operation = token[0];
         printf("%c\n", dataBase[posData - 1].requests[posRequest].operation);
      }
      if (i == 2){
         dataBase[posData - 1].requests[posRequest].initialDate.day = atoi(token);
         printf("%i\n", dataBase[posData - 1].requests[posRequest].initialDate.day);
      }
      if (i == 3){
         dataBase[posData - 1].requests[posRequest].initialDate.month = atoi(token);
         printf("%i\n", dataBase[posData - 1].requests[posRequest].initialDate.month);
      }
      if (i == 4){
         dataBase[posData - 1].requests[posRequest].initialDate.year = atoi(token);
         printf("%i\n", dataBase[posData - 1].requests[posRequest].initialDate.year);
      }
   }
   posRequest ++;

   if (posRequest == stoks){
      posRequest = 0;
      return 0;
   }
   else{
      return 1;
   }
}

int tokDataBook (char *line){

   int i = 0;
   char *token;
   token = strtok(line, ",");

   while (token){

      if (i == 0){
         strcpy(dataBase[posData].name, token);
         printf("%s\n", dataBase[posData].name);
      }
      i ++;
      token = strtok(NULL, ",-");
      if (i == 1){
         dataBase[posData].ISBN = atoi(token);
         printf("%i\n", dataBase[posData].ISBN);
      }      
      if (i == 2){
         dataBase[posData].stocks = atoi(token);
         printf("%i\n", dataBase[posData].stocks);
      }     
   }
   posData ++;
   return 1;
}

void readDataBase (char *filedata){

   FILE *fd;
   char line [MAXLINE];
   int goRequest = 0;
   int stoks;
   fd = fopen(filedata, "r");

   while (fgets(line, sizeof(line), fd)){

      if (goRequest == 0){
         goRequest = tokDataBook(line);
      }
      else{
         goRequest = tokDataRequest(line, dataBase[posData - 1].stocks);
      }
   }
}

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

   readDataBase(argv[4]);
   printf("Se leyo de la BD\n");
   for (int i = 0; i < posData; i ++){
      printf ("%s %i %i\n", dataBase[i].name, dataBase[i].ISBN, dataBase[i].stocks);
      for(int j = 0; j < dataBase[i].stocks; j ++){
         printf ("%i,%c,%i-%i-%i\n", dataBase[i].requests[j].stock, dataBase[i].requests[j].operation, dataBase[i].requests[j].initialDate.day, dataBase[i].requests[j].initialDate.month, dataBase[i].requests[j].initialDate.year);
      }
   }
   /*
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
   */
   exit(0);
}





