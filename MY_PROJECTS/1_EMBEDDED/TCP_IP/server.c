#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 3
#define PORT 8080
#define SA struct sockaddr


// CLOUDAPP

/*



STATUS: (DAIRE 28 ICIN ) => GUNCELLENDI


              0x021C10 => ELEKTRIK ACIK KLIMA KAPALI 


              0x021C20 => ELEKTRIK KAPALI KLIMA ACIK

              0x021C31 => HEM KLIMA HEM ELEKTRIK ACIK
              0x021C30 => HEM KLIMA HEM ELEKTRIK KAPALI 


*/





// 30 DAIRE 5 DURUM
uint8_t volatile status[31];


// Function designed for chat between client and server.
void func(int connfd)
{
	uint8_t buff[MAX];
	uint8_t i;

	for(i=1;i<31;i++)
	{
		status[i]=0x30; // BASLANGICTA ELEKTRIK VE DOLAYISIYLA KLIMA KAPATILIYOR.
	}
	bzero(buff, MAX);

	// infinite loop for chat
	while(1)
	{

		

		// read the message from client and copy it in buffer
		read(connfd, buff,MAX);

			

	

		if(buff[0] == 0x01) // KOMUT GELIYOR
		{	

			status[buff[1]] = buff[2];

			write(connfd,"KOMUT ALINDI ",12);
			bzero(buff, MAX);

		}
		else if(buff[0] == 0x02) // DURUM SORGULAMA GELIYOR
		{	

			buff[2] = status[buff[1]];

			write(connfd, buff, MAX);
			bzero(buff, MAX);
		}
		//ELSE EKLENMEDI  OLUSABILECEK SORUNLAR  CLIENT KISMINDA ONLENMEYE CALISILDI.(HERHANGI BIR AYKIRI DURUMA KARSI 0x00 KOMUTU GECERSIZ KOMUT OLARAK EKLENEDEBILIR)



	
	}
}

// Driver function
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}

	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}

	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}

	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("CLIENT ALINDI...\n");

	// Function for chatting between client and server
	func(connfd);

	// After chatting close the socket
	close(sockfd);

	return 0;
}
