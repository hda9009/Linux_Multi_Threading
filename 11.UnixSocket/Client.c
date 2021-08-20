/*
 * 2.UNIX_Client.c
 *
 *  Created on: Aug 20, 2021
 *      Author: hda
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define SOCKET_PATH 	"SocketFile"

int main()
{
	int clientNetwork = -1;
	char readBuffer[128];
	char writeBuffer[128] = "Hello! from the Client";

	if ( (clientNetwork = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 )
	{
		perror("Client Socket Cannot be Made\n");
		exit (EXIT_FAILURE);
	}
	printf("Socket Server is made Successfully\n");

	struct sockaddr_un clientBind;
	clientBind.sun_family = AF_UNIX;
	strcpy (clientBind.sun_path, SOCKET_PATH);
	if ( (connect(clientNetwork, (struct sockaddr*) &clientBind, sizeof (clientBind) ) ) == -1)
	{
		perror("Client Connection Cannot be Established\n");
		exit (EXIT_FAILURE);
	}
	printf("Client connection is successfully established\n");

	if ( write(clientNetwork, writeBuffer, strlen (writeBuffer)) == -1 )
	{
		perror("Write Error\n");
		exit (EXIT_FAILURE);
	}

	read(clientNetwork, &readBuffer, sizeof (readBuffer));
	printf("Data Received: %s\n", readBuffer);

	close(clientNetwork);


}

