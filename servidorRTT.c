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

	struct sockaddr_in server = { .sin_family = AF_INET, .sin_addr.s_addr = htonl(INADDR_ANY), .sin_port = htons(puerto)};

	if(bind(socketescucha, (struct sockaddr *) &server, sizeof(server)) < 0){
		fatale("Fallo en el bind");
	}

	listen(socketescucha, 1);
	for(;;){

		struct sockaddr_in cliente;
		socklen_t tamanocliente = sizeof(cliente);

		socketconexion = accept(socketescucha, (struct sockaddr *)&cliente, &tamanocliente);

		if(socketconexion < 0){
			fatale("Fallo en el accept");
		}

		while(socketconexion){
			char respuesta[BUFSIZE];
			char mensaje[BUFSIZE];
			int tamanomensaje;
			int tamanorespuesta;

			char * temp = malloc(BUFSIZE);
			char * temp2 = malloc(BUFSIZE);
			char * usuario;
			char * contrasena;
			char * combo = malloc(BUFSIZE);
			char * delimitadoruser = "USER ";
			char * delimitadorpass = "PASS ";
			FILE * lista;
			int login = 0;

       		 	bzero(respuesta, BUFSIZE);
        		bzero(mensaje, BUFSIZE);
			strcpy(mensaje, MSG_220);
			if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0){
				printf("Error en el envio\n");
				break;
			}
			if((tamanorespuesta = recv(socketconexion, respuesta, BUFSIZE, 0)) < 0){
				printf("Error en la respuesta\n");
				break;
			}

			if(strncmp(respuesta, "USER ", 5) == 0){
				strcpy(temp, respuesta);
				usuario = strtok(temp, delimitadoruser);
				int i = 0;
				char c;
				size_t largo = strlen(usuario);
				for(i; i <= largo; i++){
					c = usuario[i];
					if(isspace(c) != 0){
						usuario[i] = '\0';
						break;
					}
				}
			}

			bzero(respuesta, BUFSIZE);
	        	bzero(mensaje, BUFSIZE);
			sprintf(mensaje, MSG_331, usuario);
			if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0){
				printf("Error en el envio\n");
				break;
			}
			if((tamanorespuesta = recv(socketconexion, respuesta, BUFSIZE, 0)) < 0){
				printf("Error en la respuesta\n");
				break;
			}

			if(strncmp(respuesta, "PASS ", 5) == 0){
				strcpy(temp2, respuesta);
				contrasena = strtok(temp2, delimitadorpass);
			}else{
				printf("Respuesta invalida: password\n");
				break;
			}

			strcat(combo, usuario);
			strcat(combo, ":");
			strcat(combo, contrasena);

			lista = fopen(USRFILE, "r");
			if(lista){
				int leido;
				size_t largolinea = 0;
				char * linea = NULL;
				while((leido = getline(&linea, &largolinea, lista)) != -1){
					if((strcmp(combo, linea)) != 0){
						continue;
					}
					else{
						login = 1;
						break;
					}
				}
			}
			fclose(lista);

			if(login == 1){
				bzero(respuesta, BUFSIZE);
				bzero(mensaje, BUFSIZE);
				sprintf(mensaje, MSG_230, usuario);
				if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0){
					printf("Error en el envio\n");
					break;
				}
				if((tamanorespuesta = recv(socketconexion, respuesta, BUFSIZE, 0)) < 0){
					printf("Error en la respuesta\n");
					break;
				}
			}else{
				bzero(respuesta, BUFSIZE);
				bzero(mensaje, BUFSIZE);
				sprintf(mensaje, MSG_530);
				if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0){
					printf("Error en el envio\n");
					break;
				}
				close(socketconexion);
			}
		}
	}
}
