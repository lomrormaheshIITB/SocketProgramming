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

#include <sstream>
#include <dirent.h>
 
#include <sys/un.h> 
#include <string.h>
#include <map>
#include<vector>
#include <boost/algorithm/string.hpp> 

#define PORT 5000;


int main(int argc, char *argv[]){
	if(argc != 4) { cout << "Usage : [filename] [IP:portNum] [user-name] [password]\n";    exit(1);}

	string ip_port = string(argv[1]); 

	string delimiter = ":";
	int w1 = ip_port.find(delimiter);

	string ip_ad = ip_port.substr(0, w1);
	string po_no = ip_port.substr(w1+1,ip_port.length()-1);

	int portNum = stoi(po_no);

	string username = argv[2];
	string password = argv[3];

	// create a socket   ....
	int network_socket;
	network_socket = socket(PF_INET, SOCK_STREAM, 0);

	// specify an address to the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portNum);
	inet_aton(ip_ad.c_str(), &server_address.sin_addr);			


//----------------------------------------------------------------------------------part b -------------------------------------------------

	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));  // 0 = okay , -1 = error
	// check for errror in the connection
	if(connection_status == -1){
		cout << "There was some an error making a connection with the server\n";  exit(2);
	} 

	char ip[INET_ADDRSTRLEN]; 														//INET_ADDRSTRLEN
    inet_ntop(AF_INET, &(server_address.sin_addr), ip, INET_ADDRSTRLEN);

	if(connection_status == 0){
		cout <<"ConnectDone: " << ip << ":" << ntohs(server_address.sin_port) << endl;
	}
//-------------------------------------------------------------------------------------- part c -----------------------------------------------
	//Send null terminated string “User: user-name Pass: passwd” over the TCP connection.
	string ts = "User: " +  string(argv[2]) + " Pass: " + string(argv[3]); 
	char  tosend[1024];
	strcpy(tosend, ts.c_str());
	int sent = send(network_socket ,tosend, sizeof(tosend) , 0);


	//receive data from the server
	char server_response[1024];
	int receive_num = recv(network_socket, &server_response, sizeof(server_response), 0);
	//print out the server's response
	if(receive_num > 0){ cout << string(server_response) << endl; }
	

//----------------------------------------------------LIST --------------------------------------------------------

	char  tosend1[1024] = "LIST\0";
	sent = send(network_socket ,tosend1, sizeof(tosend1) , 0);

	char server_response3[1024];
	int receive_num2 = recv(network_socket, server_response3, sizeof(server_response3) , 0);
	if(receive_num2 > 0) { cout << server_response3 << endl;}

//----------------------------------------------------------------------------------part d -------------------------------------------------
	// null-terminated string "quit"
	char  tosend2[1024] = "quit\0";
	send(network_socket ,tosend2, sizeof(tosend2) , 0);
//----------------------------------------------------------------------------------part e -------------------------------------------------

	//client exit
	close(network_socket);

	return 0;


}