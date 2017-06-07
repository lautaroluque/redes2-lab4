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
	while(socketconexion = accept(socketescucha, (struct sockaddr *)&cliente, (socklen_t *)sizeof(cliente))){
		char respuesta[BUFSIZE];
		char mensaje[BUFSIZE];
       	 	bzero(respuesta, 512);
        	bzero(mensaje, 512);

		strcpy(mensaje, MSG_220);
		send(socketconexion, mensaje, BUFSIZE, 0);
	}
	if(socketconexion < 0){
		fatale("Fallo en el accept");
	}

	return 0;
}
