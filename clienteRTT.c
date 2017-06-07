#include "mensajesRTT.h"

int main(int argc, char* argv[]){

	if(argc!=3){
		syntax();
	}

	char * ip = argv[1];
        unsigned short prt = atoi(argv[2]);

        if((sckt = socket(PF_INET, SOCK_STERAM, 0)) < 0){
                fatale("No se pudo crear el socket");
        }

        struct socketaddr_in srvr = { .sin_family = AF_INET, .sin_port = htons(prt), .sin_addr = inetaddr(ip)};

        if(connect(sckt, (struct sockaddr *) &srvr, sizeof(srvr)) < 0){
                fatale("Fallo en el connect");
        }

	char rspst[BUFSIZE];
	char mnsj[BUFSIZE];

	bzero(rspst, 512);
	recv(sckt, rspst, BUFSIZE, 0);

	while(strcmp(rspst, MSG_220 == 0){
		printf(rspst);
		printf("\n");
		printf("Ingrese su usuario: ");
		char * usr = NULL;
		unsigned int lngtd;
		if(getline(&usr, &lngtd, stdin) > 0){
			bzero(mnsj, 512);
			strcpy(mnsj, "USER ");
			strcat(mnsj, usr);
			send(sckt, mnsj, BUFSIZE, 0);
		}
	}

	return 0;
}





