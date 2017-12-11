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
#include "temperaturaMath.h"

static int test1 (volt_t voltajeMedido, temp_t tempAmbiente);

int main(void) 
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


static int test1 (volt_t voltajeMedido, temp_t tempAmbiente)
{
	temp_t tempaux;
	volt_t voltaux, volterror;
	int retval= 0;
	
	printf( "Temperatura medida: %i\n", tempAmbiente );
	printf( "Voltaje medido: %f\n", voltajeMedido );
	tempaux= tcuplaKVoltToTemp( voltajeMedido, tempAmbiente );

	if( ERROR_TEMP != tempaux ) 
	{
		printf( "Temperatura devuelta: %i\n", tempaux ); 
		voltaux= tcuplaKTempToVolt( tempaux, tempAmbiente );
		
		if( ERROR_VOLT != voltaux )
		{
			printf( "Voltaje devuelto: %f\n", voltaux ); 
			//volterror= abs( voltajeMedido-voltaux );
			volterror= voltajeMedido-voltaux;
			printf( "Error calculado: %f\n", volterror ); 
			if( 10 > volterror )
			{
				retval= 1;
			}
		}
	}
	return retval;
}

