#include "mensajesRTT.h"

int main(int argc, char* argv[]){

	if(argc!=2){
		printf("Uso: servidorRTT [puerto]\n");
	}

	unsigned short puerto = atoi(argv[1]);
	int socketescucha;
	int socketconexion;

	if((socketescucha = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		fatale("No se pudo crear el socket");
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(puerto);

	if(bind(socketescucha, (struct sockaddr *) &server, sizeof(server)) < 0){
		fatale("Fallo en el bind");
	}

	listen(socketescucha, 1);

	struct sockaddr_in cliente;
	socklen_t tamanocliente = sizeof(cliente);

	socketconexion = accept(socketescucha, (struct sockaddr *)&cliente, &tamanocliente);

	if(socketconexion < 0){
		fatale("Fallo en el accept");
	}

	while(socketconexion){
		char respuesta[BUFSIZE];
		char mensaje[BUFSIZE];
       	 	bzero(respuesta, 512);
        	bzero(mensaje, 512);
		int tamanomensaje;
		int tamanorespuesta;

		strcpy(mensaje, MSG_220);

		if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0){
			printf("Error en el envio\n");
		}

		if((tamanorespuesta = recv(socketconexion, respuesta, BUFSIZE, 0)) < 0){
			printf("Error en la respuesta\n");
		}

		if(strncmp(respuesta, "USER ", 5) == 0){
			char * usuario;
			char * delimitador = "USER ";

			usuario = strtok(respuesta, delimitador);

			sprintf(mensaje, MSG_331, usuario);

			if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0){
				printf("Error en el envio\n");
			}

		}
	}

	return 0;
}
