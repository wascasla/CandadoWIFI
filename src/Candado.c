/*=============================================================================
 * Author: was
 * Date: 2019/10/05
 *===========================================================================*/

/*=====[Inclusion de su propia cabecera]=====================================*/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "Candado.h"
#include "sapi.h"
#include "leds.h"
#include "wifi.h"
#include "ff.h"         // <= Biblioteca FAT FS
#include "fssdc.h"      // API de bajo nivel para unidad "SDC:" en FAT FS

/*=====[Inclusiones de dependencias de funciones privadas]===================*/


/*=====[Macros de definicion de constantes privadas]=========================*/
//#define PULSADOR_DESBLOQUEAR TEC1
//#define PULSADOR_BLOQUEAR TEC2
//#define PULSADOR_RECARGAR_BATERIA TEC3
//#define TEC4    'PULSADOR_CONSULTAR_ESTADO'
#define BUTTON_LOGIC BUTTON_ONE_IS_UP

/*=====[Macros estilo funcion privadas]======================================*/

#define FILENAME "SDC:/registro.txt"
/*=====[Definiciones de tipos de datos privados]=============================*/

// Tipo de datos que renombra un tipo basico


// Tipo de datos de puntero a funcion


// Tipo de datos enumerado
typedef enum {
	ESTADO_DESBLOQUEADO,
	ESTADO_BLOQUEADO,
} CANDADO_ESTADO_t;

typedef enum {
	TECLA1,
	TECLA2,
	TECLA3,
	TECLA4,
} BOTON_PRESIONADO_t;
// Tipo de datos estructua, union o campo de bits

/*=====[Definiciones de Variables globales publicas externas]================*/
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

/*=====[Definiciones de Variables globales publicas]=========================*/


/*=====[Definiciones de Variables globales privadas]=========================*/

static BOTON_PRESIONADO_t botonPresionado;
static CANDADO_ESTADO_t estadoCandado;
static int nivelBateria;
static uint16_t id_Dispositivo;
char tcpIpDataToSend[100];
static delay_t delay_espera_respuesta;
char buf[100];

/*=====[Prototipos de funciones privadas]====================================*/



/*=====[Implementaciones de funciones publicas]==============================*/
void bloquear_Candado(void){
	estadoCandado = ESTADO_BLOQUEADO;
	LED_apagar(LUZ_AMARILLA);
}

void desbloquear_Candado(void){
	estadoCandado = ESTADO_DESBLOQUEADO;
	//prende luz amarilla significa que esta desbloqueado
	LED_encender(LUZ_AMARILLA);
}

void recargar_Bateria(void){
	//si el nivel de bateria es menor a 100 entonces suma en 5 el nivel de la misma cada vez que se aprieta el pulsador
	if (nivelBateria < 100){
		//encender lus verde indicador de
		LED_encender(LUZ_VERDE);
		nivelBateria = nivelBateria + 5;
		if (nivelBateria >= 100){
			nivelBateria = 100;
		}
		//delay(500);
		LED_apagar(LUZ_VERDE);
	}

}

void descargar_bateria(void){
	if (nivelBateria > 0){
		nivelBateria = nivelBateria - 5;
	}
}

void mostrar_Estado_Candado(void) {
	//consultar valor de los parametros del candado
	//mostrar por la uart los mismos
	char estado[13];
	char str[8];
	sprintf(str, "%d", estadoCandado);
	char str2[8];
	sprintf(str2, "%d", nivelBateria);
	char str3[8];
	sprintf(str3, "%d", id_Dispositivo);

	if (estadoCandado == ESTADO_BLOQUEADO) {

		strcpy( estado, "BLOQUEADO" );
		enviarDatosAlServidor(0,THINGSPEAK_FIELD_NUMBER_CERRADURA);
	}
	if (estadoCandado == ESTADO_DESBLOQUEADO) {
		strcpy( estado, "DESBLOQUEADO" );
		enviarDatosAlServidor(1,THINGSPEAK_FIELD_NUMBER_CERRADURA);
	}
	delay(20000);
	//envio el nivel de bateria para que actualice la tabla
	enviarDatosAlServidor(nivelBateria,THINGSPEAK_FIELD_NUMBER_PORC_BATERIA);
	delay(20000);
	enviarDatosAlServidor(0,THINGSPEAK_FIELD_NUMBER_SOLICTUD);

	uartWriteString(UART_USB, "Estado del candado: ");
	uartWriteString(UART_USB, estado);
	uartWriteString( UART_USB, "\r\n" );
	uartWriteString(UART_USB, "Nivel bateria: ");
	uartWriteString(UART_USB, str2);
	uartWriteString( UART_USB, "\r\n" );
	uartWriteString(UART_USB, "ID dispositivo: ");
	uartWriteString(UART_USB, str3);
	uartWriteString( UART_USB, "\r\n" );

	registrarEstadoSd();
}

