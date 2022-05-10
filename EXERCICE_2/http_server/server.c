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
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <wait.h>

#include 	"../adresse_internet/adresse_internet_type.h"
#include 	"../adresse_internet/adresse_internet.h"
#include 	"../socket_tcp/tcp_socket.h"
#include 	"server_http.h"



static int client = 0;
 
void * run (void * arg0)
{
	socket_tcp * a = (socket_tcp *) arg0;
	        
    char* buffer = NULL;
    buffer = (char *) malloc(BUF_SIZE*sizeof(char));
    
    char * req = (char *) malloc(BUF_SIZE*sizeof(char));
    
    if (readSocketTCP(a, (void *) buffer, BUF_SIZE) == -1)
    {
		 return NULL;
	}                
    
	
	/********** GESTION ET AFFICHAGE DU JOUR ET L'HEURE À LAQUELLE LA REQUETE DU CLIENT EST LANCE ****************/
	
	time_t timestamp = time( NULL );
    	struct tm * pTime = localtime( & timestamp );
	char timee [BUF_SIZE];
	strftime(timee, BUF_SIZE, "Date: %a, %d %b %Y %T GMT\n\n", pTime);
	
	if (writeSocketTCP(a, (void *) timee, strlen(timee)) == -1) 
		{
			return NULL;
		}
	
	printf("%s", timee);
	
	
	FILE * index = fopen("index.html", "r");
	
	if (index == NULL) 
	{
		if (writeSocketTCP(a, (void *) ERROR_404, strlen(ERROR_404)) == -1) 
		{
			return NULL;
		}

		
	}
	else {
				
		if (writeSocketTCP(a, (void *) OK_200, strlen(OK_200)) == -1) 
		{
			return NULL;
		}
		while (fgets(req, BUF_SIZE, index) != NULL) 
		{
			if (writeSocketTCP(a, (void *) req, strlen(req)) == -1) 
			{
				return NULL;
			}
		}
	}
	if (index != NULL) 
	{
		fclose(index);
	}
	
	free(req);
	free(buffer);
	closeSocketTCP(a);
		
   return arg0;
}

int main () 
{	
	
	socket_tcp * s = init_socket_tcp();
	
	if (ajoute_ecoute_socket_tcp(s, ADRESSE, PORT) == -1) 
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
		
		pthread_attr_t attr;
		if (pthread_attr_init(&attr) != 0) 
		{
			perror("pthread_attr_init");
		}
		
		if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) 
		{
			perror("pthread_attr_setdetachstate");
		} 
 
		pthread_t th;
	 
		if (pthread_create(&th, &attr,run, a) != 0) 
		{
			perror("pthread_create");
		}
	}
	
	return 0;
}



