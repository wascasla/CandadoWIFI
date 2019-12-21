/*=============================================================================
 * Author: was
 * Date: 2019/10/05
 *===========================================================================*/

/*=====[Inclusion de su propia cabecera]=====================================*/

#include "leds.h"

/*=====[Inclusiones de dependencias de funciones privadas]===================*/


/*=====[Macros de definicion de constantes privadas]=========================*/

/*=====[Macros estilo funcion privadas]======================================*/



/*=====[Definiciones de tipos de datos privados]=============================*/

// Tipo de datos que renombra un tipo basico


// Tipo de datos de puntero a funcion


// Tipo de datos enumerado

// Tipo de datos estructua, union o campo de bits

/*=====[Definiciones de Variables globales publicas externas]================*/

/*=====[Definiciones de Variables globales publicas]=========================*/


/*=====[Definiciones de Variables globales privadas]=========================*/



/*=====[Prototipos de funciones privadas]====================================*/

static void toggleLED( gpioMap_t lampara);

/*=====[Implementaciones de funciones publicas]==============================*/

void LED_encender(gpioMap_t lampara){
	//implementación
	gpioWrite( lampara , ON );
}


void LED_apagar( gpioMap_t lampara){
	//implementación
	gpioWrite( lampara , OFF );
}

void LED_titilar( gpioMap_t lampara, int32_t tiempoON, uint8_t cantVeces){

	uint8_t i;

	for(i=0;i<cantVeces;i++){
		toggleLED(lampara);
		delay(tiempoON);
	}

}

/*=====[Implementaciones de funciones de interrupcion publicas]==============*/


/*=====[Implementaciones de funciones privadas]==============================*/

void toggleLED(gpioMap_t lampara){
// si esta encendida la apago y si no la prendo

}
