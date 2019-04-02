#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
using namespace std;

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h> 
#include <netdb.h>
 
#include <sys/un.h> 
#include <string.h>
#include <map>
#include<vector>
#include <boost/algorithm/string.hpp> 
#include <arpa/inet.h> 

int Parsable(string s){
	string user , pass , username, password;
	if(!(isstring >> user >> username >> pass >> password)){
		if(user != "User:" or pass != "Pass:" or username = " " or password = " ") return 0;
		else return 1;
	}
}

int main(int argc, char *argv[])
{

	// if wrong number of command line arguments are given
	if(argc != 3) { cout << "Usage : [filename] [portNum] [passwordfile]\n";    exit(1);}

	int portNum = std::stoi(argv[1]) ;

	int server_socket;
	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	
	//define the server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portNum);
	server_address.sin_addr.s_addr = INADDR_ANY;

	//bind the server to our specified IP and port 
	int bind_num = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if(bind_num == -1) { perror("Binding Fails\n Reason "); exit(2);}

	// passwrd file doesn't exist or not reable
	ifstream my_file(argv[2]);
	if(! (bool)my_file ) { cout << "File " << argv[2] << " doesn't  exist\n";  exit(3);}
	else if(!my_file.good()) { cout << "File " <<  argv[2] <<" is not readable\n";  exit(3);}

//--------------------------------------------------------------------------------------------------  part b -------------------------------------------------------

	//STDOUT on successful bind “BindDone: portNum\n”
	if(bind_num == 0) printf("BindDone: %d\n", portNum);
	

	//STDOUT on successful listen “ListenDone: portNum\n”
	int listen_num = listen(server_socket, 1);
	if(listen_num == 0) printf("ListenDone: %d\n", portNum);

	//STDOUT on incoming connection “Client: ipaddr:port\n” where ipaddr and port are the client-side info 
	struct sockaddr_in  peer_addr;
	socklen_t addr_size; 
    addr_size = sizeof(struct sockaddr_in);

    //accept
	int client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &addr_size);
	char ip[INET_ADDRSTRLEN]; 														//INET_ADDRSTRLEN
    inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN);   //inet_ntop 
	if(client_socket != -1) printf("Client: %s : %d\n", ip, ntohs(peer_addr.sin_port));

//------------------------------------------------------------------------------------------------------
	// username , password  from password.txt file
	map<string , string> user_data;
	string line;
  	ifstream myfile(argv[2]);
  	std::vector<std::string> results;
  	if (myfile.is_open())
  	{
    	std::string delimiter1 = " ";
    	std::string delimiter2 = ":";
    	while(getline (myfile,line)){
    		int w1 = line.find(delimiter1);
    		int w2 = line.find(delimiter2);
    		string token = line.substr(w1+1, w2-w1-1);
			string token2 = line.substr(w2+1, line.length() - 1);
			user_data.insert(pair<string, string>(token, token2));
			getline (myfile,line);
    	}
    	myfile.close();
  	}

  	//--------------------------------------------------------------------------------------------------- part c ----------------------------------------------------
  	// received from client
  	string user_rec, pass_rec;
  	std::vector<std::string> resp;
  	char  client_response[1024];
  	int received = recv(client_socket, &client_response, sizeof(client_response), 0);
  	line  = string(client_response);                 // "User: Mh Pass: 21drrz";
 	std::string delimiter0 = " "; 
  	std::string delimiter1 = " P";
    std::string delimiter2 = "s: ";
    int w0 = line.find(delimiter0);
    int w1 = line.find(delimiter1);
    int w2 = line.find(delimiter2);
    user_rec = line.substr(w0+1,w1-w0-1);
    pass_rec = line.substr(w2+3,line.length()-1);


    // unable to parse string
  	if(! Parsable(string(client_response2))) { printf("Unknown Command\n"); close(client_socket); }
  	// invalid user
  		else if(user_data.find(user_rec) == user_data.end()) { printf("Invalid User\n"); close(client_socket);}
	  	//invalid password
	  	else if(pass_rec != user_data.find(user_rec)->second) { printf("Wrong Passwd\n"); close(client_socket);}
	  	//successful login new user
	  	else { 
	  		string ss = "Welcome " + user_rec;
	  		char char_array[1024];
	  		strcpy(char_array, ss.c_str());
	  		cout << char_array << endl;
	  		send(client_socket, char_array, sizeof(char_array),0);
	  	}

//---------------------------------------------------------------------------------------------- part d ------------------------------------------------------------
  	// quit user
	char  client_response2[1024];
  	int received2 = recv(client_socket, &client_response2, sizeof(client_response2), 0);


  	if(! Parsable(string(client_response2))) { printf("Unknown Command\n"); close(client_socket);}
  	else if(string(client_response2) == "quit") {cout << "Bye " << user_rec << endl; close(client_socket);}

	close(server_socket);
	return 0;
}