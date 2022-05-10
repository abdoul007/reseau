#define BACKLOG 20
#include "../adresse_internet/adresse_internet_type.h"
#include "../adresse_internet/adresse_internet.h"

typedef struct
{
	int sock_tcp;
	adresse_internet *local;
	adresse_internet *distant;
	int isBind;
	int isListen;
	int isConnected;
} socket_tcp;


//Entête des fonctions
socket_tcp *init_socket_tcp (void); 
socket_tcp *creerSocketTCP ();


int connectSocketTCP (socket_tcp *sock_tcp, const char *adresse, uint16_t port); 
int ajoute_ecoute_socket_tcp(socket_tcp *sock_tcp, const char *adresse, uint16_t port);
int acceptSocketTCP (socket_tcp *socket, socket_tcp * serv);
int writeSocketTCP(const socket_tcp * socket, const void * buffer, size_t length);  
int readSocketTCP(const socket_tcp * socket, void * buffer, size_t length);
int closeSocketTCP(socket_tcp * socket) ; 
