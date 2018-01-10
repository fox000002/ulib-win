/*
 * =====================================================================================
 *
 *       Filename:  simple.cpp
 *
 *    Description:  a simple socket example.
 *
 *        Version:  1.0
 *        Created:  2009-8-2 21:47:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huys (hys), huys03@gmail.com
 *        Company:  hu
 *
 * =====================================================================================
 */
#include "resource.h"

#include <windows.h>
#include <stdio.h>
#include "usocket.h"

char    localhost[] =   "127.0.0.1";    /* default host name            */

int main(int argc, char *argv[])
{
    USocket us;
    struct  sockaddr_in sad; /* structure to hold an IP address     */
	struct  hostent  *ptrh;  /* pointer to a host table entry       */
    struct  protoent *ptrp;  /* pointer to a protocol table entry   */
    char    buf[256];
	char    *host;           /* pointer to host name                */
	int     port;
	char    buffer[1000] = "HIX,AAPL,300,4,1,a-AAPL,2500,s\n"; 
	int     n;               /* number of characters read           */
	
	
    us.init();


    // us.gethostname(buf, 256);

    buf[strlen(buf)-2] = '\0';

    // printf("hostname : %s\n", buf);
	
	
	// memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
    // sad.sin_family = AF_INET;         /* set family to Internet     */

	// port = 9100;       /* use default port number      */

    // if (port > 0)                   /* test for legal value         */
    // {
        // sad.sin_port = htons((u_short)port);
    // }
	
	// host = localhost;
	
	// ptrh = gethostbyname(host);
    // if ( ((char *)ptrh) == NULL )
    // {
        // fprintf(stderr,"Invalid host: %s\n", host);
        // exit(1);
    // }
    // memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
	
	/* Map TCP transport protocol name to protocol number. */
    // if ( ((int)(ptrp = getprotobyname("tcp"))) == 0)
    // {
        // fprintf(stderr, "Cannot map \"tcp\" to protocol number");
        // exit(1);
    // }

    /* Create a socket. */
    //us.create(AF_INET, SOCK_STREAM, ptrp->p_proto);
	us.create();
	
	/* Connect the socket to the specified server. */
    if (SOCKET_ERROR == us.connect("127.0.0.1", 9100))
    {
        exit(1);
    }
	
	us.send(buffer, strlen(buffer));
    printf("send: %s\n", buffer);
	
	n = us.recv(buffer, sizeof(buffer));
    buffer[n] = '\0';
    printf("recv:\n%s\n", buffer);
	
	us.close();
	
    us.cleanup();

    //printf("Press any key to continue...");
    //getchar();

    return 0;
}

