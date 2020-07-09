/*! \file stepper.c
    \brief Descripción del archivo.
    \author Gonzalo G. Fernández
    \version 1.0
    \date Julio 2020

    Detalle.
*/

/* Utilidades includes */
#include <stdlib.h>
#include <string.h>

/* Aplicación includes */
#include "stepper.h"

/*! \def stepperANGLE_TO_STEPS( X )
    \brief Macro para convertir ángulos en cantidad de pasos del motor.
*/
#define stepperANGLE_TO_STEPS( x )  4096*( x )/360

/*! \var typedef struct xStepperData StepperData_t
    \brief Estructura de datos con información del stepper.
*/
typedef struct xStepperData {
    /* Estado actual de entradas al driver */
    char cDriverState;
    /* Cantidad de pasos pendientes */
    uint32_t ulPendingSteps;
    /* Dirección en que se deben realizar los
    pasos pendientes */
    StepperDir_t xDir;
    /* Posición absoluta del motor */
    int32_t lAbsPosition;
} StepperData_t;

/*! \var TimerHandle_t xStepperTimer[stepperAPP_NUM]
    \brief Array de handles de los timers asociados a los motores.
*/
TimerHandle_t xStepperTimer[stepperAPP_NUM];

/*! \var StepperData_t xStepperDataID[stepperAPP_NUM]
    \brief Instanciación de información de motores 
*/
StepperData_t xStepperDataID[stepperAPP_NUM];

/*! \var QueueHandle_t xStepperSetPointQueue
    \brief Cola de consignas recibidas a ejecutar.
*/
QueueHandle_t xStepperSetPointQueue;

/*! \fn void vStepperDriverUpdate( uint8_t cState )
    \brief Escritura en driver de motor stepper dado un determinado estado.
    \param cState Estado a escribir en el driver.
*/
void vStepperDriverUpdate( uint8_t cState )
{
    switch ( cState ) {
        case 0:
            gpioWrite(GPIO0, ON);
            gpioWrite(GPIO1, ON);
            gpioWrite(GPIO2, OFF);
            gpioWrite(GPIO3, OFF);
            break;

        case 1:
            gpioWrite(GPIO0, OFF);
            gpioWrite(GPIO1, ON);
            gpioWrite(GPIO2, OFF);
            gpioWrite(GPIO3, OFF);
            break;

        case 2:
            gpioWrite(GPIO0, OFF);
            gpioWrite(GPIO1, ON);
            gpioWrite(GPIO2, ON);
            gpioWrite(GPIO3, OFF);
            break;

        case 3:
            gpioWrite(GPIO0, OFF);
            gpioWrite(GPIO1, OFF);
            gpioWrite(GPIO2, ON);
            gpioWrite(GPIO3, OFF);
            break;

        case 4:
            gpioWrite(GPIO0, OFF);
            gpioWrite(GPIO1, OFF);
            gpioWrite(GPIO2, ON);
            gpioWrite(GPIO3, ON);
            break;

        case 5:
            gpioWrite(GPIO0, OFF);
            gpioWrite(GPIO1, OFF);
            gpioWrite(GPIO2, OFF);
            gpioWrite(GPIO3, ON);
            break;

        case 6:
            gpioWrite(GPIO0, ON);
            gpioWrite(GPIO1, OFF);
            gpioWrite(GPIO2, OFF);
            gpioWrite(GPIO3, ON);
            break;

        case 7:
            gpioWrite(GPIO0, ON);
            gpioWrite(GPIO1, OFF);
            gpioWrite(GPIO2, OFF);
            gpioWrite(GPIO3, OFF);
            break;
    }
}

/*! \fn void vStepperRelativeSetPoint( TimerHandle_t xStepperTimer, int32_t lRelativeSetPoint )
    \brief Setear una nueva consigna en motor stepper relativa a la posición actual.
    \param xStepperTimer Handle del timer asociado al motor que se desea asignar la nueva consigna.
    \param ulRelativeSetPoint Cantidad de pasos a realizar.
    \param xStepperDir Dirección de los pasos a realizar.
*/
BaseType_t xStepperRelativeSetPoint( TimerHandle_t xStepperTimer, uint32_t ulRelativeSetPoint, StepperDir_t xStepperDir )
{
    /* Estructura de datos a obtener del timer ID */
    StepperData_t *xStepperDataID;
    xStepperDataID = ( StepperData_t * ) pvTimerGetTimerID( xStepperTimer );
    /* Verificación de consigna nula */
    if ( ulRelativeSetPoint == 0 ) {
        return pdPASS;
    }
    /* Seteo de consigna como pasos pendientes y dirección */
    xStepperDataID->ulPendingSteps = ulRelativeSetPoint;
    xStepperDataID->xDir = xStepperDir;
    /* Actualización de posición absoluta */
    // Asumiendo que se va a finalizar la consigna!!
    xStepperDataID->lAbsPosition += xStepperDir * ulRelativeSetPoint;

    /* Inicialización de timer */
    return xTimerStart( xStepperTimer, 0 );
}

