#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 512
#define USRFILE "ftpusers"

#define MSG_220 "220 srvFtp version 1.0\r\n"
#define MSG_331 "331 Password required for %s\r\n"
#define MSG_230 "230 User %s logged in\r\n"
#define MSG_530 "530 Login incorrect\r\n"
#define MSG_221 "221 Goodbye\r\n"
#define MSG_550 "550 %s: no such file or directory\r\n"
#define MSG_299 "299 File %s size %d bytes\r\n"
#define MSG_226 "226 Transfer complete\r\n"

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define fatale(e) (exit((perror(e), -1)))
#define syntax(p) (fprintf(stderr, "Use la sintaxis: \n\t%s ip puerto\n", p))
