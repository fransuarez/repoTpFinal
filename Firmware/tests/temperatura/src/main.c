/*
 ============================================================================
 Name        : temperatura.c
 Author      : Francisco
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "temperaturaMath.h"
#define  RET_ERROR   ( 0 )
#define  RET_VOID    ( 1 )
#define  RET_OK      ( 2 )


typedef struct dataprocess
{
	// Datos de entrada:
	temp_t tempAmb;
	volt_t voltTcupla;
	// Datos de salida:
	temp_t tempCalc;
	volt_t voltCalc; 
	volt_t voltError;
} dproc_t;

static int 	test1 			( volt_t , temp_t );
static int 	processBuffer 	( dproc_t *, char * , int  );
static int 	processDataOk	( dproc_t *, char *  );
static int	processDataError( dproc_t *, char * );

int main (void) 
{

	if( test1( (volt_t) 2.602 , (temp_t) 25) )
	{
		puts("Test 1 OK"); 
	}
	else
	{
		puts("Fallo test 1"); /* prints Hello World */
	}

	//while(1);		
	return EXIT_SUCCESS;
}

#define SIZE_BUFF 		100
static int test1 (volt_t voltajeMedido, temp_t tempAmbiente)
{
	int sizeMsg, retaux, retval= RET_ERROR;
	FILE * fd_data_input;
	FILE * fd_data_output;
	dproc_t data;
	char data_inp_buffer[SIZE_BUFF];
	char data_out_buffer[SIZE_BUFF];
	
	fd_data_input = fopen ( "sim_voltage_tcupla.svc", "r+" );
	if( NULL !=  fd_data_input )
	{
		fd_data_output = fopen ( "process_tem_tcupla.txt", "w+" );
		if( NULL !=  fd_data_output )
		{
			fputs( "T_ref \t V_med \t\tT_cal \t V_cal \t\t V_error \n--------------------------------------------------\n", fd_data_output );
			while( !feof( fd_data_input ) )
			{
				if( NULL != fgets( data_inp_buffer, SIZE_BUFF, fd_data_input ) )
				{
					sizeMsg= strlen( data_inp_buffer );
					//data_inp_buffer [sizeMsg-1] = '\0';
					retaux= processBuffer( &data, data_inp_buffer, sizeMsg );
					if( RET_OK == retaux )
					{
						sizeMsg= processDataOk( &data, data_out_buffer );
						if( RET_ERROR != sizeMsg )
						{
							fputs( data_out_buffer, fd_data_output );
							retval= RET_OK;
						}
					}
					else if( RET_ERROR == retaux )
					{
						sizeMsg= processDataError( &data, data_out_buffer );
						if( RET_ERROR != sizeMsg )
						{
							fputs( data_out_buffer, fd_data_output );
						}
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			printf( "\n\rNo se pudo abrir process_tem_tcupla.txt\n" );
		}
	}
	else
	{
		printf( "\n\rNo se pudo abrir sim_voltage_tcupla.svc\n" );
	}

	fclose ( fd_data_input );
	fclose ( fd_data_output );

	return retval;
}


static int processBuffer ( dproc_t *data, char * buffer, int sizeBuff )		
{
	char * pTemp, *pVol;
	volt_t voltaux;
	temp_t tempaux;
	int retval= RET_VOID;

	if( NULL != ( pTemp = strchr( buffer, '@' ) ) )
	{
		if( NULL != ( pVol = strchr( pTemp, 'V' ) ) )
		{
			*pVol='\0';
			data->tempAmb = atoi( ++pTemp );
			data->voltTcupla = atof( ++pVol );

			retval= RET_ERROR;
			tempaux= tcuplaKVoltToTemp( data->voltTcupla, data->tempAmb );
			if( ERROR_TEMP != tempaux )
			{
				data->tempCalc= tempaux;

				voltaux= tcuplaKTempToVolt( tempaux, data->tempAmb );
				if( ERROR_VOLT != voltaux )
				{
					data->voltCalc= voltaux;
					data->voltError= fabs(voltaux - data->voltTcupla);
					retval= RET_OK;
				}	
			}
		}
	}
	return retval;
}

static int	processDataOk( dproc_t *data, char * buffer )
{
	if( 0 < sprintf( buffer, "@%d°C\t %1.3fmV >> %d°C \t %1.3fmV \t @E %1.3fmV \n",
		data->tempAmb, data->voltTcupla,data->tempCalc, data->voltCalc, data->voltError) )
	{
		return strlen( buffer );
	}
	return RET_ERROR;
}

static int	processDataError( dproc_t *data, char * buffer )
{
	if( 0 < sprintf( buffer, "@%d°C\t %1.3fmV >> Error\t Error\t Error \n",
		data->tempAmb, data->voltTcupla) )
	{
		return strlen( buffer );
	}
	return RET_ERROR;
}
