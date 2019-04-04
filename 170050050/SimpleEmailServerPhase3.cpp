#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <cmath>
#include <cstddef>
#include <sstream>
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
#include <iterator>
#include<vector>
#include <boost/algorithm/string.hpp> 
#include <arpa/inet.h>

int LENGTH = 1024;

long GetFileSize(string myfile){
	ifstream file( myfile, ios::binary | ios::ate);
	return file.tellg();
}

void Send_File(string folder_path, string file, int client_id){
	// obtain proper file extension
	// file = file to search (send) [ filename without extension]

	char *bufferi = new char[folder_path.length() + 1];
	strcpy(bufferi, folder_path.c_str());
	DIR *d;
	struct dirent *dir;
	d = opendir(bufferi);
	string dname;

	if (d) {
		while ((dir = readdir(d)) != NULL) { 
			
			dname = dir->d_name;
			string delimeter = ".";
			int w = dname.find(delimeter);
			string name = dname.substr(0,w);

			if(name == file){ 
				break;
			}
		}
		closedir (d);
	}

	// dname = file to send [filenme with extension]  like 3.jpg
	 ifstream is (folder_path + "/" + dname, ifstream::binary);

	 long file_size = GetFileSize(folder_path + "/" + dname);

	 long packetNum = file_size / 1024 ;
	 int remain_length =  file_size - packetNum*1024;

	 // details of data to send
	 string pack_detail = to_string(packetNum) + " " +  to_string(remain_length) +"  " + dname ;
	 char packet_detail[1024];
	 strcpy(packet_detail, pack_detail.c_str());

	 // detail of packets : send to client
	 send(client_id,packet_detail, sizeof(packet_detail),0 );
	 
	 char buffer[1024];

	 // send packets
	 for(int i=0; i< packetNum; i++){
	 	is.read(buffer,LENGTH);
	 	send(client_id, buffer, LENGTH, 0);
	 	memset(buffer, 0, sizeof(buffer));
	 }

	 // extra packet
	 if(remain_length) {
	 	is.read(buffer,LENGTH);
	 	int ss = send(client_id, buffer, remain_length, 0);
	 }
}


//------------------------------------

int Binding(int sock_id, sockaddr_in address, int portNum){
	int bind_num = bind(sock_id, (struct sockaddr *) &address, sizeof(address));
	if(bind_num == -1) { perror("Binding Fails\n Reason "); exit(2);}
	if(bind_num == 0) printf("BindDone: %d\n", portNum);
	return bind_num;
}


void PasswdFileExist(char * file){
	ifstream my_file(file);
	if(! (bool)my_file ) { cout << "File " << file << " doesn't  exist\n";  exit(3);}
	else if(!my_file.good()) { cout << "File " <<  file <<" is not readable\n";  exit(3);}

}


int Listening(int server_socket, int Num_Client, int portNum){
	int listen_num = listen(server_socket, Num_Client);
	if(listen_num == 0) printf("ListenDone: %d\n", portNum);
	return listen_num;
}


int Accepting(int server_socket){
	struct sockaddr_in  peer_addr;
	socklen_t addr_size; 
    addr_size = sizeof(struct sockaddr_in);

	int client_id = accept(server_socket, (struct sockaddr *) &peer_addr, &addr_size);
	char ip[INET_ADDRSTRLEN]; 														//INET_ADDRSTRLEN
    inet_ntop(AF_INET, &(peer_addr.sin_addr), ip, INET_ADDRSTRLEN);   //inet_ntop 
	if(client_id != -1) cout <<"Client: " << ip << ":"<< ntohs(peer_addr.sin_port) << endl;
	return client_id;
}


int Sending(int client_id, char buffer[1024]){
	int s = send(client_id, buffer, 1024,0 );
	return s;
}


string Receiving(int client_id){
	char buffer[1024];
	int r = recv(client_id, buffer, 1024 ,0 );
	return string(buffer);
}


int Parsable(string s){
	istringstream isstring(s);
	string user , pass , username, password;
	if(!(isstring >> user >> username >> pass >> password)){
		if(user != "User:" or pass != "Pass:") return 0;
		else return 1;
	}
}


