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
#include <netdb.h>
#include "adresse_internet_type.h"

adresse_internet * adresse_internet_new_aux (int flag, const char * nom, uint16_t port) 
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = flag | AI_NUMERICSERV;
	struct addrinfo * res;
	char cport[_SERVICE_NAME_MAX_SIZE];
	snprintf(cport, _SERVICE_NAME_MAX_SIZE, "%d", port);
	
	if (getaddrinfo(nom, cport, &hints, &res) != 0) 
	{
		perror("getaddrinfo");
		return NULL;
	}
		
	adresse_internet * ai = malloc(sizeof(adresse_internet));
	
	if (getnameinfo(res->ai_addr, res->ai_addrlen, ai->nom, _DNS_NAME_MAX_SIZE, ai->service, _SERVICE_NAME_MAX_SIZE, NI_NUMERICHOST | NI_NUMERICSERV) != 0) 
	{
		freeaddrinfo(res);
		perror("getnameinfo");
		return NULL;
	}
	if (memcpy(&ai->sock_addr, res->ai_addr, sizeof(struct sockaddr_storage)) == NULL) 
	{
		freeaddrinfo(res);
		perror("memcpy");
		
		return NULL;
	}
	
	return ai;
}

adresse_internet * adresse_internet_new (const char * nom, uint16_t port) 
{
	return adresse_internet_new_aux(0, nom, port);
}

adresse_internet * adresse_internet_any(uint16_t port) 
{
	return adresse_internet_new_aux(AI_PASSIVE, NULL, port);
}

adresse_internet * adresse_internet_loopback(uint16_t port) 
{
	return adresse_internet_new_aux(0, NULL, port);
}

void adresse_internet_free(adresse_internet *adresse) 
{
	free(adresse);
}

uint16_t adresse_internet_get_port(const adresse_internet * adresse) 
{
	if (adresse->service == NULL || adresse->nom == NULL) 
	{
		return 0;
	}
	
	return (uint16_t) atoi(adresse->service);
}

int adresse_internet_get_domain(const adresse_internet * adresse) 
{
	if (adresse == NULL) 
	{
		return -1;
	}
	switch ( (*adresse).sock_addr.ss_family) 
	{
		case AF_INET:
			return AF_INET;
		case AF_INET6:
			return AF_INET6;
		default:
			return -1;			
	}
}

int sockaddr_to_adresse_internet (const struct sockaddr *addr, adresse_internet *adresse) 
{
	if (addr == NULL)
	{
		return -1;
	}
	
	adresse = malloc(sizeof(adresse_internet));
	
	if (memcpy(&adresse->sock_addr, addr, sizeof(struct sockaddr_storage)) == NULL)
	{
		return -1;
	}
	
	if (strncpy((*adresse).nom, (*addr).sa_data, _DNS_NAME_MAX_SIZE) == NULL) 
	{
		return -1;
	}
	
	return 0;
}

int adresse_internet_to_sockaddr(const adresse_internet *adresse, struct sockaddr *addr) 
{
	if (adresse == NULL)
	{
		return -1;
	}
	
	addr = malloc(sizeof(struct sockaddr));
	
	if (memcpy(addr, &adresse->sock_addr, sizeof(struct sockaddr)) == NULL) 
	{
		return -1;
	}
	if (strncpy((*addr).sa_data, adresse->nom, 14) == NULL) 
	{
		return -1;
	}
	
	return 0;
}

int adresse_internet_get_info(adresse_internet * adresse, char * nom_dns, int taille_dns, char * nom_port, int taille_port) 
{
	if (nom_dns == NULL && nom_port == NULL) 
	{
		return -1;
	}
	
	strncpy(nom_dns, adresse->nom, (size_t) taille_dns);
	strncpy(nom_port, adresse->service, (size_t) taille_port);
	
	return 0;
}

int adresse_internet_get_ip (const adresse_internet *adresse, char *ip, int taille_ip) 
{
	if (adresse == NULL) 
	{
		return -1;
	}
	
	if (strncpy(ip, (*adresse).nom, (size_t) taille_ip) == NULL) 
	{
		return -1;
	}
	
	return 0;
}

int adresse_internet_compare(const adresse_internet *adresse11, const adresse_internet *adresse22) 
{
	return !strcmp(adresse11->nom, adresse22->nom) && !strcmp(adresse11->service, adresse22->service) && !memcmp(&adresse11->sock_addr, &adresse22->sock_addr, sizeof(struct sockaddr_storage));
}

int adresse_internet_copy (adresse_internet * adrdst, const adresse_internet * adrsrc) 
{
	adrdst = malloc(sizeof(adresse_internet));
	
	if (memcpy(adrdst, adrsrc, sizeof(adresse_internet)) == NULL) 
	{
		return -1;
	}
	
	return 0;	
}

