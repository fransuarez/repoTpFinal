/*
 * consola.c
 *
 *  Created on: 22/10/2014
 *      Author: Seba
 */

#include "Consola_Shell.h"

ntshell_t 		ntshell;
UART 			consola;
command_table_t *ptr_commandList;

xTaskHandle 	pxCreatedTask2;
xSemaphoreHandle mutexConsola;
extern unsigned long *ptrstack;
//static ColaCircular  inputProgBuff;
static modein_t		 inputDatos= CONSOLA;
static char 		 buffInpProg[L_FIFO_UART];
static char * 		 auxargv [AUX_ARGC];
static int 			 auxargc;
static int			 nOverFullCS;


static int chequearUART (void) ;

/************************************************************************/
void Consola_Task (void * parametrosTarea)
{
	//char chAux;
	//ColaCircular_Init (&inputProgBuff, buffInpProg, L_FIFO_UART);

	while(1) {
		/*
		switch (inputDatos) {
		case INTERNO:
			vTaskSuspendAll();
			//taskENTER_CRITICAL();
			inputDatos = PROCESANDO;
			break;
		case FIN_PROCESO:
			inputDatos= CONSOLA;
			xTaskResumeAll();
			//taskEXIT_CRITICAL();
			break;
		case PROCESANDO:
		case CONSOLA:
		default:
			break;
		}
		*/
		ntshell_execute( &ntshell, func_read, func_write, func_cb_ntshell );

		if (inputDatos== INTERNO) {
			xSemaphoreTake( mutexConsola, ( portTickType ) 10  );
			func_cb_ntopt (auxargc, auxargv);
			inputDatos= CONSOLA;
			xSemaphoreGive( mutexConsola );
		}
		*(ptrstack+1)= uxTaskGetStackHighWaterMark(pxCreatedTask2 );

		//vTaskDelay(10);
		taskYIELD();
	}
}

/**** Serial read function *****/
int func_read (char *buf, int cnt)
{
	/*
	switch (inputDatos) {
	case INTERNO:
		if (ColaCantidad (&inputProgBuff)) {
		    *buf= ColaSacar (&inputProgBuff);
		    //if (*buf == CAPI_ERROR) break;
		}
		else {
			*buf= CAPI_ERROR;
			inputDatos= CONSOLA;
		}
		break;
	case CONSOLA:
		*buf= UARTgetc (&consola);
		break;
	default:
		*buf= CAPI_ERROR;
		break;
	}
	*/
	*buf= UARTgetc (&consola);
    return 0;

//	while(!ColaVacia(&uart->colaRx))
//	{
//		UARTputc(uart, UARTgetc(uart));
//	}
}

/**** Serial write function ****/
int func_write (const char *buf, int cnt)
{
	int i;
	xSemaphoreTake( mutexConsola, ( portTickType ) 10  );
	if (chequearUART()!=0);
		//UARTputs (&consola, "\n\r-->Correccion de Cola Tx.");

    for ( i = 0; i < cnt; i++ )
    {
    	if (UARTputc(&consola, buf[i])== CAPI_ERROR)
    		return 1;
    }
    xSemaphoreGive( mutexConsola );
    return 0;
}

/**** Callback function for ntshell module ****/
int func_cb_ntshell (const char *text)
{
    return ntopt_parse(text, func_cb_ntopt);
}

/**** Callback function for ntopt module ****/
void func_cb_ntopt (int argc, char **argv)
{
    int   execnt = 0;
    command_table_t *p = ptr_commandList;

    if (argc == 0) return;

    while (p->command != NULL)
    {
        if (strcmp (argv[0], p->command) == 0) {
            p->func(argc, argv);
            execnt++;
            break;
        }
        p++;
    }
    if (execnt == 0)
        UARTputs (&consola, "\n\rCommand not found.");
}

/************************************************************************/
void printConsola(const char * texto, modep_t mode)
{
	//taskENTER_CRITICAL();
	if (mutexConsola == NULL) return;

	xSemaphoreTake( mutexConsola, ( portTickType ) 10  );
	//vTaskSuspendAll();

	if (chequearUART()!=0);
		//UARTputs (&consola, "\n\r"); //-->Correccion de Cola Tx

	switch (mode) {
	    case MP_DEB:
	    	UARTputc( &consola, '\r');
	    	UARTputc( &consola, '\n');
	    	UARTputs( &consola, "->");
	    	break;
	    case MP_EST:
	    	UARTputc( &consola, '\r');
	    	UARTputc( &consola, '\n');
	    	UARTputs( &consola, ">");
	    	break;
	    case MP_SIN_NL:
	    	UARTputc( &consola, ' ');
	    	break;
	    case MP_DEF:
	    default:
	    	UARTputc( &consola, '\r');
	    	UARTputc( &consola, '\n');
	    	UARTputc( &consola, ' ');
	}
	if (UARTputs( &consola, texto) == CAPI_ERROR)
		nOverFullCS++;
		//while (1);

	//xTaskResumeAll();
	xSemaphoreGive( mutexConsola );
}

int chequearUART (void)
{
	if (consola.colaTx.cantidadEnCola == 0 && consola.colaTx.pOut != consola.colaTx.pIn) {
		consola.colaTx.pOut= consola.colaTx.pIn;
		return 1;
	}
	else return 0;
}

int sendConsola (char * string)
{
	char * nextWord;
	int i=0;

	if (string == NULL) return 0;
/*
	length= strlen (string);
	if (ColaDisponible (&inputProgBuff) < length+2)
		return 0;

	else {
		while(*string)
			ColaPoner (&inputProgBuff, *string++);
		ColaPoner (&inputProgBuff, '\r');
		ColaPoner (&inputProgBuff, '\n');

		//UARTputc( &consola, '\r');UARTputc( &consola, '\n');UARTputc( &consola, '>');
		// inputDatos= INTERNO;
		// return 1;
	}
	*/
	strcpy (buffInpProg, string);
	//auxargv[i++]= string;

	nextWord= strtok (buffInpProg, " .");
	while (nextWord != NULL && i< AUX_ARGC) {
		//printf ("%s\n",nextWord);
		auxargv[i++]= nextWord;
		nextWord= strtok (NULL, " .");
	}
	auxargc= i;
	inputDatos= INTERNO;

	// Blanqueo los demas punteros:
	if (i< AUX_ARGC) {
		while (i< AUX_ARGC)
			auxargv[i++]= NULL;
	}

	return 1;
}