int Num_Messages(string user){
	// Returns the number of files and folders present at the specifies user 
	string s = ".";
	string s1 = "..";
	char *ptr = new char[2];
	char *ptr1 = new char[3];
	char *buffer = new char[user.length() + 1];
	strcpy(ptr, s.c_str());
	strcpy(ptr1, s1.c_str());
	strcpy(buffer, user.c_str());
	
	int count = 0;
	DIR *d;
	struct dirent *dir;
	d = opendir(buffer);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strcmp(ptr, dir->d_name) && strcmp(ptr1, dir->d_name)) {
				// Uncomment the below line to display file/directory names
				// cout << dir->d_name << endl;
				count++;
			}
		}
	}
	return count;
}


int Retr_Message(string user, string file,	int  client_id){
	char *buffer = new char[user.length() + 1];
	strcpy(buffer, user.c_str());
	DIR *d;
	struct dirent *dir;
	d = opendir(buffer);

	int count = 0;
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			
			string dname = dir->d_name;
			string delimeter = ".";
			int w = dname.find(delimeter);
			string name = dname.substr(0,w);

			if(name == file){ 
				count++;
				break;
			}
		}
		closedir (d);
	}
	return count;
}


void DirAccess(char *directory){
	DIR *dir = opendir(directory); // open dir d

	if(! dir){
		cout << "Directory " << directory << " doesn't exist\n"; 
		exit (4);
	}
}

int CreateSocket(){
	return socket(PF_INET, SOCK_STREAM, 0);
}


int destroySocket(int sock_id){
	close(sock_id);
}


map<string, string> GetUserName(char * file){
	map<string, string> user_data;
	string line;

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

//------------------------------------------------------------------------------------------------------------------
//---------------------------------------------    *INT MAIN*   ----------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	// if wrong number of command line arguments are given
	if(argc != 4) { cout << "Usage : [filename] [portNum] [passwordfile] [user-database]\n";    exit(1);}

	// check directory is accessible
	DirAccess(argv[3]);

	// port Number
	int portNum = stoi(argv[1]) ;

	int sock_id = CreateSocket();
	
	//define the server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portNum);
	inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));

	//bind the server to our specified IP and port 
	int bind_num = Binding(sock_id, server_address, portNum);
	PasswdFileExist(argv[2]);
	int listen_num = Listening(sock_id, 2, portNum);

    // username, password from passwordfile
	map<string , string> user_data = GetUserName(argv[2]);
//------------------------------------------------------------------ Connection established with client --------------------------------------

	while(true){
		int client_id = Accepting(sock_id);
	    string buffer = Receiving(client_id);
		  	
	    char char_buffer[1024];
		strcpy(char_buffer, buffer.c_str());
	    	
	    if(! Parsable(buffer)) { printf("Unknown Command\n"); close(client_id); }
	    else {
	    	string user_rec = GetUserFromClient(buffer);
			string pass_rec = GetPassFromClient(buffer);

			if(user_data.find(user_rec) == user_data.end()) { printf("Invalid User\n"); close(client_id);}
			else if(pass_rec != user_data.find(user_rec)->second) { printf("Wrong Passwd\n"); close(client_id);}
			else { 
			  		
				string ss = "Welcome " + user_rec;
				char char_array[1024];
				strcpy(char_array, ss.c_str());
				cout << char_array << endl;
				Sending(client_id, char_array);

				string list_message = Receiving(client_id);

				if(list_message != "LIST") { printf("Unknown Command\n"); close(client_id); }
				else {
					int mess = Num_Messages(argv[3] + user_rec);
					string message_t = user_rec + ": " + "No of messages " + to_string(mess);
				 	char mess_buffer[1024];
				 	strcpy(mess_buffer, message_t.c_str());

				  	cout << message_t << endl;
				  	Sending(client_id, mess_buffer);


				  	string arguments = Receiving(client_id);

				  	for(int i=0; i<stoi(arguments); i++){

					  	string retrv_message = Receiving(client_id);
						istringstream isstring(retrv_message);
						string  retr , file ;
						isstring >> retr >> file;


						if(retr != "RETRV" ) { printf("Unknown Command\n"); close(client_id); }
						else {
								int r = Retr_Message(argv[3] + user_rec, file, client_id);
							if(! r) {printf("Message Read Fail\n"); close(client_id); continue;}
							else {
								cout << user_rec << ": Transferring Message M \n";
								Send_File(argv[3] + user_rec, file, client_id);

							}
						}
				  	}

				  	string quit_message = Receiving(client_id);

					if(quit_message == "quit"){
						cout << "Bye " << user_rec << endl; 
						close(client_id);
					}
				}
			}	
	    }	    
	}

    destroySocket(sock_id);
    
	return 0;
}