void candado_inicializar_MEF(void){

	estadoCandado = ESTADO_BLOQUEADO;
	nivelBateria = 100;
	LED_apagar(LUZ_VERDE);
	LED_apagar(LUZ_ROJA);
	LED_apagar(LUZ_AMARILLA);

	uartConfig( UART_USB, 115200 );
	id_Dispositivo = 14274; //ID dispositivo

}

void candado_Update_MEF(gpioMap_t teclaPresionada) {

	int resultadoConsulta = 1;

	switch (teclaPresionada) {

	case TEC1: //tecla 1 desbloquear el candado
		//consulta el estado y la tecla que el usuario presiono
		//si la tecla que presiono es para prender entonces, se desbloquea el candado

		delayWrite(&delay_espera_respuesta, 30);

		if (estadoCandado == ESTADO_BLOQUEADO) {
			//desbloquear_Candado();
			descargar_bateria();
			//uartWriteString( UART_USB, "Candado desbloqueado" );
			//uartWriteString( UART_USB, "\r\n" );
			//exit();
			uartWriteString( UART_USB, "enviando tratando" );
			//envio solicitud de desbloqueo
			enviarDatosAlServidor(1,THINGSPEAK_FIELD_NUMBER_SOLICTUD);
			//espero varios segundos para consultar alguna respuesta
			//if( delayRead( &delay_espera_respuesta ) ) {
			delay(60000);
				//printf("%i\n",resultadoConsulta);
				//consulto si alguien dio la orden de desbloquear
				resultadoConsulta = consultarUltimoValor();

				//printf("%i\n",resultadoConsulta);
					if (resultadoConsulta == 1){
						desbloquear_Candado();
						//descargar_bateria();
						uartWriteString( UART_USB, "Candado desbloqueado" );
						uartWriteString( UART_USB, "\r\n" );
					}else{
						delay(15000);
						enviarDatosAlServidor(0,THINGSPEAK_FIELD_NUMBER_SOLICTUD);
					}
			//}

		}

		break;

	case TEC2: //tecla 2 bloquear el candado


		if (estadoCandado == ESTADO_DESBLOQUEADO) {
			bloquear_Candado();
			descargar_bateria();
			uartWriteString( UART_USB, "Candado bloqueado" );
			uartWriteString( UART_USB, "\r\n" );
			enviarDatosAlServidor(0,THINGSPEAK_FIELD_NUMBER_CERRADURA);

		}

		if (estadoCandado == ESTADO_BLOQUEADO) {
			//se debe emitir una alerta de que ya esta bloqueado
			LED_encender(LUZ_ROJA);
			delay(500);
			LED_apagar(LUZ_ROJA);

		}

		break;


	case TEC3: //tecla 3 recargar bateria

		recargar_Bateria();

		break;

	/*case TEC4: //tecla 4 consultar estado

		mostrar_Estado_Candado();

		break;*/

	default:
		candado_inicializar_MEF();
		break;

	}



}

// Implementacion de funcion de la tarea
void myTask2( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	bool_t valor;
	bool_t valor2;


    //gpioMap_t* tecla = (gpioMap_t*) taskParmPtr;
    tTecla* config = (tTecla*) taskParmPtr;

    // ---------- REPETIR POR SIEMPRE --------------------------
	while (1) {


		if (!gpioRead(config->tecla)) {
					candado_Update_MEF(config->tecla);
				}
		vTaskDelay( 1 / portTICK_RATE_MS );

	}

}


void tareaMostrarEstado(void* taskParmPtr) {

	TickType_t tiempo_inicio;
	tiempo_inicio = xTaskGetTickCount();
	while (1) {
		mostrar_Estado_Candado();
		//cada 60 segundos
		vTaskDelayUntil(&tiempo_inicio, 180000 / portTICK_RATE_MS);
	}

}

