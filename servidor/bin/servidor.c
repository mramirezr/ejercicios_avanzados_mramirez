#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <unistd.h>

/*
Estructura para 
*/
typedef struct
{
	int sock;
	struct sockaddr dir_ip;
	int addr_len;
} connection_t;

void * proceso(void * ptr)
{
	int len;
	connection_t * conn;
	long addr = 0;
    int trama[8];
    int sind[3];
    int err;
    int i;
	if (!ptr) pthread_exit(0); 
	conn = (connection_t *)ptr;

	/* 
    Se hace lectura de la trama
    */
	read(conn->sock, trama, sizeof(trama));
    sind[0]=(trama[1]+trama[4]+trama[6]+trama[7])%2;
    sind[1]=(trama[2]+trama[4]+trama[5]+trama[6])%2;
    sind[2]=(trama[3]+trama[5]+trama[6]+trama[7])%2;
    printf("\n Sindrome: ");
    for(i=0;i<3;i++)
        printf("%d",sind[i]);
    printf("\n");
    err=sind[0]+2*sind[1]+4*sind[2];

    printf("\n Secuencia Recibida: ");
    for(i=0;i<8;i++)
        printf("%d",trama[i]);
    printf("\n");
    if(err>0)
    {
        printf("Se tiene un error\n");
    }
    else
    {
        printf("La trama no contiene errores\n");
    }

	/* 
    Se cierra el socket y se liberá la conexión
     */
	close(conn->sock);
	free(conn);
	pthread_exit(0);
}

int main(int argc, char ** argv)
{
	int sock = -1;
	struct sockaddr_in dir_ip;
	int port;
	connection_t * connection;
	pthread_t thread;

	/*
    Se revisa que se haya dado un argumento, de lo contrario se arroja 
    mensaje de error 
    */
	if (argc != 2)
	{
		fprintf(stderr, "Agregue como parametro un puerto\n");
		return -1;
	}

	/* 
    Si se da un parametro,e verifica que se haya pasado un valor de número
    entero de lo contrario se manda mensaje de error
    */
	if (sscanf(argv[1], "%d", &port) <= 0)
	{
		fprintf(stderr, "Valor del puerto erroneo\n");
		return -2;
	}

	/*
    Se crea un socket para atender en el servidor.
    Se selecciona Dominio de direcciones AF INET
    El tipo de socket de flujo
    Y se utiliza TCP como protocolo de transporte
    Si el socket no se puede crear se manda error
    */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock <= 0)
	{
		fprintf(stderr, "No es posible crearse el socket\n", argv[0]);
		return -3;
	}

	/*
    Se enlaza el socket con el puerto.
    Si no es posible realizarse el enlace se manda error
    */
	dir_ip.sin_family = AF_INET;
	dir_ip.sin_addr.s_addr = INADDR_ANY;
	dir_ip.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&dir_ip, sizeof(struct sockaddr_in)) < 0)
	{
		fprintf(stderr, "no es posible enlazarse con el puerto %d \n", port);
		return -4;
	}

	/* 
    Se pone al socket en modo de escucha
    */
	if (listen(sock, 0) < 0)
	{
		fprintf(stderr, "No se puede escuchar desde el puerto\n");
		return -5;
	}

	printf("Escuchando...\n");
	
	while (1)
	{
		/* 
        Se espera a que se tengan conexiones por parte de clientes
        */
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sock, &connection->dir_ip, &connection->addr_len);
		if (connection->sock <= 0)
		{
			free(connection);
		}
		else
		{
			/* 
            Se crea el proceso sobre un hilo.
            */
			pthread_create(&thread, 0, proceso, (void *)connection);
			pthread_detach(thread);
		}
	}
	
	return 0;
}