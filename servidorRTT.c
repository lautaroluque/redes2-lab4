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

			char * usuario = malloc(BUFSIZE);
			char * contrasena = malloc(BUFSIZE);
			char * combo = malloc(BUFSIZE);
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
				strcpy(usuario, respuesta);
				usuario += 5;
				for(int i = 0; i <= strlen(usuario); i++){
					if(isspace(usuario[i]) != 0){
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
				strcpy(contrasena, respuesta);
				contrasena += 5;
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
				if(strncmp(respuesta, "RETR ", 5) == 0){
					char * nombrearchivo = malloc(strlen(respuesta)+1);
					strcpy(nombrearchivo, respuesta);
					nombrearchivo += 5;
					for(int i = 0; i <= strlen(nombrearchivo); i++){
						if(isspace(nombrearchivo[i]) != 0){
							nombrearchivo[i] = '\0';
							break;
						}
					}

					FILE * archivo;
					archivo = fopen(nombrearchivo, "r");

					if(archivo){
						int tamanoarchivo;
						fseek(archivo, 0, SEEK_END);
						tamanoarchivo = ftell(archivo);
						rewind(archivo);

						bzero(respuesta, BUFSIZE);
						bzero(mensaje, BUFSIZE);
						sprintf(mensaje, MSG_299, nombrearchivo, tamanoarchivo);
						if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0){
							printf("Error en el envio\n");
							break;
						}
						if((tamanorespuesta = recv(socketconexion, respuesta, BUFSIZE, 0)) < 0){
							printf("Error en la respuesta\n");
							break;
						}

						bzero(mensaje, BUFSIZE);
						while(fread(mensaje, BUFSIZE, 1, archivo) == BUFSIZE){
							if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0){
								printf("Error en el envio\n");
								break;
							}
						}

						bzero(respuesta, BUFSIZE);
						bzero(mensaje, BUFSIZE);
						sprintf(mensaje, MSG_226);
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
						sprintf(mensaje, MSG_550, nombrearchivo);
						if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0){
							printf("Error en el envio\n");
							break;
						}
						if((tamanorespuesta = recv(socketconexion, respuesta, BUFSIZE, 0)) < 0){
							printf("Error en la respuesta\n");
							break;
						}
					}
				}
			}else if(login == 0){
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
