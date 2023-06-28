#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>

#define PORT 80
#define HOST "192.168.8.7"
#define RESOURCE "/api/webserver/SesTokInfo"

int main(){
	int socket_desc;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[4096];

	std::string host = HOST;
	std::string resource = RESOURCE;

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	server = gethostbyname(host.c_str());

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT); /* PORT * 256 */

	// serv_addr.sin_addr.s_addr = (long)server->h_addr_list[0];
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	// serv_addr.sin_addr.s_addr = *((unsigned int *)(&server->h_addr_list[0]));

	connect(socket_desc, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	std::string request = "GET " + resource + " HTTP/1.1\nHost: " + host + "\n\n";

	send(socket_desc, request.c_str(), request.size(), 0);

	int n;
	std::string raw_site;
	while ((n = recv(socket_desc, buffer, sizeof(buffer), 0)) > 0){
		puts("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		raw_site.append(buffer, n);
	}
	close(socket_desc);

	std::cout << raw_site << std::endl;
	return 0;
}
