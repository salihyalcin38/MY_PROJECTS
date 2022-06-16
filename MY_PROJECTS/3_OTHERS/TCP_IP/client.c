
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <stdlib.h>

#define MAX 3
#define PORT 8080
#define SA struct sockaddr



// CUSTOMERAPP

/*

KOMUT: (DAIRE 3 ICIN ) => GUNCELLENDI


              0x010310 => ELEKTRIK ACIK KLIMA KAPALI 


              0x010320 => ELEKTRIK KAPALI KLIMA ACIK

              0x010331 => HEM KLIMA HEM ELEKTRIK ACIK
              0x010330 => HEM KLIMA HEM ELEKTRIK KAPALI 


*/




/* CONTROL BYTE*/
uint8_t daire=1,kontak=1,komut=1,istek=1;
char dummy;






void func(int sockfd)
{



    uint8_t buff[MAX];
    char word[12];
    

   	bzero(buff, MAX);

    while(1)
    {



    	printf("\n\nDurum bilgisi almak icin - 1\nKomut gondermek icin - 2: ");
    	scanf("%hhu",&istek);
    	scanf("%c",&dummy);

    	

    	// 1-LISTELE (getSTATUS)
    	if(istek == 1)
    	{
    		uint8_t i;

    		for(i=1;i<31;i++)
    		{

    			bzero(buff, MAX);


        		buff[0] = 0x02;
        		buff[1] = i;
        	

        		write(sockfd, buff, MAX);

        		bzero(buff, MAX);
        		read(sockfd, buff, MAX);
        		
 				if(buff[2] == 0x30)
 				{
					printf("\n%d. DAIRENIN DURUMU :   ELEKTRIK VE KLIMA KAPALI",buff[1]);	
 				}

 				else if(buff[2] == 0x10)
 				{
					printf("\n%d. DAIRENIN DURUMU :   ELEKTRIK ACIK, KLIMA KAPALI ",buff[1]);	
 				}

 				else if(buff[2] == 0x31)
 				{
					printf("\n%d. DAIRENIN DURUMU :   ELEKTRIK VE KLIMA ACIK",buff[1]);	
 				}
 				else if(buff[2] == 0x20)
 				{
 					printf("\n%d. DAIRENIN DURUMU :   ELEKTRIK KAPALI, KLIMA ACIK",buff[1]);	
 				}
 

    		}


    	}


    	// 2-KOMUT GONDER
    	else if(istek == 2)
    	{


		bzero(buff, MAX);

		// ****************
          
        printf("\n\nDaire No Giriniz(1-30 arasi) : ");
        scanf("%hhu",&daire);	
        scanf("%c",&dummy);
        printf("DAIRE %d\n",daire );

		while(daire<1 || daire>30)
        {
        	// ONDALIKLI SAYI GIRME DURUMU KONTROL EDILMEDI CUNKU ARAYUZDEN ENGELLENEBILIR.
        	printf("\nARALIKLARIN DISINDA(1-30 ARASI TAM SAYI GIRINIZ) : "); 
        	printf("\nDaire No Giriniz(1-30 arasi) : ");
        	scanf("%hhu",&daire);	
        	scanf("%c",&dummy);
        }

        // ****************

		printf("Kontak secin\n1-Elektrik\n2-Klima: ");

        scanf("%hhu",&kontak);
        scanf("%c",&dummy);

        while(!(kontak==1 || kontak==2))
        {
        	printf("\nYANLIS DEGER GIRDINIZ(1 YADA 2 GIRINIZ): ");
        	scanf("%hhu",&kontak);	
        	scanf("%c",&dummy);
        }

        // ****************

		printf("Komut secimi\n1-Ac\n2-Kapat : ");

        scanf("%hhu",&komut);
        scanf("%c",&dummy);

        while(!(komut==1 || komut==2))
        {
        	printf("\nYANLIS DEGER GIRDINIZ(1 YADA 2 GIRINIZ) : ");
        	scanf("%hhu",&komut);	
        	scanf("%c",&dummy);
        }


        	
   


        bzero(buff, MAX);


        buff[0] = 0x01;
        buff[1] = daire;

    
       	if(kontak==1 && komut==1) // ELEKTRIK AC
       	{

       		buff[0] = 0x02;
      
       		write(sockfd, buff, MAX);

        	bzero(buff, MAX);
        	read(sockfd, buff, MAX);

        	if(buff[2] == 0x10) 
        	{
				
        		printf("\nELEKTRIK ZATEN ACIK. ");
             	fflush(stdout);

        	}

        	else if(buff[2] == 0x20)
        	{
        		buff[2] = 0x31;
				printf("\nELEKTRIK ACILDI. ");
             	fflush(stdout);
        	}

        	else if(buff[2] == 0x30)
        	{
        	    buff[2] = 0x10;
				printf("\nELEKTRIK ACILDI. ");
             	fflush(stdout);
        	}

        	buff[0] = 0x01;
        	buff[1] = daire;


 
       	}

       	else if(kontak==1 && komut==2) // ELEKTRIK KAPAT 
       	{
       		buff[0] = 0x02;
 
       		write(sockfd, buff, MAX);

        	bzero(buff, MAX);
        	read(sockfd, buff, MAX);

        	if(buff[2] == 0x30)
        	{

        			
        		printf("\nELEKTRIK ZATEN KAPALI ");
             	fflush(stdout);
        	}

        	else if(buff[2] == 0x20)
        	{
                		
        		printf("\nELEKTRIK ZATEN KAPALI ");
             	fflush(stdout);
        	}

        	else if(buff[2] == 0x31)
        	{
        		buff[2] = 0x20;
        		printf("\nELEKTRIK KAPATILDI. ");
                fflush(stdout);
        	}

        	else if(buff[2] == 0x10)
        	{        	
        		buff[2] = 0x30;
        		printf("\nELEKTRIK KAPATILDI. ");
                fflush(stdout);

        	}

        	buff[0] = 0x01;
        	buff[1] = daire;
  



       	}	

		else if(kontak==2 && komut==1) // KLIMA AC 
       	{

       		buff[0] = 0x02;
 
       		write(sockfd, buff, MAX);

        	bzero(buff, MAX);
        	read(sockfd, buff, MAX);

        	if(buff[2] == 0x20)
        	{
				
        		printf("\nKLIMA ZATEN ACIK");
        		fflush(stdout);
        	}

        	else if(buff[2] == 0x31)
        	{
        		      		
        		printf("\nKLIMA ZATEN ACIK");
        		fflush(stdout);
        	}

        	else if(buff[2] == 0x10)
        	{

        		buff[2] = 0x31;
        		printf("\nKLIMA ACILDI. ");
            	fflush(stdout);
        	}

        	else if(buff[2] == 0x30)
        	{
				buff[2] = 0x20;
        		printf("\nKLIMA ACILDI. ");
            	fflush(stdout);
        	}
        	

        	buff[0] = 0x01;
        	buff[1] = daire;
       		
 		


       	}

       	else if(kontak==2 && komut==2) // KLIMA KAPAT 
       	{
       		buff[0] = 0x02;
 
       		write(sockfd, buff, MAX);

        	bzero(buff, MAX);
        	read(sockfd, buff, MAX);

        	if(buff[2] == 0x10)
        	{
        		       		
		   		printf("\nKLIMA ZATEN KAPALI ");
             	fflush(stdout);
        	}

        	else if(buff[2] == 0x30)
        	{
        		       		
		   		printf("\nKLIMA ZATEN KAPALI ");
             	fflush(stdout);
        	}

        	else if(buff[2] = 0x20)
        	{
        		buff[2] = 0x30;
        		printf("\nKLIMA KAPATILDI. ");
                fflush(stdout);
        	}

        	else if(buff[2] = 0x31)
        	{
        		buff[2] = 0x10;
        		printf("\nKLIMA KAPATILDI. ");
                fflush(stdout);
        	}

        	buff[0] = 0x01;
        	buff[1] = daire;



       		
       	}	

       	// ELSE EKLENMEDI OLUSABILECEK SORUNLAR ONLENMEYE CALISILDI.(HERHANGI BIR AYKIRI DURUMA KARSI 0x00 KOMUTU GECERSIZ KOMUT OLARAK EKLENEDEBILIR)






        write(sockfd, buff, MAX);

        bzero(buff, MAX);
        read(sockfd, word, 12);

        printf("\nCLOUDAPP -  : %s",word);

    	}

    	else 
    	{
    		printf("\n1 veya 2 degerini giriniz : ");
    	}


    }
	
}

int main()
{
	int sockfd, connfd;
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
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}

	else
		printf("CLOUDA BAGLANILDI..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}
