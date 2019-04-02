#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h> 

int main(int argc, char *argv[])
{					// -------------------------------------------------------------------------------- part a -------------------------------------------------
	if(argc != 4) { cout << "Usage : [filename] [portNum] [user-name] [passwordfile]\n";    exit(1);}

	int portNum = std::stoi(argv[1]);
	string username = argv[2];
	string password = argv[3];

	// create a socket   ....       socket function
	int network_socket;
	network_socket = socket(PF_INET, SOCK_STREAM, 0);

	// specify an address to the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portNum);                  
	server_address.sin_addr.s_addr = INADDR_ANY;				


//----------------------------------------------------------------------------------part b -------------------------------------------------

	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));  // 0 = okay , -1 = error
	// check for errror in the connection
	if(connection_status == -1){
		cout << "There was some an error making a connection with the server\n";  exit(2);
	} 

	char ip[INET_ADDRSTRLEN]; 														//INET_ADDRSTRLEN
    inet_ntop(AF_INET, &(server_address.sin_addr), ip, INET_ADDRSTRLEN);

	if(connection_status == 0){
		printf("ConnectDone: %s : %d\n",ip, ntohs(server_address.sin_port));
	}

//-------------------------------------------------------------------------------------- part c -----------------------------------------------
	//Send null terminated string “User: user-name Pass: passwd” over the TCP connection.
	char  tosend[1024] = "User: Mh Pass: 21drrz";
	int sent = send(network_socket ,tosend, sizeof(tosend) , 0);


	//receive data from the server
	char server_response[1024];
	int receive_num = recv(network_socket, &server_response, sizeof(server_response), 0);
	//print out the server's response
	cout << string(server_response) << endl;


//----------------------------------------------------------------------------------part d -------------------------------------------------
	// null-terminated string "quit"
	char  tosend2[1024] = "quit";
	sent = send(network_socket ,tosend2, sizeof(tosend2) , 0);


//----------------------------------------------------------------------------------part e -------------------------------------------------

	//client exit
	close(network_socket);

	return 0;
}