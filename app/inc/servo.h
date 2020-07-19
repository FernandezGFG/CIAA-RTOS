/*! \file servo.h
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

#ifndef SERVO_H_
#define SERVO_H_

/*! \def servoANGLE_MIN
	\brief Mínimo ángulo permitido.
*/
#define servoANGLE_MIN		-90
/*! \def servoANGLE_MAX
	\brief Máximo ángulo permitido.
*/
#define servoANGLE_MAX		90

/*! \def servoPWM_PERIOD
	\brief Periodo PWM en ms.
*/
#define servoPWM_PERIOD		20

/*! \def servoPULSE_MIN
	\brief Pulso en ms para menor posición (-90°).
*/
#define servoPULSE_MIN		1
/*! \def servoPULSE_MAX
	\brief Pulso en ms para mayor posición (+90°).
*/
#define servoPULSE_MAX		2

/*! \def servoDUTYCYCLE_MIN
	\brief Duty-cycle para posición -90°.
*/
#define servoDUTYCYCLE_MIN	servoPULSE_MIN*100/servoPWM_PERIOD

/*! \def servoDUTYCYCLE_MAX
	\brief Duty-cycle para posición +90°.
*/
#define servoDUTYCYCLE_MAX	servoPULSE_MAX*100/servoPWM_PERIOD

/*! \def servoSCT_PWM
	\brief Base del periférico SCT en el chip.
*/
#define servoSCT_PWM			LPC_SCT
/*! \def servoSCT_PWM_PIN_SERVO
	\brief Pin de coneción del servo en COUT1 (TFIL_1 o SERVO0).
*/
#define servoSCT_PWM_PIN			1
/*! \def servoSCT_PWM_INDEX
	\brief Conexión en pin con index 2.
*/
#define servoSCT_PWM_INDEX		2

/*! \def servoSCT_PWM_RATE
	\brief Frecuencia del PWM en SCT (50Hz)
*/
#define servoSCT_PWM_RATE		1000/servoPWM_PERIOD

/*!	\def servoSCU_PORT
	\brief Puerto 4 de SCU
*/
#define servoSCU_PORT		0x4
/*!	\def servoSCU_PIN
	\brief Pin 1 de SCU
*/
#define servoSCU_PIN			1

/*! \def servoMAX_SETPOINT_QUEUE_LENGTH
    \brief Máxima cantidad de consignas que se puede almacenar en buffer.
*/
#define servoMAX_SETPOINT_QUEUE_LENGTH    10

/*! \def servoERROR_NOTIF_ANG
	\brief Bit de error de ángulo del motor.
*/
#define servoERROR_NOTIF_ANG	5

/*! \fn void vServoSendMsg( char *pcMsg )
	\brief Enviar consigna a cola de consignas pendientes.
	\param pcMsg String con consigna a enviar.
*/
void vServoSendMsg( char *pcMsg);

/*! \fn BaseType_t xServoInit( void )
	\brief Inicialización de módulo asociado a servomotor.
*/
BaseType_t xServoInit( void );

#endif /* SERVO_H_ */