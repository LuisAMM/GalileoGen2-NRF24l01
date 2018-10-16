#include "programa_final.hpp"

using namespace std;

uint8_t NRFStatus;
uint8_t j=0;

//manejador de senales para terminar el programa cuando se ha enviado todo el archivo, CTRL + c
void sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("closing nicely\n");
	j++;
    }
}

//pone en alto o en bajo el pin de CE
void NRFWriteCE(GPIO CE,uint8_t Value)
{	
	if (Value)
		CE->write(1);
	else
		CE->write(0);
		
}

//pone en alto o en bajo el pin de CSN
void NRFWriteCSN(GPIO CSN , uint8_t Value)
{
	if (Value ) 
		CSN->write(1);
	else
		CSN->write(0);
}

//Envia un byte de data al modulo mediante SPI
uint8_t transferSPI(SPI spi, uint8_t data)
{		
	return spi->writeByte(data);
}

//Lee la data de un registro
uint8_t NRFReadRegister(SPI spi, uint8_t RegNum, GPIO CSN) //csn,spi
{
	uint8_t ReturnValue=0;
	NRFWriteCSN(CSN,0);	
	if (RegNum < 0x20)
	{
		NRFStatus = transferSPI(spi,RegNum); //se solicita la informacion del registro
		ReturnValue = transferSPI(spi,0xff); // Se envia basura para generar clocks y que nos devuelva la informacion del registro		
	}
	else
	{
		ReturnValue = -1;
	}
	NRFWriteCSN(CSN,1);		
	return ReturnValue;
}

//escribir en un registro
uint8_t NRFWriteRegister(uint8_t RegNum,uint8_t Value, GPIO CSN, SPI spi) //csn
{
	uint8_t ReturnValue=0;
	NRFWriteCSN(CSN,0);	
	if (RegNum < 0x20)
	{		
		NRFStatus = transferSPI(spi,0x20+regnum); // se envia el comando W_REGISTER				
		ReturnValue = transferSPI(spi,Value);	  // escribe la data en el registro
	}
	else
	{
		ReturnValue = -1;
	}
	NRFWriteCSN(CSN,1);
	return ReturnValue;
}

//vacia el FIFO de TX
uint8_t NRFFlushTX(SPI spi, GPIO CSN)
{
	NRFWriteCSN(CSN,0);	
	NRFStatus = transferSPI(spi,0xe1); // envia el comando Flush TX
	NRFWriteCSN(CSN,1);
}

//vacia el FIFO de RX
uint8_t NRFFlushRX(SPI spi, GPIO CSN)
{
	NRFWriteCSN(CSN,0);	
	NRFStatus = transferSPI(spi,0xe2); // envia el comando Flush RX
	NRFWriteCSN(CSN,1);
}

//obtiene el tamano del payload que esta de primero en el FIFO de RX
uint8_t GetPW(SPI spi, GPIO CSN)
{
        NRFWriteCSN(CSN,0);
        NRFStatus = transferSPI(spi,0x60); // envia el comando R_RX_PL_WID
        return transferSPI(spi,0xff); // envia basura para generar clocks, y que nos regrese la data
        NRFWriteCSN(CSN,1);
}

//coloca la direccion del pipe de recepcion 0
uint8_t NRFSetRXAddress0(uint8_t AddressLength, uint8_t * Address, GPIO CSN, SPI spi)
{
	uint8_t index;
	switch (AddressLength) {
		case 3 : {
			NRFWriteRegister(3,1,CSN,spi); // 3 byte address length			
			break;
		}
		case 4 : {
			NRFWriteRegister(3,2,CSN,spi); // 4 byte address length
			break;
		}
		case 5 : {
			NRFWriteRegister(3,3,CSN,spi); // 5 byte address length
			break;
		}
		default: {
			return -1; // invalid address length
		}
	}
	NRFWriteCSN(CSN,0);
	NRFStatus = transferSPI(spi,0x20+0x0a); // empieza a escribir la direccion del RX_P0_Pipe
	for (index = 0; index < AddressLength; index++)
	{
		NRFStatus = transferSPI(spi,Address[index]);
	}
	NRFWriteCSN(CSN,1);
}

//coloca la direccion del pipe de recepcion 1
uint8_t NRFSetRXAddress1(uint8_t AddressLength, uint8_t * Address, GPIO CSN, SPI spi)
{
	uint8_t index;
	switch (AddressLength) {
		case 3 : {
			NRFWriteRegister(3,1,CSN,spi); // 3 byte address length			
			break;
		}
		case 4 : {
			NRFWriteRegister(3,2,CSN,spi); // 4 byte address length
			break;
		}
		case 5 : {
			NRFWriteRegister(3,3,CSN,spi); // 5 byte address length
			break;
		}
		default: {
			return -1; // invalid address length
		}
	}
	NRFWriteCSN(CSN,0);
	NRFStatus = transferSPI(spi,0x20+0x0b); // empieza a escribir la direccion del RX_P1_Pipe
	for (index = 0; index < AddressLength; index++)
	{
		NRFStatus = transferSPI(spi,Address[index]);
	}
	NRFWriteCSN(CSN,1);
}

