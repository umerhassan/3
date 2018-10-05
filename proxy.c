/*
 * 	ServerSocket.c
 *
 *  Created on: Sep 13, 2018
 *  Author: Hassan and Reza
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
#define NUMBEROFERRORS 3 // Number of Errors you want on webpage
#define MYPORTNUMFORWEBSERVER 80 //port for proxy to talk to webserver
#define MYPORTNUM 12345 // port for webbrowser to talk to proxy
int main() {

	/* Address initialization serverside*/
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server)); //resetting the address to zero in server
	server.sin_family = AF_INET;
	server.sin_port = htons(MYPORTNUM);  //convert 16-bit host-byte-order (little endian) to network-byte-order (big endian)
	server.sin_addr.s_addr = htonl(INADDR_ANY); //assigning any ip address on local machine
	
	/* Create the listening socket */
	int lstn_sock;
	lstn_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (lstn_sock < 0) {
		printf("Error in socket() while creating lstn_sock\n");
		exit(-1);
	}
	else {
		printf("Socket created successfully.\n");
	}

	/* Bind the socket to address and port */
	int status;
	status = bind(lstn_sock, (struct sockaddr *) &server, sizeof(struct sockaddr_in));
	if (status < 0) {
		printf("Error in bind()\n");
		exit(-1);
	}
	else {
		printf("Binding completed.\n");
	}

	/* Connect to TCP server */
	status = listen(lstn_sock, 5);
	if (status < 0) {
		printf("Error in listen()\n");
		exit(-1);
	}
	else {
		printf("Listening for connection requests...\n");
	}

	/* Main Loop for listening */
	while (1) {

		/* Accept a connection */
		int connected_sock;
		connected_sock = accept(lstn_sock, NULL, NULL);
		if (connected_sock < 0) {
			printf("Error in accept()\n");
			exit(-1);
		}
		printf("Connection established.\n");

		

		/* Receive data from the web browser*/
				char rcv_message[1024000];
		//while (1) {
			int count1 = recv(connected_sock, rcv_message, sizeof(rcv_message), 0); // store the message recieved in rcv_message
			if (count1 < 0) {
				printf("Error in recv()\n");
			} else {
				printf("Client said: %s\n", rcv_message); //msg from webbrowser (its just the header)
				char* hostname = strstr(rcv_message,"Host: "); //Extract the line that says Host: pages.cpsc.ucalgary.ca 
			
				char firstline[1024] = "abc"; //initialization of firstline
				int count = 0;
				for(int i = 0, j=6;(hostname[j]!='\r' || count == 2);i++,j++) {  //getting hostname till /r
					firstline[i]=hostname[j];
					if(hostname[j]==':') count++; //checking if there is a port number: eg Host: pages.ucalgary.ca:441
				}
					
				
				/* Address initialization clientside*/
				struct sockaddr_in server_addr;
				memset(&server_addr, 0, sizeof(server_addr));
				server_addr.sin_family = AF_INET;
				server_addr.sin_port = htons(MYPORTNUMFORWEBSERVER); //connecting on port 80
				struct hostent* address;
					
				//	for(int i = 0; i<strlen(firstline); i++) printf("firstline[%d] : %c\n",i,firstline[i]); for debugging
				address = gethostbyname(firstline); // converting hostname to ip address
				bcopy((char *) address->h_addr, (char *)&server_addr.sin_addr.s_addr, address->h_length); // copying address to server_addr
								
				
				/* Create the listening socket  */
				int c_sock;
				c_sock = socket(AF_INET, SOCK_STREAM, 0);
				if (c_sock < 0) {
					printf("Error in c_socket() while creating lstn_sock\n");
					exit(-1);
				}

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

				/* Send data - sending the header data to webserver*/
				int count5;
				char message[102400];
				
				count5 = send(c_sock, rcv_message, sizeof(rcv_message), 0);
				if (count5 < 0) {
					printf("Error in send()\n");
				}
				else { printf("Data sent()\n"); }
				
				/*recieve stuff from web server GET RESPONSE */
				char rcv_message2[1024000];
				count5 = recv(c_sock, rcv_message2, sizeof(rcv_message2), 0);
				if (count5 < 0) {
					printf("Error in recv()\n");
				} else {
					printf(" Message recieved: %s\n", rcv_message2);
				}
				
				/* GET CONTENT LENGTH */
				char* len = strstr(rcv_message2,"Content-Length: ");
				char len2[100];
				if(len != NULL)
					for(int i = 15, j=0; i<strlen(len); i++, j++) 
						len2[j] = len[i];
				int abc1 = atoi(len2); //converting string to a number
				
				// for debuggingprintf("********************Length is: %d\n", abc1 );
				
				// ONLY proceed if the message is 200 OK
				if(strstr(rcv_message2, "200 OK") != NULL){
					int xyz = strlen(rcv_message2) - abc1 + 4 ; //total length - content length  gives you start of the body
					if(strlen(rcv_message2) > xyz ){
						for(int i = 0; i< NUMBEROFERRORS ; i++) {
							
							/* This generates a random index from start of the body to end index which is strlen(rcv_message2) */
							int num = (rand() % (strlen(rcv_message2) - xyz + 1)) + xyz; 
							//for debugging printf("Rand number is: %d\n", num );
							
							// Don't change these symbols IF it's html file
							if((rcv_message2[num] == '<' || rcv_message2[num] == '>' || rcv_message2[num] == 'p' || rcv_message2[num] == 'b') && (strstr(rcv_message2, "HTML") != NULL)){
								num = num + 6;
								rcv_message2[num] = 'X';
							}
							else {
								rcv_message2[num] = 'X'; //replace index data with X, can be any random number here but X is easier to find
							}
						
						}
					}
				}
				/* Send changed Data back to browser*/
				int count7;
				count7 = send(connected_sock, rcv_message2, sizeof(rcv_message2), 0);
				if (count7 < 0) {
						printf("Error in send()\n");
				}
				
						close(c_sock);		

		//	}

		}
		
		close(connected_sock);
	}
	
	
	/* Close the socket */
	
	close(lstn_sock);
	return 0;
}
