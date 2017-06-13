#include "mensajesRTT.h"

int main(int argc, char* argv[]){

	if(argc!=3){
		syntax("clienteRTT");
	}

	char * ip = argv[1];
        unsigned short prt = atoi(argv[2]);
	int sckt;

        if((sckt = socket(PF_INET, SOCK_STREAM, 0)) < 0){
                fatale("No se pudo crear el socket");
        }

        struct sockaddr_in srvr = { .sin_family = AF_INET, .sin_port = htons(prt), .sin_addr.s_addr = inet_addr(ip)};

        if(connect(sckt, (struct sockaddr *) &srvr, sizeof(srvr)) < 0){
                fatale("Fallo en el connect");
        }

	char rspst[BUFSIZE];
	char mnsj[BUFSIZE];
	int tmrspst;

	printf("Conexion correcta\n");

	bzero(rspst, 512);
	if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0){
		printf("Error en la recepcion\n");
	}

	printf("%s\n", rspst);

	if(strcmp(rspst, MSG_220) == 0){
		printf("Ingrese su usuario: ");
		char * usr = NULL;
		size_t lngtd = 0;
		if(getline(&usr, &lngtd, stdin) > 0){
			bzero(mnsj, 512);
			strcpy(mnsj, "USER ");
			strcat(mnsj, usr);
			send(sckt, mnsj, BUFSIZE, 0);
		}
	}

	bzero(rspst, 512);
	if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0){
		printf("Error en la recepcion\n");
	}

	printf("%s\n", rspst);

	if(strncmp(rspst, "331 ", 4) == 0){
		printf("Ingrese su password: ");
		char * psswrd = NULL;
		size_t lngtd = 0;
		if(getline(&psswrd, &lngtd, stdin) > 0){
			bzero(mnsj, 512);
			strcpy(mnsj, "PASS ");
			strcat(mnsj, psswrd);
			send(sckt, mnsj, BUFSIZE, 0);
		}
	}

	bzero(rspst, 512);
	if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0){
		printf("Error en la recepcion\n");
	}

	printf("%s\n", rspst);

	return 0;
}