//coloca la direccion del pipe de transmision
uint8_t NRFSetTXAddress(uint8_t AddressLength,uint8_t* Address, GPIO CSN, SPI spi)
{
	uint8_t index;
	switch (AddressLength) {
		case 3 : {
			NRFWriteRegister(3,1,CSN,spi); // 3 byte address length			
			break;
		}
		case 4 : {
			NRFWriteRegister(3,2,CSN,spi); // 4 byte address length
			break;
		}
		case 5 : {
			NRFWriteRegister(3,3,CSN,spi); // 5 byte address length
			break;
		}
		default: {
			return -1; // invalid address length
		}
	}
	NRFWriteCSN(CSN,0);
	NRFStatus = transferSPI(spi,0x20+0x10); // empieza a escribir la direccion del pipe de transmision
	for (index = 0; index < AddressLength; index++)
	{
		transferSPI(spi,Address[index]);
	}
	NRFWriteCSN(CSN,1);
	return(0);
}

//coloca el nRF24l01+ en modo de transmisor
uint8_t NRFEnableTXMode(SPI spi, GPIO CSN)
{
	NRFWriteRegister(0,0x0e,CSN,spi); 
}

//coloca el nRF24l01+ en modo de receptor
uint8_t NRFEnableRXMode(SPI spi, GPIO CSN)
{
	NRFWriteRegister(0,0x0f,CSN,spi); 
}

//escribe la data que se va a enviar
uint8_t NRFWriteData(uint8_t Length, uint8_t * Data, GPIO CE, GPIO CSN, SPI spi) //csn
{
	uint8_t index;
	if (Length > 32)
		return -1; // mucha data
	NRFWriteCE(CE,0);
	NRFWriteRegister(0x07,0x70,CSN,spi); // limpia las banderas de RX_DR,TX_DS,MAX_RT
	NRFEnableTXMode(spi,CSN);
	NRFWriteCSN(CSN,0);
	NRFStatus = transferSPI(spi,0xa0); // empieza a escribir en el buffer de TX
	for (index = 0; index < Length; index++)
	{
		transferSPI(spi,Data[index]);
	}
	NRFWriteCSN(CSN,1);
	NRFWriteCE(CE,1);	
	printf("Sending..");
	NRFEnableRXMode(spi,CSN);
}

//lee la data recibida
uint8_t NRFReadData(uint8_t MaxLength,uint8_t * Data, GPIO CSN, SPI spi) //csn
{ // se asume la data en el pipe 1
	uint8_t available_bytes;
	uint8_t index;
	uint8_t Length;
	available_bytes=NRFReadRegister(spi,0x12,CSN); // ve cuantos bytes hay disponibles en el pipe 1
	if (available_bytes == 0)
		return 0;
	NRFWriteCSN(CSN,0);
	NRFStatus = transferSPI(spi,0x61); // empieza a leer del FIFO de RX
	if (available_bytes > MaxLength)
		Length = MaxLength;
	else
		Length = available_bytes;
	for (index = 0; index < Length; index++)
	{
		Data[index]=transferSPI(spi,0xff);
	}
	NRFWriteCSN(CSN,1);
	return Length;
}

// configuracion inicial de los registros
void NRFinit(SPI spi, GPIO CE, GPIO CSN)
{		
	NRFWriteRegister(0,0x0f,CSN,spi);		// activo PRX, PWR_UP, CRC en 2 bytes y CRC habilitado 
	NRFWriteRegister(1,3,CSN,spi);			// habilitar auto ack en P0 y P1
	NRFWriteRegister(2,0x03,CSN,spi);		// habilito los pipes 0-1 para data
	NRFWriteRegister(3,3,CSN,spi);			// direccion de 5 bytes
	NRFWriteRegister(4,0x24,CSN,spi);		// 750us entre reintentos, 4 retransmisiones
	NRFWriteRegister(0x05,0x05,CSN,spi); 	// canal 5
	NRFWriteRegister(0x06,0x06,CSN,spi); 	// 1Mbps, 0 dBm
	NRFWriteRegister(0x07,0x70,CSN,spi); 	// climpiar todas las banderas
	NRFWriteRegister(0x1c,0x03,CSN,spi);	// activar payload dinamico en los pipes 0-1
	NRFWriteRegister(0x1d,0x04,CSN,spi);	//activar opcion de payload dinamico
	NRFWriteCE(CE,1);
	NRFEnableRXMode(spi,CSN);				// empieza a escuchar
	Activate(spi,CSN);
}

//funcion que se encarga de recibir la data del RF
void poll(GPIO CE, GPIO CSN, SPI spi)
{
	FILE* arch=fopen("prueba.txt","w");
	while(j==0)
	{
		// verifica si hay data en el FIFO de RX
		if ((NRFReadRegister(spi,0x7,CSN) & 0x40) > 0)
		{
			printf("Got Data\n");
		        
			char RXData[GetPW(spi,CSN)]; //crea una variable del tamano de la data
		        
			NRFReadData(GetPW(spi,CSN),(uint8_t*)RXData,CSN,spi); // lee la data
		    RXData[sizeof(RXData)]=0;
		    printf("%s\n",RXData );
			fwrite(RXData,sizeof(RXData),1,arch);
		    NRFWriteRegister(0x07,0x70,CSN,spi); // limpia todas las banderas
		}
	}
	fclose(arch);
}

//imprime todos los registros
void printReg(SPI spi, GPIO CSN)
{
    uint8_t regnum,i;
        for (regnum = 0; regnum <= 0x17; regnum++)
        {
            printf("Registro(%x) : 0x%x\n",regnum,NRFReadRegister(spi,regnum,CSN));
        }
}