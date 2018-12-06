#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <winsock2.h>
#include <windows.h>

#define SZREQ 512
#define SZPORT 12
#define SZADDR 128
#define SZBUF 1024
#define DEFADDR "127.0.0.1"
#define DEFPORT 80

/* 
   compile with tinycc 
   tcc get.c ws2_32.def 
*/ 

void ShowHelp()
{
    printf(
        "get <addr> [<port>] [/<req>] [.]\n"
        "connect to <addr> at <port> "
        "and listen or GET request <req> "
        "\noutput to stdout\n\n"
        "addr: default 127.0.0.1\n"
        "port: default 80\n"
        "req: request by http\n"
        ".: with header/ stop at first line");
    exit(0);
}

int pErr(char *s)
{
    int x;
    x = WSAGetLastError();
    printf("%s %ld\r\n", s, x);
    return x;
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET so;
    SOCKADDR_IN service;
    int i, j, k, iResult, sent, received;
    int once = 0;
    int web = 0;
    int portnum = 0;
    char mark[4] = "\r\n\r\n";
    char *buf;
    char *addr;
    char *port;
    char *rq;
    if (argc < 2) ShowHelp();
    // output \n no change to \r\n
    setmode(STDOUT_FILENO, _O_BINARY);
    addr = malloc(SZADDR);
    memset(addr, 0, SZADDR);
    rq = malloc(SZREQ);
    memset(rq, 0, SZREQ);
    port = malloc(SZPORT);
    memset(port, 0, SZPORT);
    for (i = 1; i < argc; i++) {
        if ((strlen(rq) == 0) && (strlen(argv[i]) > 0)
            && (strstr(argv[i], "/") != NULL)) {
            strcpy(rq, argv[i]);
            web = 1;
            continue;
        }
        if ((strlen(addr) == 0) && (strlen(argv[i]) > 3)
            && (strstr(argv[i], ".") != NULL)) {
            strcpy(addr, argv[i]);
            continue;
        }
        if (portnum == 0) {
            strcpy(port, argv[i]);
            portnum = atoi(port);
            continue;
        }
        if (strcmp(argv[i], ".") == 0) {
            once = 1;
        }
    }
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    do {
        if (iResult != NO_ERROR) {
            pErr("err startup");
            break;
        }
        so = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (so == INVALID_SOCKET) {
            iResult = pErr("err socket():");
            break;
        }
        service.sin_family = AF_INET;
        if (strlen(addr) == 0) strcpy(addr, DEFADDR);
        service.sin_addr.s_addr = inet_addr(addr);
        if (portnum == 0) portnum = DEFPORT;
        service.sin_port = htons(portnum);
        if (connect(so,
                    (SOCKADDR *)&service,
                    sizeof(service)) == SOCKET_ERROR) {
            iResult = pErr("err connect");
            break;
        }
        buf = malloc(SZBUF);
        memset(buf, 0, SZBUF);
        sprintf(buf, "get %s %d", addr, portnum);
        SetConsoleTitle(buf);
        free(addr);
        free(port);
        k = strlen(rq);
        memset(buf, 0, SZBUF);
        if (web) {
            sprintf(buf, "GET %s HTTP/1.1\r\n\r\n", rq);
            k = strlen(buf);
            sent = send(so, buf, k, 0);
            if (sent < k) {
                iResult = pErr("err send");
                break;
            }
        }
        free(rq);
        received = SZBUF;
        j = 0;
        do {
            memset(buf, 0, received);
            received = recv(so, buf, SZBUF, 0);
            k = 0;
            if (web && (!once)) {
                i = 0;
                while ((i < received) && (j <= 3)) {
                    if (*(buf + i) == *(mark + j))
                        ++j;
                    else
                        j = 0;
                    ++i;
                }
                if (j > 3) {
                    k = i ;
                    web = 0;
                }
            }
            if (received == 0 || received == WSAECONNRESET)
                break;
            else
                fwrite(buf + k, 1, received - k, stdout);
        }
        while ((received > 0) && (!once || web));
        iResult = 0;
    }
    while (0);
    WSACleanup();
    free(buf);
    return iResult;
}
