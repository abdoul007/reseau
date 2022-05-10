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
    
	char cmd [100];
	for (size_t i = 0; i < strlen(buffer) && buffer[i] != ' '; ++i) 
	{
		cmd[i] = buffer[i];
	}
	
	
	/****CAS ERREUR 501: SI LA METHOD N'EST PAS IMPLEMENTE ***/
	
	if (strcmp(cmd, "GET") != 0) 
	{
		if (writeSocketTCP(a, (void *) ERROR_501, strlen(ERROR_501)) == -1) 
		{
			return NULL;
		}
		
		FILE * rep_501 = fopen("501.html", "r");
		
		if (rep_501 != NULL) 
		{
			while (fgets(req, BUF_SIZE, rep_501) != NULL) 
			{
				if (writeSocketTCP(a, (void *) req, strlen(req)) == -1) 
				{
					return NULL;
				}
			}
		}
		fclose(rep_501);
	}
	
	
	/**********CAS ERREUR 404: SI LA PAGE DEMANDE N'EXISTE PAS  ET 400: SI LA REQUETE EST MAL FORMULEE**********/
	
	char page [100];
	size_t deb = strlen(cmd) + 2;
	for (size_t i = deb; i < strlen(buffer) && buffer[i] != ' '; ++i) 
	{
		page[i-deb] = buffer[i];
	}
	
	FILE * index = fopen(page, "r");
	FILE * rep_404 = fopen("404.html", "r");
	FILE * rep_400 = fopen("400.html", "r");
	
	if (index == NULL) 
	{
		if (writeSocketTCP(a, (void *) ERROR_404, strlen(ERROR_404)) == -1) 
		{
			return NULL;
		}

		if (rep_404 != NULL) 
		{
			while (fgets(req, BUF_SIZE, rep_404) != NULL) 
			{
				if (writeSocketTCP(a, (void *) req, strlen(req)) == -1) 
				{
					return NULL;
				}
			}
			fclose(rep_404);
		}
	}
	else if (strchr(page, '.') == NULL || strcmp(strchr(page, '.'), ".html") != 0) 
	{
		if (writeSocketTCP(a, (void *) ERROR_400, strlen(ERROR_400)) == -1) 
		{
			return NULL;
		}

		
		if (rep_400 != NULL) 
		{
			while (fgets(req, BUF_SIZE, rep_400) != NULL) 
			{
				if (writeSocketTCP(a, (void *) req, strlen(req)) == -1) 
				{
					return NULL;
				}
			}
			fclose(rep_400);
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



