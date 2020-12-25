#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

#define MSG_DUP_FILE "Error: File is existet on server."
#define MSG_RECV_FILE "Successful transfering."
#define MSG_CLOSE "Cancel file transfer"
#define MSG_RECV "Received."

char* extract_file_name(char* file_path) {
	int i;
	int n = strlen(file_path);
	char* file_name;
	for(i = n-1; i >= 0; --i) {
		if(file_path[i] == '/')
			break;
	}

	if(i == 0) //current directory so that no '/'
		return file_path;

	file_name = (char*)malloc((n-i)*sizeof(char));
	memcpy(file_name, &file_path[i+1], n-i);

	return file_name;
}

int send_file(int client_sock, char* file_path) {
	struct stat st;
	char recv_data[BUFF_SIZE];
	char sendbuff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	FILE* fp;
	int nLeft, idx;
	char* file_name = NULL;
	off_t file_size = 0;
	char file_size_str[65];
	size_t result = 0;

	if(file_path[0] == '\0') { // enter an empty string
		printf("Sending file ended. Exiting.\n");
		bytes_sent = send(client_sock, file_path, 1, 0); 
		if(bytes_sent <= 0) 
			printf("Connection closed!\n");
		return 1;
	}

	// check if file exists
	if(stat(file_path, &st) == -1) { // Not exists
		fprintf(stderr, "Error: File not found.\n");
		bytes_sent = send(client_sock, MSG_CLOSE, strlen(MSG_CLOSE), 0); //echo error message
		if(bytes_sent <= 0) 
			printf("Connection closed!\n");
		return -1;
	}

	file_name = extract_file_name(file_path);
	printf("Uploading file to server: %s\n",file_name);	
	bytes_sent = send(client_sock, file_name, strlen(file_name), 0);
	if(bytes_sent <= 0) {
		printf("Connection closed!\n");
		return -1;
	}
	
	// confirm that server received file name and check file status on server side
	bytes_received = recv(client_sock, recv_data, BUFF_SIZE-1, 0); 
	if(bytes_received <= 0) {
		printf("Connection closed!\n");
		return -1;
	}
	else
		recv_data[bytes_received] = '\0'; 

	printf("%s\n", recv_data);
	if(strcmp(recv_data, MSG_DUP_FILE) == 0)		//file was found on server, duplicate file	
		return -1;
	bzero(recv_data, sizeof(recv_data));

	file_size = st.st_size;
	sprintf(file_size_str,"%lu",file_size);
	bytes_sent = send(client_sock, file_size_str, strlen(file_size_str), 0);
	if(bytes_sent <= 0) {
		printf("Connection closed!\n");
		return -1;
	}

	//open file and send data
	if((fp=fopen(file_path, "rb")) == NULL) {
		fprintf(stderr, "Open file error.\n");
		exit(1);
	}
	int loop_size = file_size;
	nLeft = file_size%BUFF_SIZE;	// cuz file size is not divisible by BUFF_SIZE

	while(loop_size > 0) {
		idx = 0;

		result += fread(sendbuff, 1, nLeft, fp); // use fread instead of fgets because fgets stop reading if newline is read
		while (nLeft > 0)
		{
			// Assume s is a valid, connected stream socket
			bytes_sent = send(client_sock, &sendbuff[idx], nLeft, 0);
			if (bytes_sent <= 0)
			{
				// Error handler
				printf("Connection closed.Trying again.\n");
			}
			nLeft -= bytes_sent;
			idx += bytes_sent;
		}
		
		bzero(sendbuff, sizeof(sendbuff)); 
		loop_size -= BUFF_SIZE; // decrease unfinished bytes 
		nLeft = BUFF_SIZE;		// reset nLeft
	}

	if(result != file_size) {
		printf("Error reading file.\n");
		return -1;
	}

	bytes_received = recv(client_sock, recv_data, BUFF_SIZE-1, 0);
	if(bytes_received <= 0) {
		printf("Connection closed!\n");
		return -1;
	}
	else
		recv_data[bytes_received] = '\0'; 

	printf("%s\n", recv_data);
	if(strcmp(recv_data, MSG_RECV_FILE) != 0)		//if cannot receive last message, file transfer is interrupted	
		return -1;

	// clean
	fclose(fp);
	free(file_name);
	return 0;
}

int main(int argc, char *argv[]) {
	int client_sock;
	char buff[BUFF_SIZE];
	int SERVER_PORT; 
	char* SERVER_ADDR;
	struct sockaddr_in server_addr; /* server's address information */
	int status;
	
	if(argc < 2) {
		printf("No specified port.\n");
		exit(0);
	}

	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	SERVER_ADDR =  "192.168.43.196";
	SERVER_PORT = atoi(argv[1]);
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("Error!Can not connect to sever! Client exit imediately!\n");
		return 0;
	}
	while(1) {
		//Step 4: Communicate with server
		printf("Insert string to send:");
		memset(buff,'\0',(strlen(buff)+1));
		fgets(buff, BUFF_SIZE, stdin);	
		buff[strlen(buff)-1] = '\0'; //remove trailing newline	
		status = send_file(client_sock, buff);
		
		if(status == 1)
			break;
	}		

	//Step 5: Close socket
	close(client_sock);
	return 0;
}
