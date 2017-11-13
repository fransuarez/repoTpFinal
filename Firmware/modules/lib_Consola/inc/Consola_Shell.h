/*
 * consola.h
 *
 *  Created on: 22/10/2014
 *      Author: Seba
 */

#ifndef CONSOLA_SHELL_H_
#define CONSOLA_SHELL_H_

#include "UART.h"
//Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Definicion del RTOS a usar --------------------------------------------------
#if __USE_FREERTOS == FreeRTOSv7p0p1
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#endif


#include "ntshell.h"
#include "ntopt.h"

/************************************************************************/
#define L_FIFO_UART 	300
#define AUX_ARGC		32

typedef struct
{
    char *command;
    char *description;
    void (*func) (int argc, char **argv);

} command_table_t;

typedef enum modeprint
{
	MP_DEF=0,
	MP_DEB,
	MP_EST,
	MP_SIN_NL

} modep_t;

typedef enum modeinput
{
	CONSOLA,
	INTERNO,
	PROCESANDO,
	FIN_PROCESO

} modein_t;
/************************************************************************/
extern UART 	 	consola;
extern command_table_t * ptr_commandList;
xSemaphoreHandle mutexConsola;

//extern const command_table_t cmdlist[];
/************************************************************************/
void 	Consola_Task 		( void * parametrosTarea );
int		func_read			( char *buf, int cnt );
int		func_write			( const char *buf, int cnt );
int		func_cb_ntshell		( const char *text );
void	func_cb_ntopt		( int argc, char **argv );

void 	printConsola (const char *texto, modep_t modo_impresion);
int 	sendConsola (char *);

#endif /* CONSOLA_H_ */
