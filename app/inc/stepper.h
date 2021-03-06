/*! \file stepper.h
    \brief Tarea, timers, y funciones asociado al control
    de los motores paso a paso.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020
*/

#ifndef STEPPER_H_
#define STEPPER_H_

/* FreeRTOS includes */
#include "FreeRTOS.h"

/*! \def stepperAPP_NUM
    \brief Cantidad de motores en la aplicación
*/
#define stepperAPP_NUM  3

/*! \def stepperTIMER_PERIOD
    \brief Periodo de pasos de los motores (velocidad del motor)
*/
#define stepperTIMER_PERIOD 2

/*! \def stepperTIMER_MIN_PERIOD
	\brief Período mínimo de timer.
*/
#define stepperTIMER_MIN_PERIOD	1

/*! \def stepperTIMER_MAX_PERIOD
	\brief Período máximo de timer.
*/
#define stepperTIMER_MAX_PERIOD	4


/*! \def stepperDIR_NEGATIVE
    \brief Dirección negativa del motor (depende de conexión del driver)
*/
#define stepperDIR_NEGATIVE 0

/*! \def stepperDIR_POSITIVE
    \brief Dirección negativa del motor (depende de conexión del driver)
*/
#define stepperDIR_POSITIVE 1

/*! \def stepperMAX_SETPOINT_QUEUE_LENGTH
    \brief Máxima cantidad de consignas que se puede almacenar en buffer.
*/
#define stepperMAX_SETPOINT_QUEUE_LENGTH    10

/*! \def stepperERROR_NOTIF_ID
	\brief Bit de error de ID del motor.
*/
#define stepperERROR_NOTIF_ID	1

/*! \def stepperERROR_NOTIF_DIR
	\brief Bit de error de dirección del motor.
*/
#define stepperERROR_NOTIF_DIR	2

/*! \def stepperERROR_NOTIF_VEL
	\brief Bit de error de velocidad del motor.
*/
#define stepperERROR_NOTIF_VEL	3

/*! \def stepperERROR_NOTIF_ANG
	\brief Bit de error de ángulo del motor.
*/
#define stepperERROR_NOTIF_ANG	4

/*! \var typedef char StepperDir_t
    \brief Definición de tipo para dirección de motores stepper.
*/
typedef char StepperDir_t;

/*! \fn uint32_t ulStepperGetAngle( uint8_t ucStepperIndex )
	\brief Obtener ángulo pendiente de motor paso a paso.
	\param ucStepperIndex Índice del motor paso a paso.
	\return Pasos pendientes del motor en forma de ángulo.
*/
uint32_t ulStepperGetAngle( uint8_t ucStepperIndex );

/*! \fn void vStepperSendMsg( char *pcMsg )
	\brief Enviar consigna a cola de consignas pendientes.
	\param pcMsg String con consigna a enviar.
*/
void vStepperSendMsg( char *pcMsg );

/*! \fn BaseType_t xStepperInit( void )
    \brief Inicialización de motores de la aplicación.
*/
BaseType_t xStepperInit( void );

#endif /* STEPPER_H_ */
