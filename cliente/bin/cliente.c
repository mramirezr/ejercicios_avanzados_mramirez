#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char ** argv)
{
	int puerto;
	int sock = -1;
	struct sockaddr_in address;
	struct hostent * host;
	int len;
    int trama[8];
    int i;
	int err;
	/*
    Se verifica que se hayan colocado los suficientes
    estructura de argumentos:
    IP puerto mensaje(4 bits) error(del 1 al 7 si se coloca 0 no habrá errores)
    */

	if (argc != 5)
	{
		printf("argumentos insuficientes, influir IP puerto mensaje (4bits) y error\n");
		return -1;
	}

	/*
    Se obtiene el puerto y se verifica que sea entero
    */
	if (sscanf(argv[2], "%d", &puerto) <= 0)
	{
		fprintf(stderr, "puerto no válido\n");
		return -2;
	}

	/* 
    Se crea el socket
    Direcciones AF inet
    Del tipo flujo
    protocolo tcp
    */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock <= 0)
	{
		fprintf(stderr, "No es posible crear el socket\n");
		return -3;
	}

	/* 
    Se establece conexión con el servidor
    */
	address.sin_family = AF_INET;
	address.sin_port = htons(puerto);
	host = gethostbyname(argv[1]); // se toma la ip de los argumentos
	if (!host)
	{
		fprintf(stderr, "dirección del servidor no válida\n");
		return -4;
	}
	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
	if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
	{
		fprintf(stderr, "No se puede conectar con el servidor");
		return -5;
	}

	/* 
    Se construyen los bits de paridad utilizando una códificación de Bloque 7,4
    */
    trama[0]=0; // indica inicio de trama

    //datos
    trama[4]=(int)argv[3][0]-48; //convierte el valor del ascii a decimal 
    trama[5]=(int)argv[3][1]-48;
    trama[6]=(int)argv[3][2]-48;
    trama[7]=(int)argv[3][3]-48;

    //calculo de bits de paridad
    trama[1]=(trama[4]+trama[6]+trama[7])%2;
    trama[2]=(trama[4]+trama[5]+trama[6])%2;
    trama[3]=(trama[5]+trama[6]+trama[7])%2;
    err=(int)argv[4][0]-48;
    if(err>0)
    {
        trama[err]=(trama[err]+1)%2;
    }
	write(sock, trama, sizeof(trama));

	/* 
    Se cierra el socket
    */
	close(sock);

	return 0;
}