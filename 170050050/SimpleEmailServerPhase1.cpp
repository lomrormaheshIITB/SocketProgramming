#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <dirent.h>
#include <iterator>
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
	istringstream isstring(s);
	string user , pass , username, password;
	if(!(isstring >> user >> username >> pass >> password)){
		if(user != "User:" or pass != "Pass:") return 0;
		else return 1;
	}
}


map<string, string> GetUserName(char * file){
	map<string, string> user_data;
	string line;
	vector <string> results;

  	ifstream myfile(file);

  	if (myfile.is_open())
  	{
    	while(getline(myfile,line)){
    		istringstream isstring(line);

    		string username, password;
    		if(isstring >> username >> password){
			user_data.insert(pair<string, string>(username, password ));
			getline (myfile,line);
    		}
    	}
    	myfile.close();
	}    	
	return user_data;
}


string GetUserFromClient(string line){
	istringstream isstring(line);
	string user , username, pass, password;
	isstring >> user >> username >> pass >> password;
	return username;
}

string GetPassFromClient(string line){
	istringstream isstring(line);
	string user , username, pass, password;
	isstring >> user >> username >> pass >> password;
	return password;
}

string Receiving(int client_id){
	char buffer[1024];
	int r = recv(client_id, buffer, 1024 ,0 );
	return string(buffer);
}

////////-------------------------------------------------------;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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
	int client_id = accept(server_socket, (struct sockaddr *) &peer_addr, &addr_size);
	char ip[INET_ADDRSTRLEN]; 														//INET_ADDRSTRLEN
    inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN);   //inet_ntop 
	if(client_id != -1) cout <<"Client: " << ip << ":"<< ntohs(peer_addr.sin_port) << endl;

//------------------------------------------------------------------------------------------------------
	// username , password  from password.txt file
	map<string , string> user_data = GetUserName(argv[2]);
  	//--------------------------------------------------------------------------------------------------- part c ----------------------------------------------------
  	// received from client
  	string user_rec;
  	string pass_rec;
    string buffer = Receiving(client_id);
	  	
    char char_buffer[1024];
	strcpy(char_buffer, buffer.c_str());

    // unable to parse string
  	if(! Parsable(buffer)) { printf("Unknown Command\n"); close(client_id); }
  	else {
  		user_rec = GetUserFromClient(buffer);
		pass_rec = GetPassFromClient(buffer);
  	// invalid user
  		if(user_data.find(user_rec) == user_data.end()) { printf("Invalid User\n"); close(client_id);}
	  	//invalid password
	  	else if(pass_rec != user_data.find(user_rec)->second) { printf("Wrong Passwd\n"); close(client_id);}
	  	//successful login new user
	  	else { 
	  		string ss = "Welcome " + user_rec;
	  		char char_array[1024];
	  		strcpy(char_array, ss.c_str());
	  		cout << char_array << endl;
	  		send(client_id, char_array, sizeof(char_array),0);


			string t2 = Receiving(client_id);

  			if(t2 != "quit") { printf("Unknown Command\n"); close(client_id);}
  			else if(t2 == "quit") {cout << "Bye " << user_rec << endl; close(client_id);}
	  	}
  	}
  		

//---------------------------------------------------------------------------------------------- part d ------------------------------------------------------------
  	// quit user

  	close(server_socket);
	return 0;
}