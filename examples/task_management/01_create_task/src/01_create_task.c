/*
    Ejemplo 1: Creación de tareas con freeRTOS
    -----------------------------------------------------------
    El ejemplo esta basado en el ejemplo 1 del libro 
    "Masteringthe FreeRTOS™ Real Time Kernel: A Hands-On 
    Tutorial Guide" de Richard Barry, pero con una variación 
    para interactuar con los componentes de la placa de 
    desarrollo EDU-CIAA-NXP con la librería sAPI del 
    firmware_v3.

    En este caso consiste en la creación de dos tareas 
    distintas: La primer realiza un toggle del LED1, y la 
    segunda un toggle del LED2. Ambas tareas mediante un delay 
    "forzado".
    -----------------------------------------------------------
    Autor: Gonzalo G. Fernández
    Fecha: 14/01/2020
    Archivo: 01_create_task.c
    Licencia: MIT
*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/* sAPI library include. */
#include "sapi.h"

/* Variable global para crear un delay "forzado" */
#define mainDELAY_LOOP_COUNT        ( 0xffffff )

/* Tareas a crear */
void vTaskBlinkLed1( void *pvParameters );
void vTaskBlinkLed2( void *pvParameters );

/*-----------------------------------------------------------*/

int main( void )
{
    /* Inicializar la placa */
    boardConfig();

    /* Creación de la primer tarea. */
    xTaskCreate(
        vTaskBlinkLed1, /* Puntero a la función donde se 
        encuentra implementada la tarea. */
        (const char *)"Blink LED1",/* Un nombre descriptivo de 
        la tarea (solo como ayuda en debugueo).
         */
        configMINIMAL_STACK_SIZE*2,
        /* Tamaño del stack asignado a la tarea por el kernel 
        */
        NULL, /* Este ejemplo no recibe parámetros. */
        1, /* Se le asigna una prioridad 1. */
        NULL ); /* No se le pasa task handle */
    
    /* Creación de la segunda tarea. */
    xTaskCreate( 
        vTaskBlinkLed2, (const char *)"Blink LED2",
        configMINIMAL_STACK_SIZE*2, NULL, 1, NULL );
    
    /* Se lanza el scheduler y comienzan a ejecutarse ambas 
    tareas. */
    vTaskStartScheduler();
    
    /* La ejecución nunca debería llegar a este punto, salvo
     que regrese el squeduler por quedarse sin memoria 
     disponible. */
    for( ;; );

    return 0; 
}

/*-----------------------------------------------------------*/
/* Tarea que realiza un toggle del LED1. */
void vTaskBlinkLed1( void *pvParameters )
{
    volatile uint32_t ul; // Variable para loop for

    for( ;; )
    {
        gpioToggle(LED1);

        for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
        {
            /* Este bucle solo provoca un delay "forzado". */
        }
    }
}

/*-----------------------------------------------------------*/
/* Tarea que realiza un toggle del LED2. */
void vTaskBlinkLed2( void *pvParameters )
{
    volatile uint32_t ul; // Variable para loop for

    for( ;; )
    {
        gpioToggle(LED2);

        for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
        {
            /* Este bucle solo provoca un delay "forzado". */
        }
    }
}