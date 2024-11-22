# Tarea2_SO
Tarea Número 2 - Hilos y Paginación

## Parte 1: Compilar y Ejecutar Tarea2v4.cpp
+ Comando para compilar en WSL: g++ Tarea2v4.cpp
+ Comando para ejecutar en WSL: ./a.out -p "val" -c "val" -s "val" -t "val"
+ Donde "val" es el valor numérico asignado a cada argumento. Los argumentos se definen de la siguiente manera: "-p" es la cantidad de hilos productores, "-c" es la cantidad de hilos consumidores, "-s" es el tamaño de la cola y "-t" es el tiempo de espera de los consumidores luego de que el último productor haya terminado de ocupar la cola. La lectura de argumentos está programada de tal manera que mientras cada argumento esté acompañado de su "val" estos pueden ir en cualquier orden, por ejemplo: ./a.out -s "val" -t "val" -p "val" -c "val" 

## Parte 2: Compilar y Ejecutar vmemory.cpp
+ Comando para compilar en WSL: g++ vmemory.cpp
+ Comando para ejecutar en WSL: ./a.out 3 "FIFO" "referencias.txt"
+ Los argumentos son: el número de marcos de página, el algoritmo de reemplazamiento (con las opciones: "Optimo", "FIFO", "LRU" o "RELOJ") y el archivo con direcciones virtuales (el formato es: números enteros seguidos de un salto de linea) respectivamente.