/*! \fn void prvStepperTimerCallback( TimerHandle_t xStepperTimer )
    \brief Función de callback que ejecutará cada timer al entrar en estado Running.
*/
static void prvStepperTimerCallback( TimerHandle_t xStepperTimer )
{
    /* Estructura de datos a obtener del timer ID */
    StepperData_t *xStepperDataID;
    xStepperDataID = ( StepperData_t * ) pvTimerGetTimerID( xStepperTimer );
    /* Verificación de existencia de pasos pendientes */
    if ( xStepperDataID->ulPendingSteps == 0 ) {
        /* Detener timer si no existen pasos pendientes */
        xTimerStop( xStepperTimer, 0 );
        return;
    }
    /* Cálculo de nuevo estado del driver según dirección */
    if ( xStepperDataID->xDir == stepperDIR_NEGATIVE ) {
        if ( xStepperDataID->cDriverState == 0 ) {
            xStepperDataID->cDriverState = 7;
        } else {
            xStepperDataID->cDriverState--;
        }
    } else if ( xStepperDataID->xDir == stepperDIR_POSITIVE ) {
        if ( xStepperDataID->cDriverState == 7 ) {
            xStepperDataID->cDriverState = 0;
        } else {
            xStepperDataID->cDriverState++;
        }
    }
    /* Actualización del driver */
    vStepperDriverUpdate( xStepperDataID->cDriverState );
    /* Decremento de pasos pendientes a realizar */
    xStepperDataID->ulPendingSteps--;
    /* Actualización del timer ID */
    vTimerSetTimerID( xStepperTimer, ( void * ) xStepperDataID );
}

/*! \fn void vStepperControlTask( void *pvParameters )
    \brief Tarea encargada del control en el flujo de trabajo de los motores stepper, gestionando consignas y todo procesamiento relacionado con ellos.
*/
void vStepperControlTask( void *pvParameters )
{
    /* Creación de cola de consignas recibidas a ejecutar */
    xStepperSetPointQueue = xQueueCreate(
        /* Longitud máxima de la cola */
        stepperMAX_SETPOINT_QUEUE_LENGTH,
        /* Tamaño de elementos a guardar en cola */
        sizeof( char * )
    );

    /* Puntero a consignas recibidas */
    char *pcReceivedSetPoint;

    /* ID del motor recibida */
    uint8_t cID;
    /* Dirección */
    StepperDir_t xDir;
    /* Ángulo a realizar */
    uint32_t ulAngle;

    for ( ;; ) {
        // /* Lectura de cola de consignas */
        // xQueueReceive(
        //     /* Handle de la cola a leer */
        //     xStepperSetPointQueue,
        //     /* Elemento donde guardar información leída */
        //     &pcReceivedSetPoint,
        //     /* Máxima cantidad de tiempo a esperar por una lectura */
        //     portMAX_DELAY
        // );

        // cID = atoi( &pcReceivedSetPoint[3] );
        // xDir = atoi( &pcReceivedSetPoint[5] );
        // ulAngle = atoi( &pcReceivedSetPoint[7] );
        // ulAngle = stepperANGLE_TO_STEPS( ulAngle );
        xStepperRelativeSetPoint( xStepperTimer[0], stepperANGLE_TO_STEPS( 45 ), stepperDIR_POSITIVE );

        vTaskDelay( pdMS_TO_TICKS(20000) );
    }
}

/*! \fn BaseType_t xStepperInit( void )
    \brief Inicialización de motores de la aplicación.
*/
BaseType_t xStepperInit( void )
{
    /* Inicialización de salidas a driver del stepper */
    gpioInit( GPIO0, GPIO_OUTPUT );
    gpioInit( GPIO1, GPIO_OUTPUT );
    gpioInit( GPIO2, GPIO_OUTPUT );
    gpioInit( GPIO3, GPIO_OUTPUT );

    /* Creación de tarea de control de flujo de trabajo de los motores stepper */
    BaseType_t xStatus;
    xStatus = xTaskCreate(
        /* Puntero a la función que implementa la tarea */
        vStepperControlTask,
        /* Nombre de la tarea amigable para el usuario */
        ( const char * ) "StepperControlTask",
        /* Tamaño de stack de la tarea */
        configMINIMAL_STACK_SIZE*2,
        /* Parámetros de la tarea */
        NULL,
        /* Prioridad de la tarea */
        tskIDLE_PRIORITY+5,
        /* Handle de la tarea creada */
        NULL
    );

    if ( xStatus == pdFAIL ) {
        return pdFAIL;
    }
    
    char pcAuxTimerName[15];
    for (uint8_t i=0; i<stepperAPP_NUM; i++) {
        strcpy( pcAuxTimerName, "StepperTimer" );
        strcat( pcAuxTimerName, "0" + i );
        /* Creación de los software timers */
        xStepperTimer[i] = xTimerCreate(
            /* Nombre descriptivo del timer */
            (const char *)pcAuxTimerName,
            /* Periodo del timer especificado en ticks */
            pdMS_TO_TICKS( stepperTIMER_PERIOD ),
            /* pdTRUE para timer tipo auto-reload y pdFALSE para tipo one-shoot */
            pdTRUE,
            /* Valor de ID del timer */
            ( void * ) &xStepperDataID[i],
            /* Función de callback del timer */
            prvStepperTimerCallback
        );

        if ( xStepperTimer[i] == NULL ) {
            /* Error al crear timer */
            return pdFAIL;
        }
    }
    /* Inicialización exitosa */
    return pdTRUE;
}