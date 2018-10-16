#ifndef PROGRAMA_FINAL_H
#define PROGRAMA_FINAL_H

#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "mraa.hpp"

#include <string>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define SPI mraa::Spi*
#define GPIO mraa::Gpio*
#define  BUFSIZE     2148064     

/**
 * @brief manejador de senales para terminar el programa cuando se ha enviado todo el archivo, CTRL + c 
 * 
 * @param signo 
 */
void sig_handler(int signo);

/**
 * @brief pone en alto o en bajo el pin de CE
 * 
 * @param CE pin utilizado para CE
 * @param Value Alto o bajo
 */
void NRFWriteCE(GPIO CE,uint8_t Value);

/**
 * @brief pone en alto o en bajo el pin de CSN
 * 
 * @param CSN pin utilizado para CSN
 * @param Value Alto o bajo
 */
void NRFWriteCSN(GPIO CSN , uint8_t Value);

/**
 * @brief Envia un byte de data al modulo mediante SPI 
 * 
 * @param spi clase SPI
 * @param data datos a escribir
 * @return uint8_t retultado de la escritura
 */
uint8_t transferSPI(SPI spi, uint8_t data);

/**
 * @brief Lee la data de un registro 
 * 
 * @param spi clase SPI
 * @param RegNum numero de registro
 * @param CSN pin de CSN
 * @return uint8_t resultado de la lectura
 */
uint8_t NRFReadRegister(SPI spi, uint8_t RegNum, GPIO CSN); 

/**
 * @brief escribir en un registro 
 * 
 * @param RegNum numero de registro
 * @param Value valor a escribir en el registro
 * @param CSN pin CSN
 * @param spi clase SPI
 * @return uint8_t resultado de la operacion
 */
uint8_t NRFWriteRegister(uint8_t RegNum,uint8_t Value, GPIO CSN, SPI spi);

/**
 * @brief vacia el FIFO de TX 
 * 
 * @param spi clase SPI
 * @param CSN pin de CSN
 * @return uint8_t resultado de la operacion
 */
uint8_t NRFFlushTX(SPI spi, GPIO CSN);

/**
 * @brief vacia el FIFO de RX 
 * 
 * @param spi  clase SPI
 * @param CSN pin CSN 
 * @return uint8_t resultado de la operacion
 */
uint8_t NRFFlushRX(SPI spi, GPIO CSN);

/**
 * @brief obtiene el tamano del payload que esta de primero en el FIFO de RX 
 * 
 * @param spi clase SPI 
 * @param CSN pin CSN 
 * @return uint8_t tamano del payload
 */
uint8_t GetPW(SPI spi, GPIO CSN);

/**
 * @brief coloca la direccion del pipe de recepcion 0 
 * 
 * @param AddressLength longitud de la direccion
 * @param Address Direccion 
 * @param CSN pin CSN 
 * @param spi clase SPI 
 * @return uint8_t resultado de la operacion
 */
uint8_t NRFSetRXAddress0(uint8_t AddressLength, uint8_t * Address, GPIO CSN, SPI spi);

/**
 * @brief coloca la direccion del pipe de recepcion 1 
 * 
 * @param AddressLength longitud de la direccion 
 * @param Address  Direccion
 * @param CSN pin CSN 
 * @param spi clase SPI 
 * @return uint8_t resultado de la operacion
 */
uint8_t NRFSetRXAddress1(uint8_t AddressLength, uint8_t * Address, GPIO CSN, SPI spi);

/**
 * @brief coloca la direccion del pipe de transmision 
 * 
 * @param AddressLength longitud de la direccion 
 * @param Address  Direccion
 * @param CSN pin CSN 
 * @param spi clase SPI 
 * @return uint8_t resultado de la operacion
 */
uint8_t NRFSetTXAddress(uint8_t AddressLength,uint8_t* Address, GPIO CSN, SPI spi);


/**
 * @brief coloca el nRF24l01+ en modo de transmisor 
 * 
 * @param spi clase SPI 
 * @param CSN pin CSN 
 * @return uint8_t resultado de la operacion
 */
uint8_t NRFEnableTXMode(SPI spi, GPIO CSN);


/**
 * @brief coloca el nRF24l01+ en modo de receptor 
 * 
 * @param spi clase SPI 
 * @param CSN pin CSN 
 * @return uint8_t resultado de la operacion
 */
uint8_t NRFEnableRXMode(SPI spi, GPIO CSN);

/**
 * @brief escribe la data que se va a enviar 
 * 
 * @param Length longitud de la data
 * @param Data datos a transmitir
 * @param CE  pin de CE
 * @param CSN pin CSN 
 * @param spi clase SPI 
 * @return uint8_t resultado de la operacion
 */
uint8_t NRFWriteData(uint8_t Length, uint8_t * Data, GPIO CE, GPIO CSN, SPI spi);

/**
 * @brief lee la data recibida 
 * 
 * @param MaxLength longitud maxima a leer
 * @param Data buffer donde se almacenaran los datos a transmitir
 * @param CSN pin CSN 
 * @param spi clase SPI 
 * @return uint8_t resultado de la operacion
 */
uint8_t NRFReadData(uint8_t MaxLength,uint8_t * Data, GPIO CSN, SPI spi);

/**
 * @brief  configuracion inicial de los registros 
 * 
 * @param spi clase SPI 
 * @param CE pin CE
 * @param CSN pin CSN 
 */
void NRFinit(SPI spi, GPIO CE, GPIO CSN);

/**
 * @brief funcion que se encarga de recibir la data del RF 
 * 
 * @param CE pin CE
 * @param CSN pin CSN 
 * @param spi clase SPI 
 */
void poll(GPIO CE, GPIO CSN, SPI spi);

/**
 * @brief imprime todos los registros 
 * 
 * @param spi clase SPI 
 * @param CSN pin CSN 
 */
void printReg(SPI spi, GPIO CSN);

#endif