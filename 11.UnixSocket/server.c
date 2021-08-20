/*
 ============================================================================
 Name        : Socket.c
 Author      : HDA
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
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

int main(void) {
	int serverNetwork = -1, clientNetwork = -1;
	char readBuffer[128];
	char writeBuffer[128] = "Message has been Received successfully";

	unlink (SOCKET_PATH);
	serverNetwork = socket(AF_UNIX, SOCK_STREAM, 0);
	if (serverNetwork == -1)
	{
		perror("Server Socket Cannot be Made\n");
		exit (EXIT_FAILURE);
	}
	printf("Socket Server is made Successfully\n");

	struct sockaddr_un serverBind, clientDetails;
	serverBind.sun_family = AF_UNIX;
	strcpy (serverBind.sun_path, SOCKET_PATH);
	if (bind(serverNetwork, (const struct sockaddr*)&serverBind, sizeof (serverBind) ) == -1)
	{
		perror("Binding cannot be done\n");
		exit (EXIT_FAILURE);
	}
	printf("Binding is done Successfully\n");


	if (listen(serverNetwork, 20) == -1)
	{
		perror("Listening cannot be done\n");
		exit (EXIT_FAILURE);
	}
	printf("Listening is done Successfully\n");


	while (1)
	{
		clientNetwork = accept(serverNetwork, (struct sockaddr *)&clientDetails, sizeof (clientDetails));
		if (clientNetwork == -1)
		{
			perror("Binding cannot be done\n");
					exit (EXIT_FAILURE);
		}
		printf("Client is connected Successfully\n");

		read(clientNetwork, &readBuffer, sizeof (readBuffer));
		printf("Message Received: %s\n", readBuffer);
		write ( clientNetwork, writeBuffer, sizeof (writeBuffer) ) ;

		close (clientNetwork);
		printf("Client Disconnected\n\n");

	}
	close (serverNetwork);
	return EXIT_SUCCESS;
}

