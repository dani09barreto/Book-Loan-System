#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "book.h"

data dataBase [MAXBOOK];
int posData = 0;
int posRequest = 0;
date dateDefault;

int tokDataRequest (char *line, int stoks){

   int i = 0;
   char *token;
   token = strtok(line, ",");

   while (token){
      if (i == 0){
         dataBase[posData - 1].requests[posRequest].stock = atoi(token);
      }
      i ++;
      token = strtok(NULL, ",-");
      if (i == 1){
         dataBase[posData - 1].requests[posRequest].operation = token[0];
      }
      if (i == 2){
         dataBase[posData - 1].requests[posRequest].initialDate.day = atoi(token);
         dateDefault.day = atoi(token);
      }
      if (i == 3){
         dataBase[posData - 1].requests[posRequest].initialDate.month = atoi(token);
         dateDefault.month = atoi(token);
      }
      if (i == 4){
         dataBase[posData - 1].requests[posRequest].initialDate.year = atoi(token);
         dateDefault.year = atoi(token);
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
      }
      i ++;
      token = strtok(NULL, ",-");
      if (i == 1){
         dataBase[posData].ISBN = atoi(token);
      }      
      if (i == 2){
         dataBase[posData].stocks = atoi(token);
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

void sendAnswer(int bit, int fd){
   if (bit == 1){
      write(fd, "1", 10); 
   }
   if (bit == 0){
      write(fd, "0", 10); 
   }
}

void requestBook (book *bookRequest, int fd){
   
   int make = 0;
   int count = 0;
   int bit = 0;
   time_t t = time(NULL);
   struct tm tm = *localtime(&t);


   printf("\tSe abre %s para enviar la respuesta a la solicitud\n", bookRequest->secondpipe);
   printf("\t---------------------\n");
   
   do{
      if ((fd = open (bookRequest->secondpipe, O_WRONLY)) == -1){
         perror("\tReceptor abriendo el pipe respuesta\n");
         perror("\tSe intentara mas tarde\n");
         sleep(5);
      }
      else{
         make = 1;
      }
   }while(make == 0);


   for(int i = 0; i < posData; i++){

      if(dataBase[i].ISBN == bookRequest->ISBN){

         for(int j = 0; j < dataBase[i].stocks; j++){

            if(dataBase[i].requests[j].operation == 'D'){

               dataBase[i].requests[j].operation = 'P';
               dataBase[i].requests[j].initialDate.day = tm.tm_mday;
               dataBase[i].requests[j].initialDate.month = (tm.tm_mon + 1);
               dataBase[i].requests[j].initialDate.year = (tm.tm_year + 1900);
               bit = 1;
               printf("\tSe escribe la respuesta al PS solicitud\n");
               sendAnswer(bit, fd);
               return;                
            }
         }
         if (bit == 0){
            printf("\t Se escribe la respuesta al PS solicitud\n");
            sendAnswer(bit, fd);
            return;
         }
      }
      count++;
   }
   if(posData == count){
      printf("\t Se escribe la respuesta al PS solicitud\n");
      sendAnswer(bit, fd);
   }

}

void returnBook (book *bookRequest, int fd){

   int make = 0;
   int count = 0;
   int bit = 0;
   time_t t = time(NULL);
   struct tm tm = *localtime(&t);

   printf("\tSe abre %s para enviar la respuesta a la solicitud\n", bookRequest->secondpipe);
   printf("\t---------------------\n");
   do{
      if ((fd = open (bookRequest->secondpipe, O_WRONLY)) == -1){
         perror("\tReceptor abriendo el pipe respuesta\n");
         perror("\tSe intentara mas tarde\n");
         sleep(5);
      }
      else{
         make = 1;
      }
   }while(make == 0);

   for(int i = 0; i < posData; i++){

      if(dataBase[i].ISBN == bookRequest->ISBN){

         for(int j = 0; j < dataBase[i].stocks; j++){

            if(dataBase[i].requests[j].operation == 'P'){

               dataBase[i].requests[j].operation = 'D';
               dataBase[i].requests[j].initialDate.day = tm.tm_mday;
               dataBase[i].requests[j].initialDate.month = (tm.tm_mon + 1);
               dataBase[i].requests[j].initialDate.year = (tm.tm_year + 1900);
               bit = 1;
               printf("\tSe escribe la respuesta al PS solicitud\n");
               sendAnswer(bit, fd);
               return;                
            }
         }
         if (bit == 0){
            printf("\t Se escribe la respuesta al PS solicitud\n");
            sendAnswer(bit, fd);
            return;
         }
      }
      count++;
   }
   if(posData == count){
      printf("\t Se escribe la respuesta al PS solicitud\n");
      sendAnswer(bit, fd);
   }
}

void renovateBook (book *bookRequest, int fd){
    
   int make = 0;

   printf("\tSe abre %s para enviar la respuesta a la solicitud\n", bookRequest->secondpipe);
   printf("\t---------------------\n");
   do{
      if ((fd = open (bookRequest->secondpipe, O_WRONLY)) == -1){
         perror("\tReceptor abriendo el pipe respuesta\n");
         perror("\tSe intentara mas tarde\n");
         sleep(5);
      }
      else{
         make = 1;
      }
   }while(make == 0);
   printf("\tProceso receptor envia respuesta a pipe del proceso solicitud\n");
   write(fd, "1", 10);
}

int main (int argc, char *argv[]){

   int  fd, fd1, bytes,create = 0;
   book bookRequest;
   mode_t fifo_mode = S_IRUSR | S_IWUSR;
  
   if (argc != 7){
      perror("\tNumero de argumentos invalidos\n");
      printf("\tej: ./debug/receptor –p debug/pipeReceptor –f files/filedatos –s filesalida\n");
      exit (0);
   }

   readDataBase(argv[4]);
   printf("\tDate default: %d-%d-%d", dateDefault.day, dateDefault.month, dateDefault.year);
   printf("\n");
   printf("\tSe leyo de la BD\n");
   printf("\t----------------\n");
   for (int i = 0; i < posData; i ++){
      printf ("\t%s %i %i\n", dataBase[i].name, dataBase[i].ISBN, dataBase[i].stocks);
      for(int j = 0; j < dataBase[i].stocks; j ++){
         printf ("\t%i,%c,%i-%i-%i\n", dataBase[i].requests[j].stock, dataBase[i].requests[j].operation, dataBase[i].requests[j].initialDate.day, dataBase[i].requests[j].initialDate.month, dataBase[i].requests[j].initialDate.year);
      }
   }

   printf("\n");
   printf("\t---------------------------------------------\n");
   printf("\tSe crea el pipe %s para recibir solicitud\n", argv[2]);
   printf("\t---------------------\n");
   unlink(argv[2]);
   if (mkfifo (argv[2], fifo_mode) == -1) {
      perror("\tReceptor mkfifo");
      exit(1);
   }
   printf("\tSe esta abriendo el pipe %s para recibir solicitud\n", argv[2]);
   printf("\t---------------------\n");
   if ((fd = open(argv[2], O_RDONLY)) == -1){
      perror("\tError al abrir el pipe\n");
      exit (0);
   }

   printf("\tSe abrio %s \n", argv[2]);
   printf("\t---------------------\n");

   do{
      
      printf("\t---------------------------\n");      
      printf("\tSe esta leyendo la solicitud que manda el PS\n"); 
      bytes = read (fd, &bookRequest, sizeof(bookRequest));
      if (bytes == 0){
         printf("\t---------------------------\n");
         printf("\tAcabo el proceso se esperara 40 segundos para que genere una solicitud mas\n");
         sleep(40);
         bytes = read (fd, &bookRequest, sizeof(bookRequest));
         if (bytes == 0)
            break;
      }
      if (bytes == -1) {
         perror("\tproceso lector:");
         exit(1);
      }
      printf("\t---------------------------\n");
      printf("\tSe lee:\n");
      printf("\tSolicitud: %c\n", bookRequest.operation);
      printf("\tNombre: %s\n", bookRequest.name);
      printf("\tISBN: %d\n", bookRequest.ISBN);
      printf("\tPipe: %s\n", bookRequest.secondpipe);
      
      do { 
         if ((fd1 = open(bookRequest.secondpipe, O_WRONLY)) == -1) {
            perror("\tReceptor Abriendo el segundo pipe\n");
            printf("\tSe volvera a intentar despues\n");
            sleep(5);
         } else create = 1; 
      }  while (create == 0);

      switch (bookRequest.operation){

      case 'P':
         requestBook(&bookRequest, fd1);
         break;
      case 'D':
         returnBook(&bookRequest, fd1);
         break;
      case 'R':
         returnBook(&bookRequest, fd1);
         break;
      default:
         printf("\tAccion no se puede realizar\n");
         break;
      }

   }while(bytes > 0);
   printf("\t------------------------\n");
   printf("\t----------------\n");
   for (int i = 0; i < posData; i ++){
      printf ("\t%s %i %i\n", dataBase[i].name, dataBase[i].ISBN, dataBase[i].stocks);
      for(int j = 0; j < dataBase[i].stocks; j ++){
         printf ("\t%i,%c,%i-%i-%i\n", dataBase[i].requests[j].stock, dataBase[i].requests[j].operation, dataBase[i].requests[j].initialDate.day, dataBase[i].requests[j].initialDate.month, dataBase[i].requests[j].initialDate.year);
      }
   }
   printf("\tel proceso termino\n");
   exit(0);
}