//envio solicitud de apertura de la cerradura
void enviarSolcitudAperturaServidor(int valor){

		  int pivot= 0;

		  tcpIpDataToSend[0] = 0; // Reseteo la cadena que guarda las otras agregando un caracter NULL al principio

	      strcat( tcpIpDataToSend, "GET /update?api_key=" );     // Agrego la peticion de escritura de datos

	      strcat( tcpIpDataToSend, THINGSPEAK_WRITE_API_KEY );   // Agrego la clave de escritura del canal

	      strcat( tcpIpDataToSend, "&field" );                   // Agrego field del canal
	      strcat( tcpIpDataToSend, intToString(THINGSPEAK_FIELD_NUMBER_SOLICTUD) );

	      strcat( tcpIpDataToSend, "=" );                        // Agrego el valor a enviar
	      //strcat( tcpIpDataToSend, intToString( sensorValue ) );
	      strcat( tcpIpDataToSend, intToString( valor ) );
	      // Envio los datos TCP/IP al Servidor de Thingpeak
	      // Ver en: https://thingspeak.com/channels/377497/
	      if (esp01SendTPCIPDataToServer( THINGSPEAK_SERVER_URL, THINGSPEAK_SERVER_PORT,
	                                  tcpIpDataToSend, strlen( tcpIpDataToSend ) )){


	      }
	    		  ;
}

int consultarUltimoValor(){

		 int pivot = 0;
		  // GET https://api.thingspeak.com/channels/928937/feeds.json?api_key=C5F6K3EI1E995INB&results=2
		  //GET /channels/928937/fields/1/last?api_key=C5F6K3EI1E995INB
		  tcpIpDataToSend[0] = 0; // Reseteo la cadena que guarda las otras agregando un caracter NULL al principio

		  strcat( tcpIpDataToSend, "GET /channels/928937/fields/2/last?api_key=C5F6K3EI1E995INB" );
	      //strcat( tcpIpDataToSend, "GET /update?api_key=" );     // Agrego la peticion de escritura de datos

	      //strcat( tcpIpDataToSend, THINGSPEAK_WRITE_API_KEY );   // Agrego la clave de escritura del canal

	      //strcat( tcpIpDataToSend, "&field" );                   // Agrego field del canal
	      //strcat( tcpIpDataToSend, intToString(THINGSPEAK_FIELD_NUMBER) );

	      //strcat( tcpIpDataToSend, "=" );                        // Agrego el valor a enviar
	      //strcat( tcpIpDataToSend, intToString( sensorValue ) );
	      //strcat( tcpIpDataToSend, intToString( 45 ) );
	      // Envio los datos TCP/IP al Servidor de Thingpeak
	      // Ver en: https://thingspeak.com/channels/377497/
	      if (preparacionDeDatosParaEnviar( THINGSPEAK_SERVER_URL, THINGSPEAK_SERVER_PORT,
	                                  tcpIpDataToSend, strlen( tcpIpDataToSend ) , &pivot)){

	    	  pivot = consultaUltimoValor();
	    	  //uartWriteString( UART_USB, "dentro del enviarSolcitudAperturaServidor:" );
	    	  //	    	  			printf("%i\n",pivot);
	    	  //meter un delay no bloqueante dutante 2 minutos para que lea una respuesta si abrir o no
	    	  /*if(pivot <= 2){
	    		  /*if( delayRead( 60000 ) ) {

	    		  }
	    		  pivot++;

	    	  }*/
	    	  return pivot;
	      }

}

void enviarDatosAlServidor(int valor, int fiel){

		  int pivot= 0;

		  tcpIpDataToSend[0] = 0; // Reseteo la cadena que guarda las otras agregando un caracter NULL al principio

	      strcat( tcpIpDataToSend, "GET /update?api_key=" );     // Agrego la peticion de escritura de datos

	      strcat( tcpIpDataToSend, THINGSPEAK_WRITE_API_KEY );   // Agrego la clave de escritura del canal

	      strcat( tcpIpDataToSend, "&field" );                   // Agrego field del canal
	      strcat( tcpIpDataToSend, intToString(fiel) );

	      strcat( tcpIpDataToSend, "=" );                        // Agrego el valor a enviar
	      //strcat( tcpIpDataToSend, intToString( sensorValue ) );
	      strcat( tcpIpDataToSend, intToString( valor ) );
	      // Envio los datos TCP/IP al Servidor de Thingpeak
	      // Ver en: https://thingspeak.com/channels/377497/
	      if (esp01SendTPCIPDataToServer( THINGSPEAK_SERVER_URL, THINGSPEAK_SERVER_PORT,
	                                  tcpIpDataToSend, strlen( tcpIpDataToSend ) )){


	      }
	    		  ;
}

