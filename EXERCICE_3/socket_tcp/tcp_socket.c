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

//#include "adresse_internet_type.h"
//#include "adresse_internet.h"

#include "tcp_socket.h"

/* Fonction qui alloue et initialise la socket et 
*qui renvoie un pointeur vers la zone mémoire allouée et initialisée.
*Cette fonction retourne l'adresse de la socket en cas de succes et retourne NULL sinon.
*/

socket_tcp *init_socket_tcp (void) {
	socket_tcp *sock_tcp;
	sock_tcp = malloc (sizeof(socket_tcp));
	if (memset(sock_tcp, 0, sizeof(socket_tcp)) == NULL) 
	{
		return NULL;
	}
	(*sock_tcp).sock_tcp = -1;
	
	return sock_tcp;
}


//Fonction de création de la socket

socket_tcp *creerSocketTCP () {
	socket_tcp *sock = init_socket_tcp ();
	if (sock == NULL) return NULL;
	if ((sock->sock_tcp = socket (AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		return NULL;
	}	
	return sock;
}


int connectSocketTCP (socket_tcp *sock_tcp, const char *adresse, uint16_t port) 
{
	if ((sock_tcp->sock_tcp = socket (AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		return -1;
	}
	struct sockaddr_in in;
	in.sin_port = port;
	in.sin_family = AF_INET;
	in.sin_addr.s_addr = inet_addr(adresse);
	if (sockaddr_to_adresse_internet((struct sockaddr *) &in, sock_tcp->distant) != 0) return -1; 
	if (connect (sock_tcp->sock_tcp, (struct sockaddr *) &in, sizeof(in)) == -1) 
	{
		return -1;
	}
	sock_tcp->isConnected = 1;
	return 0;
}


/*Fonction qui permet de créer une socket d’écoute et de l’attacher à l’adresse et au port donnés en paramètres. 
*Si adresse est NULL, la socket s écoute sur toutes les interfaces.
*/

int ajoute_ecoute_socket_tcp(socket_tcp *sock_tcp, const char *adresse, uint16_t port)
{
	if ((sock_tcp->sock_tcp = socket (AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		return -1;
	}
	if (adresse != NULL)
	{ 
		(*sock_tcp).local = adresse_internet_new(adresse, port);
	}
	else 
	{
		(*sock_tcp).local = adresse_internet_any(port);
	}
	
	struct sockaddr_in addresse;
	addresse.sin_family = AF_INET;
	addresse.sin_port = htons((uint16_t) atoi(sock_tcp->local->service));
	addresse.sin_addr.s_addr = inet_addr(sock_tcp->local->nom);
	
	if( bind((*sock_tcp).sock_tcp, (struct sockaddr *) &addresse, sizeof(struct sockaddr)) == -1) { perror("bind"); return -1; }
	(*sock_tcp).isBind = 1;
	if (listen( (*sock_tcp).sock_tcp, BACKLOG) == -1) { perror("listen"); return -1; }
	(*sock_tcp).isListen = 1;
	return 0;
}

int acceptSocketTCP (socket_tcp *socket, socket_tcp * serv) 
{
	int connexion;
	if ((connexion = accept (socket->sock_tcp, NULL, NULL)) == -1) 
	{
		perror("accept");
		return -1;
	}
	(*serv).sock_tcp = connexion;
	serv->isConnected = 1;
	return 0;
}

/*Fonction qui écrit sur la socket, un bloc d’octets buffer de taille length 
*et retourne la taille des données écrites.
*/

int writeSocketTCP(const socket_tcp * socket, const void * buffer, size_t length) 
{
	int i = (int) write( (*socket).sock_tcp, buffer, length);
	if (i == -1) perror("write");
	return i;
}


/*Fonction qui lit sur la socket un bloc d’octets de taille au plus length dans buffer et retourne 
*la taille des données réellement lues.
*/

int readSocketTCP(const socket_tcp * socket, void * buffer, size_t length) 
{
	int i = (int) read( (*socket).sock_tcp, buffer, length);
	if (i == -1) perror("read");
	return i;
}


/*Fonction qui ferme la connexion dans les deux sens et libère l’espace éventuellement alloué par la socket_tcp */
int closeSocketTCP(socket_tcp * socket) 
{
	if (close((*socket).sock_tcp) == -1) 
	{
		return -1;
	}
	free(socket);
	return 0;
}
