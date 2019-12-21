/*=============================================================================
 * Author: Walter Schemith
 * Date: 2019/10/05
 *===========================================================================*/

/*=====[Evitar inclusion multiple comienzo]==================================*/

#ifndef _WIFI_H_
#define _WIFI_H_

/*=====[Inclusiones de dependencias de funciones publicas]===================*/

#include "sapi.h"        // <= Biblioteca sAPI
#include <string.h>   // <= Biblioteca de manejo de Strings, ver:

/*=====[C++ comienzo]========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Macros de definicion de constantes publicas]=========================*/
#define UART_DEBUG                 UART_USB
#define UART_ESP01                 UART_232
#define UARTS_BAUD_RATE            115200

#define ESP01_RX_BUFF_SIZE         1024

#define WIFI_SSID                  "ferreyra"     // Setear Red Wi-Fi
#define WIFI_PASSWORD              "guille22cuerva" // Setear password

#define THINGSPEAK_SERVER_URL      "api.thingspeak.com"
#define THINGSPEAK_SERVER_PORT     80
#define THINGSPEAK_WRITE_API_KEY   "LG7ZWE14MPVERV2L"
#define THINGSPEAK_FIELD_NUMBER_CERRADURA    2
#define THINGSPEAK_FIELD_NUMBER_SOLICTUD     1
#define THINGSPEAK_FIELD_NUMBER_PORC_BATERIA 3


/*=====[Macros estilo funcion publicas]======================================*/



/*=====[Definiciones de tipos de datos publicos]=============================*/
CONSOLE_PRINT_ENABLE
DEBUG_PRINT_ENABLE

// Tipo de datos que renombra un tipo basico

// Tipo de datos de puntero a funcion


// Tipo de datos enumerado


// Tipo de datos estructua, union o campo de bits

/*=====[Prototipos de funciones publicas]====================================*/

bool_t esp01Init( uartMap_t uartForEsp, uartMap_t uartForDebug, uint32_t baudRate );

void esp01CleanRxBuffer( void );

bool_t esp01ShowWiFiNetworks( void );

bool_t esp01ConnectToWifiAP( char* wiFiSSID, char* wiFiPassword );

bool_t esp01ConnectToServer( char* url, uint32_t port );

bool_t esp01SendTCPIPData( char* strData, uint32_t strDataLen );

bool_t esp01SendTPCIPDataToServer( char* url, uint32_t port, char* strData, uint32_t strDataLen );

bool_t envioDeDatosAlServidor( char* strData, uint32_t strDataLen, int resultado );
bool_t preparacionDeDatosParaEnviar( char* url, uint32_t port, char* strData, uint32_t strDataLen, int resultado );

int consultaUltimoValor();

void stopProgramError( void );

char substr(char substr);

/*=====[Prototipos de funciones publicas de interrupcion]====================*/



/*=====[C++ fin]=============================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Evitar inclusion multiple fin]=======================================*/

#endif /* _MODULE_NAME_H_ */
