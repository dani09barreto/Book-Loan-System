# Sistemas-Para-Prestamo-De-Libros
Proyecto de la materia de sistemas operativos, creado en C y en el sistema operativo de Linux
# Debug
Para el debug del programa es necesario ejecutar primero el ejecutable receptor con el siguiente comando
```sh
cd src/
```
```sh
./debug/receptor -p debug/pipeReceptor -f files/BD.txt -s files/BDout.txt
```
Luego en necesario ejecutar los procesos solicitud con el ejecutable center de la siguiente manera (recuerde que no tiene limite de procesos)
```sh
./debug/center –i files/PS.txt –p debug/pipeReceptor
```
# Autores
- Daniel Felipe Barreto Rojas
- Carlos D'Silvestri Ruiz
- Santiago Gutierrez Vargas