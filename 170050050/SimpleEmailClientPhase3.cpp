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
#include <list>
#include <sys/stat.h>
#include <ctype.h>
#include <typeinfo>

int LENGTH = 1024;

int DirAccess(char *directory){
	DIR *dir = opendir(directory); // open dir d

	if(! dir){
		cout << "Directory " << directory << " doesn't exist\n"; 
		exit (4);
	}
	return 1;
}

int List_Parse(string mylist){
	int list_size = mylist.size();
	for(int i=0; i< list_size; i++){
		 if(! isdigit(mylist[i]))  return 0;
		 i++;
	}
	return 1;
}

void Receive_File(int network_socket, char * folder){

	// reveive packet detail
	char  packet_detail[1024];
	recv(network_socket,packet_detail, sizeof(packet_detail),0);
	string ss = string(packet_detail);
	string full_pack ,partial_size,  name;
	istringstream iss(ss);
	vector<string> results((istream_iterator<string>(iss)),
                                 istream_iterator<string>());
	vector<string>::iterator it=results.begin();
	full_pack = *it;
	it++;
	partial_size = *it;
	++it;
	name = *it;

	//create folder and file
	int stat;
	int access = DirAccess(folder);
	if(access) { rmdir(folder);  stat = mkdir(folder, 0777);} 
	else {
		printf( "cannot access %s\n", folder );  exit(4);
	}

  	if(! stat) { cout << "Can't create folder\n";  exit(4);} 


   	FILE* pFile;
   	string filepath = string(folder) + "/" + name;
    pFile = fopen(filepath.c_str() , "wb");

	char buffer[1024];
	for(int i=0; i < stoi(full_pack); i++){
		recv(network_socket, buffer, sizeof(buffer),0);
		fwrite(buffer, 1, 1024, pFile);
		memset(buffer, 0, sizeof(buffer));
	}
	if(stoi(partial_size)){
		recv(network_socket, buffer, stoi(partial_size),0);
		fwrite(buffer, 1, stoi(partial_size), pFile);
	}
}

int main(int argc, char *argv[]){
	//serverIPAddr:port, user-name, passwd, list-of-messages, local-folder
	if(argc != 6) { cout << "Usage : [filename] [IP:portNum] [user-name] [password] [list-of-messages] [local-folder]\n";    exit(1);}

	//print usage on stderr and exit with exit-code 3, if the list does not parse to a list of numbers.
	if(! List_Parse(argv[4])) { cout << " Can't parse list to a list-of-messages \n";  exit (3); }
	int arguments = string(argv[4]).size()/2 + 1;
	string arg_array[arguments];
	for(int i=0;i<arguments;i++){
		string k(1, argv[4][2*i]);
		arg_array[i] = k;
	}

	// create delete local folder
	
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

//--------------------------------------------------RETRV M ---------------------------------------------------------

	// no of arguments
	string ddd = to_string(arguments);
	char args[1024];
	strcpy(args, ddd.c_str());
	send(network_socket, args, sizeof(args), 0);


	char  tosend2[1024];

	for(int i=0; i<arguments; i++){
		string  s = "RETRV " + arg_array[i];
		strcpy(tosend2, s.c_str());
		sent = send(network_socket ,tosend2, sizeof(tosend2) , 0);
		Receive_File(network_socket, argv[5]);
	}


	/*char server_response4[1024];
	int receive_num4 = recv(network_socket, server_response4, sizeof(server_response4) , 0);
	if(receive_num4 > 0) { cout << server_response4 << endl;}*/


	Receive_File(network_socket, argv[5]);
//----------------------------------------------------------------------------------part d -------------------------------------------------
	// null-terminated string "quit"
	char  tosend5[1024] = "quit\0";
	send(network_socket ,tosend5, sizeof(tosend5) , 0);
//----------------------------------------------------------------------------------part e -------------------------------------------------

	//client exit
	close(network_socket);

	return 0;	
}