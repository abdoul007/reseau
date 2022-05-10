#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <wait.h>

#include "../adresse_internet/adresse_internet_type.h"
#include "../adresse_internet/adresse_internet.h"
#include "../socket_tcp/tcp_socket.h"

#define PORT 8010
#define BUF_SIZE 5000

static int client = 0;

void * run (void * arg0)
{
	socket_tcp * a = (socket_tcp *) arg0;
	
	FILE * index = fopen("../data/index.html", "r");
	
	if(index == NULL)
	{
		printf("Erreur lors de l'ouverture du fichier ...");
		
		return NULL;
	}
	
	
	time_t timestamp = time( NULL );
    	struct tm * pTime = localtime( & timestamp );
	char timee [BUF_SIZE];
	strftime(timee, BUF_SIZE, "Date: %a, %d %b %Y %T GMT\n\n", pTime);
	
	printf("%s", timee);
	
	char buf [BUF_SIZE];
	
	strcpy(buf, "HTTP/1.1 200 OK\nServer: Apache\nContent-Type: text/html\n\n");
	if (writeSocketTCP(a, (void *) buf, strlen(buf)) == -1)
	{
		return NULL;
	}
		
	memset(buf, 0, BUF_SIZE);
	
	while(fgets(buf, BUF_SIZE, index) != NULL)
	{
		if (writeSocketTCP(a, (void *) buf, strlen(buf)) == -1)
		{
			return NULL;
		}	
	}

	
	closeSocketTCP(a);	
   	return arg0;
}

int main () 
{
	socket_tcp * s = init_socket_tcp();
	if (ajoute_ecoute_socket_tcp(s,"127.0.0.1", PORT) == -1) 
	{
		return 1;
	}
	
	printf("Le server Web est en attente de connexion sur le port %d...\n", PORT);
	printf("%d client connecte\n", client);
	
	while(1)
	{
		socket_tcp *a = init_socket_tcp();
		if (acceptSocketTCP(s, a)==-1)
		{
			return 2;
		}
	
	printf("%d client connecte(s)\n", ++client);
	// Creation du thread ...
 
	pthread_t th;
 
	if (pthread_create(&th, NULL,run, (void *) a) != 0) 
	{
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
}
	return 0;
}



