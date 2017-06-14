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

	bzero(rspst, BUFSIZE);
	if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0){
		printf("Error en la recepcion\n");
	}

	printf("%s\n", rspst);

	if(strcmp(rspst, MSG_220) == 0){
		printf("Ingrese su usuario: ");
		char * usr = NULL;
		size_t lngtd = 0;
		if(getline(&usr, &lngtd, stdin) > 0){
			bzero(mnsj, BUFSIZE);
			strcpy(mnsj, "USER ");
			strcat(mnsj, usr);
			send(sckt, mnsj, BUFSIZE, 0);
		}
	}

	bzero(rspst, BUFSIZE);
	if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0){
		printf("Error en la recepcion\n");
	}

	printf("%s\n", rspst);

	if(strncmp(rspst, "331 ", 4) == 0){
		printf("Ingrese su password: ");
		char * psswrd = NULL;
		size_t lngtd = 0;
		if(getline(&psswrd, &lngtd, stdin) > 0){
			bzero(mnsj, BUFSIZE);
			strcpy(mnsj, "PASS ");
			strcat(mnsj, psswrd);
			send(sckt, mnsj, BUFSIZE, 0);
		}
	}

	bzero(rspst, BUFSIZE);
	if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0){
		printf("Error en la recepcion\n");
	}

	printf("%s\n", rspst);

	if(strncmp(rspst, "530 ", 4) == 0){
		close(sckt);
		return 0;
	}

	if(strncmp(rspst, "230 ", 4) == 0){
		while(sckt){
			printf("Operacion:");
			char * prcn = NULL;
			char * nmbrrchv = NULL;
			size_t lngtd = 0;
			if(getline(&prcn, &lngtd, stdin) > 0){
				if(strncmp(prcn, "get ",4) == 0){
					if((strlen(prcn)) < 5){
						printf("Nombre de archivo invalido");
						break;
					}else{
						strcpy(nmbrrchv, prcn);
						nmbrrchv += 4;
					}
					bzero(mnsj, BUFSIZE);
					strcpy(mnsj, "RETR ");
					strcat(mnsj, nmbrrchv);
					send(sckt, mnsj, BUFSIZE, 0);
				}
			}

			bzero(rspst, BUFSIZE);
			if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0){
				printf("Error en la recepcion\n");
			}

			if(strncmp(rspst, "299 ", 4) == 0){
				printf("%s\n", rspst);
				char * strtmnrchv;
				int tmnrchv;
				strcpy(strtmnrchv, rspst);
				strtmnrchv += 9 + (strlen(nmbrrchv)) + 6;
				for(int i = 0; i <= strlen(strtmnrchv); i++){
					if(isspace(strtmnrchv[i]) != 0){
						strtmnrchv[i] = '\0';
						break;
					}
				}
				tmnrchv = atoi(strtmnrchv);

				FILE * rchv;
				rchv = fopen(nmbrrchv, "w");

				int trfr = tmnrchv;

				bzero(rspst, BUFSIZE);
				while(((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) > 0) && (trfr > 0)){
					fwrite(rspst, sizeof(char), tmrspst, rchv);
					trfr -= tmrspst;
				}

				bzero(rspst, BUFSIZE);
				if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0){
					printf("Error en la recepcion\n");
				}
				printf("%s\n", rspst);
				break;
			}else if(strncmp(rspst, "550 ", 4) == 0){
				printf("%s\n", rspst);
				break;
			}

		}
	}

	return 0;
}
