/*=============================================================================
 * Author: Walter Schemith
 * Date: 2019/10/05
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

// Includes de FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "Candado.h"
#include "sapi.h"
#include "wifi.h"
#include "ff.h"       // <= Biblioteca FAT FS
#include "fssdc.h"      // API de bajo nivel para unidad "SDC:" en FAT FS

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/
static FATFS fs;           // <-- FatFs work area needed for each volume
static FIL fp;             // <-- File object needed for each open file
static rtc_t rtc = {
   2019,
   12,
   19,
   4,
   20,
   10,
   0
};
/*=====[Definitions of private global variables]=============================*/
tTecla tecla_config[3];

/*=====[Main function, program entry point after power on or reset]==========*/

int main( void )
{
   // ----- Setup -----------------------------------
   boardInit();

   //printf( "candado con freeRTOS y sAPI.\r\n" );

   tecla_config[0].tecla 	= TEC1;
   tecla_config[1].tecla 	= TEC2;
   tecla_config[2].tecla 	= TEC3;

   uartConfig (UART_USB, 115200);



   candado_inicializar_MEF();
   inicializarSD();

   // Inicializar UART_USB como salida de consola
     debugPrintConfigUart( UART_DEBUG, UARTS_BAUD_RATE );

     if( !esp01Init( UART_ESP01, UART_DEBUG, UARTS_BAUD_RATE ) ){
        stopProgramError(); // Como dio falso (error) me quedo en un bucle infinito
     }

     if( !esp01ConnectToWifiAP( WIFI_SSID, WIFI_PASSWORD ) ){
        stopProgramError(); // Como dio falso (error) me quedo en un bucle infinito
     }


   // Crear tarea en freeRTOS
       BaseType_t res =
       xTaskCreate(
           myTask2,                     // Funcion de la tarea a ejecutar
           ( const char * )"tarea_desbloquear",   // Nombre de la tarea como String amigable para el usuario
           configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
		   &tecla_config[0],                          // Parametros de tarea
           tskIDLE_PRIORITY+1,         // Prioridad de la tarea
           0                           // Puntero a la tarea creada en el sistema
       );

	res = xTaskCreate(
			myTask2,                 // Funcion de la tarea a ejecutar
			(const char *) "tarea_bloquear", // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 2, // Cantidad de stack de la tarea
			&tecla_config[1],                          // Parametros de tarea
			tskIDLE_PRIORITY + 1,         // Prioridad de la tarea
			0                         // Puntero a la tarea creada en el sistema
			);

	res = xTaskCreate(
				myTask2,                 // Funcion de la tarea a ejecutar
				(const char *) "tarea_cargar_bateria", // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE * 2, // Cantidad de stack de la tarea
				&tecla_config[2],                          // Parametros de tarea
				tskIDLE_PRIORITY + 1,         // Prioridad de la tarea
				0                         // Puntero a la tarea creada en el sistema
				);

       res =
           xTaskCreate(
        		tareaMostrarEstado,                     // Funcion de la tarea a ejecutar
               ( const char * )"tarea_mostrar_estado",   // Nombre de la tarea como String amigable para el usuario
               configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
               0,                          	// Parametros de tarea
               tskIDLE_PRIORITY+2,         	// Prioridad de la tarea
               0                           	// Puntero a la tarea creada en el sistema
           );

       if(res == pdFAIL)
       {
       	//error
       }

       	// Iniciar scheduler
       vTaskStartScheduler();


   // ----- Repeat for ever -------------------------
   while( true ) {

	   // Si cae en este while 1 significa que no pudo iniciar el scheduler

   }

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}
