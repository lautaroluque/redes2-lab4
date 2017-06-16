#include "mensajesFTP.h"

int main(int argc, char* argv[])
{
    unsigned short puerto;
    int socketescucha;
    int socketconexion;
    struct sockaddr_in cliente;
    socklen_t tamanocliente;
    char respuesta[BUFSIZE];
    char mensaje[BUFSIZE];
    char buffer[BUFSIZE];
    int tamanomensaje;
    int tamanorespuesta;
    FILE * lista;
    char * usuario;
    char * contrasena;
    char * combo;
    int login;
    int leido;
    size_t largolinea;
    char * linea;
    char * nombrearchivo;
    FILE * archivo;
    int tamanoarchivo;
    int tamanoleido;
    int tamanoenviar;

    if(argc!=2)
    {
        printf("Uso: servidorFTP [puerto]\n");
    }
    puerto = atoi(argv[1]);
    if((socketescucha = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        fatale("No se pudo crear el socket");
    }
    struct sockaddr_in server = { .sin_family = AF_INET, .sin_addr.s_addr = htonl(INADDR_ANY), .sin_port = htons(puerto)};
    if(bind(socketescucha, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        fatale("Fallo en el bind");
    }
    listen(socketescucha, 1);
    for(;;)
    {
        tamanocliente = sizeof(cliente);
        socketconexion = accept(socketescucha, (struct sockaddr *)&cliente, &tamanocliente);
        if(socketconexion < 0)
        {
            fatale("Fallo en el accept");
        }
        usuario = malloc(BUFSIZE);
        contrasena = malloc(BUFSIZE);
        combo = malloc(BUFSIZE);
        login = 0;
        bzero(respuesta, BUFSIZE);
        bzero(mensaje, BUFSIZE);
        strcpy(mensaje, MSG_220);
        if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0)
        {
            printf("Error enviando mensaje inicial\n");
            break;
        }
        if((tamanorespuesta = recv(socketconexion, respuesta, BUFSIZE, 0)) < 0)
        {
            printf("Error recibiendo comando inicial\n");
            break;
        }

        if(strncmp(respuesta, "USER ", 5) == 0)
        {
            strcpy(usuario, respuesta);
            usuario += 5;
            usuario[strcspn(usuario, "\n")] = 0;
/*          for(int i = 0; i <= strlen(usuario); i++)
            {
                if(isspace(usuario[i]) != 0)
                {
                    usuario[i] = '\0';
                    break;
                }
            }
*/          printf("Ingresando usuario: %s\n", usuario);
        }
        else
        {
            printf("Comando inválido: %s\n", respuesta);
            break;
        }
        bzero(respuesta, BUFSIZE);
        bzero(mensaje, BUFSIZE);
        sprintf(mensaje, MSG_331, usuario);
        if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0)
        {
            printf("Error enviando solicitud de password\n");
            break;
        }
        if((tamanorespuesta = recv(socketconexion, respuesta, BUFSIZE, 0)) < 0)
        {
            printf("Error recibiendo password\n");
            break;
        }

        if(strncmp(respuesta, "PASS ", 5) == 0)
        {
            strcpy(contrasena, respuesta);
            contrasena += 5;
            contrasena[strcspn(contrasena, "\n")] = 0;
        }
        else
        {
            printf("Comando inválido: %s\n", respuesta);
            break;
        }
        strcat(combo, usuario);
        strcat(combo, ":");
        strcat(combo, contrasena);
        strcat(combo, "\n");
        lista = fopen(USRFILE, "r");
        if(lista)
        {
            largolinea = 0;
            linea = NULL;
            while((leido = getline(&linea, &largolinea, lista)) != -1)
            {
                if((strcmp(combo, linea)) != 0)
                {
                    continue;
                }
                else
                {
                    login = 1;
                    break;
                }
            }
        }
        fclose(lista);
        while(login == 1)
        {
            bzero(respuesta, BUFSIZE);
            bzero(mensaje, BUFSIZE);
            sprintf(mensaje, MSG_230, usuario);
            if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0)
            {
                printf("Error enviando confirmacion de login\n");
                break;
            }
            if((tamanorespuesta = recv(socketconexion, respuesta, BUFSIZE, 0)) < 0)
            {
                printf("Error recibiendo operación\n");
                break;
            }
            if(strncmp(respuesta, "RETR ", 5) == 0)
            {
                nombrearchivo = malloc(strlen(respuesta)+1);
                strcpy(nombrearchivo, respuesta);
                nombrearchivo += 5;
                nombrearchivo[strcspn(nombrearchivo, "\n")] = 0;
/*              for(int i = 0; i <= strlen(nombrearchivo); i++)
                {
                    if(isspace(nombrearchivo[i]) != 0)
                    {
                        nombrearchivo[i] = '\0';
                        break;
                    }
                }
*/              archivo = fopen(nombrearchivo, "r");
                if(archivo)
                {
                    fseek(archivo, 0, SEEK_END);
                    tamanoarchivo = ftell(archivo);
                    rewind(archivo);
                    bzero(respuesta, BUFSIZE);
                    bzero(mensaje, BUFSIZE);
                    sprintf(mensaje, MSG_299, nombrearchivo, tamanoarchivo);
                    if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0)
                    {
                        printf("Error enviando información de archivo\n");
                        break;
                    }
                    if((tamanomensaje = send(socketconexion, &tamanoarchivo, sizeof(int), 0)) < 0)
                    {
                        printf("Error en el enviando tamaño de archivo\n");
                        break;
                    }
                    bzero(buffer, BUFSIZE);
                    tamanoleido = 0;
                    tamanoenviar = tamanoarchivo;
                    while(tamanoenviar > 0)
                    {
                        tamanoleido = fread(buffer, 1, BUFSIZE, archivo);
                        if((tamanomensaje = send(socketconexion, buffer, tamanoleido, 0)) < 0)
                        {
                            printf("Error enviando archivo\n");
                            break;
                        }
                        tamanoenviar -= tamanoleido;
                    }
                    fclose(archivo);
                    bzero(mensaje, BUFSIZE);
                    sprintf(mensaje, MSG_226);
                    if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0)
                    {
                        printf("Error enviando confirmacion de envío\n");
                        break;
                    }
                }
                else
                {
                    bzero(mensaje, BUFSIZE);
                    sprintf(mensaje, MSG_550, nombrearchivo);
                    if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0)
                    {
                        printf("Error enviando mensaje de archivo inexistente\n");
                        break;
                    }
                }
            }
            else if(strncmp(respuesta, "QUIT", 4) == 0)
            {
                    bzero(respuesta, BUFSIZE);
                    bzero(mensaje, BUFSIZE);
                    sprintf(mensaje, MSG_221);
                    if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0)
                    {
                        printf("Error enviando saludo final\n");
                        break;
                    }
		close(socketconexion);
                break;
            }
            else
            {
                printf("Comando inválido: %s\n", respuesta);
                break;
            }
        }
        if(login == 0)
        {
            bzero(respuesta, BUFSIZE);
            bzero(mensaje, BUFSIZE);
            sprintf(mensaje, MSG_530);
            if((tamanomensaje = send(socketconexion, mensaje, BUFSIZE, 0)) < 0)
            {
                printf("Error enviando mensaje de usuario o contraseña inexistente");
                break;
            }
            close(socketconexion);
        }
    }
}