void registrarEstadoSd(){
			//rtcRead( &rtc );
	      //showDateAndTime( &rtc );


	    	  //verificar el estado del candado
	    	  char estado[13];
	    	  char str[8];
	    	  sprintf(str, "%d", estadoCandado);
	    	  if (estadoCandado == ESTADO_BLOQUEADO) {

	    	  		strcpy( estado, "BLOQUEADO" );
	    	  	}
	    	  if (estadoCandado == ESTADO_DESBLOQUEADO) {
	    	  		strcpy( estado, "DESBLOQUEADO" );

	    	  	}
	    	  char str2[8];
	    	  	sprintf(str2, "%d", nivelBateria);
	    	  	char str3[8];
	    	  	sprintf(str3, "%d", id_Dispositivo);

	         int nbytes = 0;
	         /*int n = sprintf( buf, "IdDispositivo: %s, Estado: %s, Nivel_bateria: %s  %04d-%02d-%02d %02d:%02d:%02d\r\n",
	            str3,
	        	estado,
				str2,
	            rtc.year,
	            rtc.month,
	            rtc.mday,
	            rtc.hour,
	            rtc.min,
	            rtc.sec
	         );*/

	         //int n = sprintf( buf, "IdDispositivo: %s, Estado: %s, Nivel_bateria: %s\r\n",
	         int n = sprintf( buf, "IdDispositivo: %s, Estado: %s, NivelBateria: %s\r\n",
	         	            str3,
	         	        	estado,
	         				str2
	         	         );


		      if( f_open( &fp, FILENAME, FA_WRITE | FA_OPEN_APPEND ) == FR_OK ){
		         //f_write( &fp, "Hola was\r\n", 12, &nbytes );
		         f_write( &fp, buf, n, &nbytes );

		         f_close(&fp);

		         if( nbytes == n ){
		            // Turn ON LEDG if the write operation was successful
		            //gpioWrite( LEDG, ON );
		            printf("Escribio correctamente\n");
		         }
		      } else{
		         // Turn ON LEDR if the write operation was fail
		         //gpioWrite( LEDR, ON );
		         printf("Escribio %d bytes\n", nbytes);
		      }


}


void registrarSolicitudSd(){

}

void inicializarSD(){
	// SPI configuration
	   //spiConfig( SPI0 );

	   // Initialize SD card driver
	   //FSSDC_InitSPI ();

	   //printf("Inicializamos RTC\n");
	   //rtcInit(); // Inicializar RTC
	   //rtcWrite( &rtc ); // Establecer fecha y hora
	   //delay(2000);
	   //printf("RTC listo\n");
	   //registrarEstadoSd();

	spiConfig( SPI0 );

	   // Inicializar el conteo de Ticks con resolucion de 10ms,
	   // con tickHook diskTickHook
	   //tickConfig( 10 );
	   //tickCallbackSet( diskTickHook, NULL );

	   // ------ PROGRAMA QUE ESCRIBE EN LA SD -------

	   UINT nbytes;

	   // Initialize SD card driver
	   FSSDC_InitSPI ();
	   // Give a work area to the default drive
	   if( f_mount( &fs, "SDC:", 0 ) != FR_OK ) {
	      // If this fails, it means that the function could
	      // not register a file system object.
	      // Check whether the SD card is correctly connected
	   }

	   // Create/open a file, then write a string and close it
	   registrarEstadoSd();

	   /*uint8_t i=0;

	   for( i=0; i<5; i++ ){

	      if( f_open( &fp, FILENAME, FA_WRITE | FA_OPEN_APPEND ) == FR_OK ){
	         f_write( &fp, "Hola was\r\n", 12, &nbytes );

	         f_close(&fp);

	         if( nbytes == 12 ){
	            // Turn ON LEDG if the write operation was successful
	            gpioWrite( LEDG, ON );
	            printf("Escribio correctamente\n");
	         }
	      } else{
	         // Turn ON LEDR if the write operation was fail
	         gpioWrite( LEDR, ON );
	         printf("Escribio %d bytes\n", nbytes);
	      }
	   }*/


}





