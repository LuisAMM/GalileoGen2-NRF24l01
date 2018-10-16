#include "programa_final.hpp"

using namespace std;

int main(int argc, char  *argv[])
{
	uint8_t pipes[][6]={"1Node","3Node"};

	SPI spi;
	spi = new mraa::Spi(0);
	spi->frequency(1000000);

	signal(SIGINT, sig_handler);

	GPIO CE = new mraa::Gpio(9);
	GPIO CSN = new mraa::Gpio(8);
	
	CE->dir (mraa::DIR_OUT);
	CSN->dir (mraa::DIR_OUT);
	
	CE->write(0);
	CSN->write(1);

	NRFinit(spi,CE,CSN);
	NRFSetRXAddress0(5,pipes[1],CSN,spi); 
	NRFSetTXAddress(5,pipes[1],CSN,spi);	
	NRFSetRXAddress1(5,pipes[0],CSN,spi);	
	NRFFlushTX(spi,CSN);
	NRFFlushRX(spi,CSN);
	NRFEnableRXMode(spi,CSN);
	NRFWriteCE(CE,1);
	printReg(spi,CSN);
	poll(CE,CSN,spi);

	int tam,Npaq,resto;
	int PORTNUMBER=3000;
	char buffer1[500];
	char *buffer2;

	int     sockfd;     
	struct  sockaddr_in CLNT_addr;    /* Client Internet address    */ 
	
	FILE* fd;
	fd=fopen("prueba.zip","r");
	fseek(fd, 0L, SEEK_END);
	tam=ftell(fd);
	rewind(fd);
	
	if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) 
	{     
    		perror( "Error" );   
    		exit(1);
    	}
	
	CLNT_addr.sin_family = AF_INET;                                     
	CLNT_addr.sin_port   = htons( PORTNUMBER ); 
	inet_pton( AF_INET, argv[ 1 ], &CLNT_addr.sin_addr ); 
	
	if( connect( sockfd, (struct sockaddr*)&CLNT_addr, sizeof( CLNT_addr ) ) ==-1 )
	{       
		perror("Error");                                    
		exit(5);
	}
	
	Npaq=tam/10;
	resto=tam%10;

	send( sockfd, &tam, sizeof(int), 0 );
	send( sockfd, &Npaq, sizeof(int), 0 );
	send( sockfd, &resto, sizeof(int), 0 );
	printf("tam==%d Npaq==%d resto==%d\n",tam,Npaq,resto);
	
	while(Npaq!=0)
	{
		fread(buffer1,1,10,fd);
		if(send( sockfd, buffer1, 10, 0 ) ==-1 )
		{
			perror("Error");                  
			exit(5);
		}
		Npaq--;
		bzero(buffer1,10);
	}
	if(resto!=0)
	{
		buffer2=(char*)malloc(resto);
		fread(buffer2,1,resto,fd);
		if(send( sockfd, buffer2, resto, 0 ) ==-1 )
                {
                        perror("Error");                 
                        exit(5);
                }
		free(buffer2);
	}

	fclose(fd);
	sleep(2);
	close(sockfd);
}