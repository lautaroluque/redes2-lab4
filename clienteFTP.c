#include "mensajesFTP.h"

int main(int argc, char* argv[])
{

    char * ip = argv[1];
    unsigned short prt = atoi(argv[2]);
    int sckt;
    char rspst[BUFSIZE];
    char mnsj[BUFSIZE];
    char bffr[BUFSIZE];
    int tmrspst;
    char * usr;
    size_t lngtd;
    char * psswrd;
    char * prcn;
    char * nmbrrchv;
    int tmnrchv;
    FILE * rchv;
    int trfr;

    if(argc!=3)
    {
        syntax("clienteFTP");
    }

    if((sckt = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        fatale("No se pudo crear el socket");
    }

    struct sockaddr_in srvr = { .sin_family = AF_INET, .sin_port = htons(prt), .sin_addr.s_addr = inet_addr(ip)};

    if(connect(sckt, (struct sockaddr *) &srvr, sizeof(srvr)) < 0)
    {
        fatale("Fallo en el connect");
    }

    bzero(rspst, BUFSIZE);
    if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0)
    {
        printf("Error en la recepcion\n");
    }

    printf("%s\n", rspst);

    if(strcmp(rspst, MSG_220) == 0)
    {
        printf("Ingrese su usuario: ");
        usr = NULL;
        lngtd = 0;
        if(getline(&usr, &lngtd, stdin) > 0)
        {
            usr[strcspn(usr, "\n")] = 0;
            bzero(mnsj, BUFSIZE);
            strcpy(mnsj, "USER ");
            strcat(mnsj, usr);
            send(sckt, mnsj, BUFSIZE, 0);
        }
    }

    bzero(rspst, BUFSIZE);
    if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0)
    {
        printf("Error recibiendo respuesta de usuario\n");
    }

    printf("%s\n", rspst);

    if(strncmp(rspst, "331 ", 4) == 0)
    {
        printf("Ingrese su password: ");
        psswrd = NULL;
        lngtd = 0;
        if(getline(&psswrd, &lngtd, stdin) > 0)
        {
            bzero(mnsj, BUFSIZE);
            strcpy(mnsj, "PASS ");
            strcat(mnsj, psswrd);
            send(sckt, mnsj, BUFSIZE, 0);
        }
    }

    bzero(rspst, BUFSIZE);
    if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0)
    {
        printf("Error recibiendo respuesta de login\n");
    }

    printf("%s\n", rspst);

    if(strncmp(rspst, "530 ", 4) == 0)
    {
        printf("Usuario o contraseña incorrecto\n");
        close(sckt);
        return 0;
    }

    if(strncmp(rspst, "230 ", 4) == 0)
    {
        while(sckt)
        {
            printf("Operacion: ");
            prcn = malloc(sizeof(char));
            nmbrrchv = NULL;
            lngtd = sizeof(char);
            fflush(stdin);
//          if(getline(&prcn, &lngtd, stdin) != -1)
            if(getdelim(&prcn, &lngtd, '\n', stdin) != -1)
            {
                prcn[strcspn(prcn, "\r")] = 0;
                if((strncmp(prcn, "QUIT",4) == 0) || (strncmp(prcn, "quit",4) == 0))
                {
                    bzero(mnsj, BUFSIZE);
                    strcpy(mnsj, "QUIT");
                    send(sckt, mnsj, BUFSIZE, 0);
                    break;
                }
                else if((strncmp(prcn, "GET ",4) == 0) || (strncmp(prcn, "get ",4) == 0))
                {
                    if((strlen(prcn)) < 5)
                    {
                        printf("Uso: GET <nombre de archivo>\n");
                        continue;
                    }
                    else
                    {
                        nmbrrchv = malloc(strlen(prcn)+1);
                        strcpy(nmbrrchv, prcn);
                        nmbrrchv += 4;
                        for(int i = 0; i <= strlen(nmbrrchv); i++)
                        {
                            if(isspace(nmbrrchv[i]) != 0)
                            {
                                nmbrrchv[i] = '\0';
                                break;
                            }
                        }
                        bzero(mnsj, BUFSIZE);
                        strcpy(mnsj, "RETR ");
                        strcat(mnsj, nmbrrchv);
                        send(sckt, mnsj, BUFSIZE, 0);
                    }
                }
                else
                {
                    printf("Operación inválida\n");
                    continue;
                }
            }
            else
            {
                continue;
            }

            bzero(rspst, BUFSIZE);
            if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0)
            {
                printf("Error recibiendo respuesta de la operación\n");
                break;
            }

            if(strncmp(rspst, "299 ", 4) == 0)
            {
                printf("%s\n", rspst);
                tmnrchv = 0;

                bzero(rspst, BUFSIZE);
                if((tmrspst = recv(sckt, &tmnrchv, sizeof(int), 0)) < 0)
                {
                    printf("Error recibiendo tamaño de archivo\n");
                    break;
                }

                rchv = fopen(nmbrrchv, "w");

                trfr = tmnrchv;

                bzero(bffr, BUFSIZE);
                while(trfr > BUFSIZE)
                {
                    if((tmrspst = recv(sckt, bffr, BUFSIZE, 0)) < 0)
                    {
                        printf("Error recibiendo archivo\n");
                        fclose(rchv);
                        break;
                    }
                    fwrite(bffr, 1, tmrspst, rchv);
                    trfr -= tmrspst;
                }
                if((tmrspst = recv(sckt, bffr, trfr, 0)) < 0)
                {
                    printf("Error recibiendo archivo\n");
                    fclose(rchv);
                    break;
                }
                fwrite(bffr, 1, tmrspst, rchv);

                fclose(rchv);

                bzero(rspst, BUFSIZE);
                if((tmrspst = recv(sckt, rspst, BUFSIZE, 0)) < 0)
                {
                    printf("Error recibiendo confirmacion de envio\n");
                    break;
                }
                printf("%s\n", rspst);
            }
            else if(strncmp(rspst, "550 ", 4) == 0)
            {
                printf("%s\n", rspst);
            }
        }
    }
    return 0;
}
