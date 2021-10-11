#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "book.h"

data dataBase [MAXBOOK]; // Esta Variable dataBase es usada para guardar los nombre de los libros y sus datos de la BD:
int posData = 0; // Representa la ubicacion en el arreglo dataBase de los libros.
int posRequest = 0; // Representa la ubicacion de las solicitudes por cada libro.
date dateDefault; // Esta variable se define cada vez que se lee la base de datos para conocer la fecha de defecto

/*
Name : tokDataRequest
Parameters: char *line, int stoks
Function: Esta funcion es usada para guardar los libros por medio de la tokenizacion pues recibimos
 una linea y la cantidad de libros que existen y con ello guardamos esos datos (Cantidad, Solicitud,fecha)
  en el arreglo dataBase.
  Return value: int - Este retornara un 1 o un 0 dependiendo, si ya ha leido todas las solicitudes
  que existen por cada libro retorna un 0 de manera que la funcion readDataBase lea los datos del 
  siguiente libro y en caso contrario vuelva a llamar esta funcion para leer la siguiente solicitud.
*/

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

/*
Name : tokDataBook
Parameters :  char *line
Function: La funcion tokDataBook la usamos para guardar la informacion de cada libro en el arreglo 
dataBase usando la variable posData ademas de guardar los datos como el nombre, el isbn y la cantidad
de libros existentes.
Return value: int - Esta funcion retorna un 1 debido a que la informacion del libro solo esta
en una linea del archivo. Ademas ese retorno le informa a la funcion readDataBase que debe leer las
solicitudes.
*/

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

/*
Name : readDataBase
Parameters: char *filedata
Function : Esta funcion administra la lectura del archivo para poder determinar que informacion se 
esta leyendo por medio de la variable goRequest la cual nos indica en que funcion debemos estar leyendo 
ya sea la informacion del libro o las solicitudes del mismo.
Return value: void
*/

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

/*
Name : sendAnswer
Parameters : int bit, int fd
Function: Esta funcion reescribe el archivo, dependiendo el bit que recibe que le indica si una 
solicitud es exitosa o no.
Return value : void
*/

void sendAnswer(int bit, int fd){
   if (bit == 1){
      write(fd, "1", 10); 
   }
   if (bit == 0){
      write(fd, "0", 10); 
   }
}

/*
Name : requestBook
Parameters : book* bookRequest, int fd
Function : Esta funcion se encarga de realizar las peticiones de los libros ya que recibe la 
informacion de un libro por medio de la estructura de libro, y revisa que sea posible 
realizar una peticion, es decir , busca que haya algun libro devuelto y en dicho caso cambia su valor a
'P' de pedido y modifica su fecha por la actual.
Return value : void
*/

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

/*
Name : returnBook
Parameters : book* bookRequest, int fd
Function : Realiza la solicitud de devolver un libro, por lo que se busca un ejemplar el cual 
posea un estado de 'P' (pedido) para asi actualizar su estado y establecerlo como 'D'(devuelto) ademas 
de actualizar la fecha.
Return value : void
*/

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

/*
Name : renovateBook
Parameters : book* bookRequest, int fd
Function : Realizamos la renovacion de una libro pedido anteriormente por lo que aumentamos
10 dias su reserva y analizamos si esos 10 dias afectan el mes o el año.
Return value : void
*/

void renovateBook (book *bookRequest, int fd){
    
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

            if(dataBase[i].requests[j].operation == 'P' && dataBase[i].requests[j].initialDate.day == dateDefault.day){
               printf("si hay\n");

               dataBase[i].requests[j].operation = 'R';
               if ((dataBase[i].requests[j].initialDate.day + 10)%30 != dataBase[i].requests[j].initialDate.day + 10){

                  dataBase[i].requests[j].initialDate.day += 10;
                  dataBase[i].requests[j].initialDate.day %= 30;

                  if ((dataBase[i].requests[j].initialDate.month + 1)%12 != dataBase[i].requests[j].initialDate.month + 1){
                     dataBase[i].requests[j].initialDate.month = 1;
                     dataBase[i].requests[j].initialDate.year ++;
                  }
                  else{
                     dataBase[i].requests[j].initialDate.month ++;
                  }
               }
               else{
                  dataBase[i].requests[j].initialDate.day += 10;
               }
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

/*
Name : makeFile
Parameters : char* namefile
Function : La funcion se encarga de escribir el archivo de salida con la informacion de la base de
datos actualizada usando el nombre del archivo que le llega por parametro.
Return value : void
*/

void makeFile (char *namefile){
   
   FILE *fp = fopen(namefile, "w");
   if (fp == NULL){
      perror("\tEl Archivo no pudo ser abierto\n");
      exit (0);
   }

   for (int i = 0; i < posData; i ++){
      fprintf(fp, "%s,%d,%d\n", dataBase[i].name, dataBase[i].ISBN, dataBase[i].stocks);
      for (int j = 0; j < dataBase[i].stocks; j ++){
         fprintf(fp, "%d,%c,%d-%d-%d\n", dataBase[i].requests[j].stock, dataBase[i].requests[j].operation,dataBase[i].requests[j].initialDate.day, dataBase[i].requests[j].initialDate.month, dataBase[i].requests[j].initialDate.year);
      }
   }
   fflush(fp);
   fclose(fp);
   printf("\t Se actualizo la base de datos\n");
}


int main (int argc, char *argv[]){
  
   if (argc != 7){
      perror("\tNumero de argumentos invalidos\n");
      printf("\tej: ./debug/receptor –p debug/pipeReceptor –f files/filedatos –s filesalida\n");
      exit (0);
   }
   
   int  fd, fd1, bytes,create = 0;
   book bookRequest;
   mode_t fifo_mode = S_IRUSR | S_IWUSR;

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
         renovateBook(&bookRequest, fd1);
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
   makeFile(argv[6]);
   printf("\tel proceso termino\n");
   close(argv[2]);
   exit(0);
}





