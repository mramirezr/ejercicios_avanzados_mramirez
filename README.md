# ejercicios_avanzados_mramirez
Cliente/Servidor para envio de tramas con verificacion de errores

Si se desea compilar el servidor debido a que maneja hilos se debe compilar de la siguiente manera:
cc servidor.c -o server -pthread
Al correr el programa se debe especificar como argumento un puerto.

Se puede compilar el cliente de la siguiente manera:
cc cliente.c -o client
Para el cliente al momento de correr el programa se debe especificar los siguientes agumentos:
./cliente IP puerto ABCD(4 bits) X(un error que va de 1 a 7)

La trama que se mandará del cliente al servidor está formada de la siguiente forma:

bit 0 -> 0 inicio de trama
bit 1 -> primer bit de paridad
bit 2 -> segundo bit de paridad
bit 3 -> tercer bit de paridad
bit 4 -> A
bit 5 -> B
bit 6 -> C
bit 7 -> D

Si el valor de X es mayor a 0, entonces realizará una operación xor con 1 al bit seleccionado para simular un error en el canal.

Para hacer una corrección y detección de errores un solo bit de paridad NO SERVIRÁ de nada!
Por lo tanto se implementó un algoritmo simple de codificación de bloque 7,4.
Esta codificación toma 4 bits de información y agregará 3 bits de paridad que en el servidor permitiran detectar si existe algún problema con la información recibida.

OJO, esto está realizado con enteros, para fines didacticos se aprecia el funcionamiento de una mejor manera,
si esto fuera a implementarse lo CORRECTO sería realizarlo todo a nivel de bit.
