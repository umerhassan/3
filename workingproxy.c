/*
 * ServerSocket.c
 *
 *  Created on: Sep 13, 2018
 *      Author: reza
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <string.h>

#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>

int main() {

	/* Address initialization */
	struct sockaddr_in server;
	int MYPORTNUM = 12345;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(MYPORTNUM);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* Create the listening socket */
	int lstn_sock;
	lstn_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (lstn_sock < 0) {
		printf("Error in socket() while creating lstn_sock\n");
		exit(-1);
	}
	printf("Socket created successfully.\n");

	/* Bind the socket to address and port */
	int status;
	status = bind(lstn_sock, (struct sockaddr *) &server,
			sizeof(struct sockaddr_in));
	if (status < 0) {
		printf("Error in bind()\n");
		exit(-1);
	}
	printf("Binding completed.\n");

	/* Connect to TCP server */
	status = listen(lstn_sock, 5);
	if (status < 0) {
		printf("Error in listen()\n");
		exit(-1);
	}
	printf("Listening for connection requests...\n");

	/* Main Loop for listening */
	while (1) {

		/* Accept a connection */
		int connected_sock;
		connected_sock = accept(lstn_sock, NULL,
		NULL);
		if (connected_sock < 0) {
			printf("Error in accept()\n");
			exit(-1);
		}
		printf("Connection established.\n");

		

		/* Receive data */
		char rcv_message[1024000];
		//while (1) {
			 int count1 = recv(connected_sock, rcv_message, sizeof(rcv_message), 0);
			if (count1 < 0) {
				printf("Error in recv()\n");
			} else {
				printf("Client said: %s\n", rcv_message);
				char* hostname = strstr(rcv_message,"Host");
				
				char firstline[1024];
				for(int i = 0, j=6;hostname[j]!='\n';i++,j++){
					firstline[i]=hostname[j];
				}
				
				//printf("Hostname: %s\n: ",firstline);
				
				
				struct sockaddr_in server_addr;
				int MYPORTNUMFORWEBSERVER = 80;
				memset(&server_addr, 0, sizeof(server_addr));
				server_addr.sin_family = AF_INET;
				server_addr.sin_port = htons(MYPORTNUMFORWEBSERVER);
				struct hostent* address;
				char* abc = "pages.cpsc.ucalgary.ca";
								//printf("xyz %s abc",firstline); NOT SHOWING

				address = gethostbyname(abc);
				//printf("NAME: %s\n",address->h_name);
				
				
				bcopy((char *) address->h_addr, (char *)&server_addr.sin_addr.s_addr, address->h_length);
						//printf("Address is %s\n",inet_ntoa(server_addr.sin_addr));
								
				
						/* Create the listening socket */
				int c_sock;
				c_sock = socket(AF_INET, SOCK_STREAM, 0);
				if (c_sock < 0) {
					printf("Error in c_socket() while creating lstn_sock\n");
					exit(-1);
				}
				//else { printf("********************************\n"); }

					/* Connect to TCP server */
				int status1;
				status1 = connect(c_sock, (struct sockaddr *) &server_addr,
					sizeof(struct sockaddr_in));
				if (status < 0) {
					printf("Error in connect()\n");
					exit(-1);
				} else {
					printf("Connected.\n");
				}

				/* Send data*/
				int count5;
				char message[102400];
				
				count5 = send(c_sock, rcv_message, sizeof(rcv_message), 0);
				if (count5 < 0) {
					printf("Error in send()\n");
				}
				else { printf("Data sent()\n"); }
				
				char rcv_message2[1024000];
				count5 = recv(c_sock, rcv_message2, sizeof(rcv_message2), 0);
				if (count5 < 0) {
					printf("Error in recv()\n");
				} else {
					printf(" Length : %d Message recieved: %s\n", strlen(rcv_message2), rcv_message2);
				}
				if(strlen(rcv_message2) > 300 ) {
				int num = (rand() % (strlen(rcv_message2) - 300 + 1)) + 300; 
				printf("Rand number is: %d\n", num );
				rcv_message2[num] = 'X';
				}
				
				int count7;
				count7 = send(connected_sock, rcv_message2, sizeof(rcv_message2), 0);
				if (count7 < 0) {
						printf("Error in send()\n");
				}
		
		//	}

		}
				

	}
	/* Close the socket */
	close(lstn_sock);
	return 0;
}
