/*=============================================================================
 * Author: was
 * Date: 2019/10/05
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __CANDADO_H__
#define __CANDADO_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "sapi.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
typedef struct
{
	gpioMap_t tecla;			//config

} tTecla;

/*=====[Prototypes (declarations) of public functions]=======================*/
void bloquear_Candado(void);

void desbloquear_Candado(void);

void recargar_Bateria(void);

void descargar_bateria(void);

void obtener_Estado_Candado(void);

void candado_inicializar_MEF(void);

//void candado_Update_MEF(uint8_t);
void candado_Update_MEF(gpioMap_t teclaPresionada);
void prenderLed(gpioMap_t teclaPresionada);

void myTask( void* taskParmPtr );
void myTask2( void* taskParmPtr );
void myTask3( void* taskParmPtr );
void tareaMostrarEstado( void* taskParmPtr );
void enviarSolcitudAperturaServidor(int valor);
int consultarUltimoValor();
void enviarDatosAlServidor(int valor, int fiel);
void registrarEstadoSd();
void registrarSolicitudSd();
void inicializarSD();

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __CANDADO_H__ */
