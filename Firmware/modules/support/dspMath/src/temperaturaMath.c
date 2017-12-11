/*
 *  Created on: Nov 26, 2017
 *  	Author: fran
 *
 * @file tempCalculos.c
 * @brief NTC thermistor library (Implementation)
 * @version 1.0
 * @copyright GNU Lesser General Public License version 3
 *            <http://www.gnu.org/licenses/lgpl.html>
 * Copyright (c) 2007, 2013 - SoftQuadrat GmbH, Germany
 * Contact: thermistor (at) softquadrat.de
 * Web site: thermistor.sourceforge.net
 */

/** \addtogroup soporte_libs FreeRTOS blink example
 ** @{ */

/*==================[inclusions]=============================================*/
#include "temperaturaMath.h"
#include <math.h>

/* constants ================================================================ */
#define OFFSET_KELVIN 		(-273.15)
#define SIZE_TABLA_CONV 	15

/*==================[internal data declaration]==============================*/
static coefConvTer voltConvTipoK [SIZE_TABLA_CONV]= {
// 		(1)°C  	0	  -1	 -2		-3	   -4	  -5	 -6		-7	   -8	  -9
//(10)°C		
		{-10, {-0.392,-0.431,-0.470,-0.508,-0.547,-0.586,-0.624,-0.663,-0.701,-0.739 }},
		{  0, { 0.000,-0.039,-0.079,-0.118,-0.157,-0.197,-0.236,-0.275,-0.314,-0.353 }},
//	 			0	  +1	 +2		+3		+4	  +5	 +6		+7	   +8	  +9
		{  0, { 0.000, 0.039, 0.079, 0.119, 0.158, 0.198, 0.238, 0.277, 0.317, 0.357 }},
		{ 10, { 0.397, 0.437, 0.477, 0.517, 0.557, 0.597, 0.637, 0.677, 0.718, 0.758 }},
		{ 20, { 0.798, 0.838, 0.879, 0.919, 0.960, 1.000, 1.041, 1.081, 1.122, 1.163 }},
		{ 30, { 1.203, 1.244, 1.285, 1.326, 1.366, 1.407, 1.448, 1.489, 1.530, 1.571 }},
		{ 40, { 1.612, 1.653, 1.694, 1.735, 1.776, 1.817, 1.858, 1.899, 1.941, 1.982 }},
		{ 50, { 2.023, 2.064, 2.106, 2.147, 2.188, 2.230, 2.271, 2.312, 2.354, 2.395 }},
		{ 60, { 2.436, 2.478, 2.519, 2.561, 2.602, 2.644, 2.685, 2.727, 2.768, 2.810 }},
		{ 70, { 2.851, 2.893, 2.934, 2.976, 3.017, 3.059, 3.100, 3.142, 3.184, 3.225 }},
		{ 80, { 3.267, 3.308, 3.350, 3.391, 3.433, 3.474, 3.516, 3.557, 3.599, 3.640 }},
		{ 90, { 3.682, 3.723, 3.765, 3.806, 3.848, 3.889, 3.931, 3.972, 4.013, 4.055 }},
		{100, { 4.096, 4.138, 4.179, 4.220, 4.262, 4.303, 4.344, 4.385, 4.427, 4.468 }},
		{110, { 4.509, 4.550, 4.591, 4.633, 4.674, 4.715, 4.756, 4.797, 4.838, 4.879 }},
		{120, { 4.920, 4.961, 5.002, 5.043, 5.084, 5.124, 5.165, 5.206, 5.247, 5.288 }},
};
// 	difTemp= dTempRef- voltConvTipoK[0].decenaCelsius+10;
/*==================[macros and definitions]=================================*/
#define VOLTAJE_VALIDO_TK(X) (( voltConvTipoK[0].coefPorUnidadCelsius[0] <= X )\
							 && ( voltConvTipoK[SIZE_TABLA_CONV-1].coefPorUnidadCelsius[9] >= X ))
#define TEMP_VALIDA_TK(A) 	 (( voltConvTipoK[0].decenaCelsius-9 <= A )\
		 	 	 	 	 	 && ( voltConvTipoK[SIZE_TABLA_CONV-1].decenaCelsius+9 >= A ))

/*==================[internal functions declaration]=========================*/
static double poly(double x, int degree, double p[]);
static temp_t busquedaBinariaK (volt_t volt_ref);
static temp_t tempFromVoltK (volt_t voltRef);
static volt_t voltFromTempK (temp_t dTempRef);

/* internal public functions ================================================ */
/*
 * @brief Calcula la resistividad del termistor NTC a partir de los coeficientes de la
 * 		ecuacion de Steinhart {A,B,C} y la temperatura medida en °C.
 * @param dT temperatura medida en °C.
 * @param dCoeff coeficientes de la ecuacion de Steinhart.
 * @return valor de la resistencia equivalente en ohms.
 */
double tstorTempToRes (double dT, double dCoeff[])
{
	double r;
	double u, v, p, q, b, c, d;

	dT = dT - OFFSET_KELVIN;
	d = (dCoeff[0] - 1.0 / dT) / dCoeff[3];
	c = dCoeff[1] / dCoeff[3];
	b = dCoeff[2] / dCoeff[3];
	q = 2.0 / 27.0 * b * b * b - 1.0 / 3.0 * b * c + d;
	p = c - 1.0 / 3.0 * b * b;
	v = - pow(q / 2.0 + sqrt(q * q / 4.0 + p * p * p / 27.0), 1.0 / 3.0);
	u =   pow(-q / 2.0 + sqrt(q * q / 4.0 + p * p * p / 27.0), 1.0 / 3.0);
	r  = exp(u + v - b / 3.0);
	return r;
}

// -----------------------------------------------------------------------------
/*
 * @brief Calcula la temperatura del termistor NTC a partir de los coficientes de la
 * 		ecuacion de Steinhart {A,B,C} y la resistencia en ohms.
 * @param dR resitencia medida en ohms.
 * @param dCoeff coeficientes de la ecuacion de Steinhart.
 * @return valor de temperatura en °C.
 */

double tstorResToTemp (double dR, double dCoeff[])
{
	double ti;

	ti = poly(log(dR), 3, dCoeff);
	ti = (1.0 / ti) + OFFSET_KELVIN;
	return ti;
}

// -----------------------------------------------------------------------------
/*
 * @brief Calcula la temperatura de la termocupla tipo K a partir de la tension medida
 * 		en sus terminales en mV y la temperatura ambiente o del recinto de medicion en °C.
 * @param dVolt tension medida en la termocupla en mV.
 * 				Debe estar en el rango de la tabla.
 * @param dTempRef temperatura ambiente de referencia medida en °C.
 * 		  		Debe estar dentro del rango de la tabla de conversion.
 * @return valor de temperatura en °C.
 */
temp_t tcuplaKVoltToTemp (volt_t dVolt, temp_t dTempRef)
{
	volt_t volt_ref;
	temp_t ret_val= ERROR_TEMP;

	if( VOLTAJE_VALIDO_TK (dVolt) && TEMP_VALIDA_TK (dTempRef) )
	{
		// 1- Busca el valor de tension para la temperatura referencia de la termocupla.
		volt_ref = voltFromTempK( dTempRef );
		// 2- Suma este valor a la tension medida.
		volt_ref += dVolt;
		// 3- Busca en la tabla la temperatura para tension total calculada.
		ret_val = tempFromVoltK( volt_ref );
	}
	return ret_val;
}

// -----------------------------------------------------------------------------
/*
 * @brief Calcula la tension que equivalente en los terminales en mV de la termocupla a partir 
 *		de la temperatura medida en la termocupla y la ambiente en °C.
 * @param dTemp temperatura en el punto de medicion en °C.
 * @param dTempRef temperatura de referencia en el ambiente en °C.
 * @return valor de tension en mV.
 */
volt_t tcuplaKTempToVolt (temp_t dTemp, temp_t dTempRef)
{
	volt_t volt_ref= ERROR_VOLT;

	if( TEMP_VALIDA_TK (dTemp) && TEMP_VALIDA_TK (dTempRef) )
	{
		volt_ref  = voltFromTempK( dTemp ) - voltFromTempK( dTempRef );
	}
	return volt_ref;
}



/* private functions ======================================================== */
/*
 * @brief Evaluates p(x) for a polynom p.
 * 		  Calculates the value of polynom p at x accordings to Horners schema.
 * @param p polynom.
 * @param x value to be inserted into the polynom.
 * @return calculated polynom value.
 */
static double poly (double x, int degree, double p[])
{
	double retval = 0.0;
	int i;

	for (i = degree; i >= 0; i--)
	{
		retval = retval * x + p[i];
	}
	return retval;
}

/*
 * @brief Calcula la tension de referencia segun tabla conversion.
 * @param dTempRef temperatura en grados celsius.
 * @return el valor de tension en mV segun tabla K.
 */
static volt_t voltFromTempK (temp_t dTempRef)
{
	temp_t difTemp;
	uint32_t decenaT, unidadT;

 	difTemp= dTempRef-(voltConvTipoK[0].decenaCelsius) + 10;
 	decenaT= difTemp/10;
 	unidadT= difTemp%10;

 	return (voltConvTipoK[decenaT].coefPorUnidadCelsius[unidadT]);

}

static temp_t tempFromVoltK (volt_t voltRef)
{
	temp_t ret_aux= 0;

	if( 0 != voltRef )
	{
		ret_aux= busquedaBinariaK( voltRef );
	}
	return ret_aux;
}


static temp_t busquedaBinariaK (volt_t volt_ref)
{
	int32_t pos_i, pos_j ,inf_i, sup_i, inf_j, sup_j, pos_ret_i, pos_ret_j;
	temp_t ret_val= 0;

	inf_i= 0;
	sup_i= SIZE_TABLA_CONV-1;
	pos_ret_i= -1;

	/* Realiza la busqueda primero buscando entre filas y despues entre columnas.
	 * El valor volt_ref debe ser distinto de 0. De serlo no hace falta calcular
	 * la temperatura referencia para la el caso de la termocupla tipo K.
	 */
	while( inf_i <= sup_i)
	{
		// 1- Comienza la busqueda por la fila central de la tabla de valores.
		pos_i =((sup_i-inf_i)/2)+inf_i;
		inf_j= 0;
		sup_j= 9;

		pos_ret_j= -1;
		while( inf_j <= sup_j )
		{
			// Comienza desde el centro de las columnas.
			pos_j =((sup_j-inf_j)/2)+inf_j;

			if( voltConvTipoK[pos_i].coefPorUnidadCelsius[pos_j] >= volt_ref )
			{
				if( 0 < pos_j )
				{
					if( voltConvTipoK[pos_i].coefPorUnidadCelsius[pos_j-1] <= volt_ref)
					{
						pos_ret_j= pos_j;
						break;
					}
				}
			}
			if( volt_ref < voltConvTipoK[pos_i].coefPorUnidadCelsius[pos_j] )
			{
				// Esto se debe a como estan ordenados los datos en la tabla.
				(volt_ref<= 0)? inf_j =pos_j+1: (sup_j =(pos_j-1));
			}
			else
			{
				// Esto se debe a como estan ordenados los datos en la tabla.
				(volt_ref<= 0)? sup_j =pos_j-1: (inf_j =(pos_j+1));
			}
		}

		if(-1 != pos_ret_j)
		{
			pos_ret_i= pos_i;
			break;
		}
		else
		{
			if( volt_ref < voltConvTipoK[pos_i].coefPorUnidadCelsius[0] )
			{
				sup_i =pos_i-1;
			}
			else
			{
				inf_i =pos_i+1;
			}
		}
	}

	if(-1 != pos_ret_i)
	{
	// Retorna la temperatura como la suma del campo decena mas la unidad (como el numero de columna hallado).
		if( 0 >= volt_ref )
		{
			// Esto se debe a como estan ordenado los datos en la tabla.
			pos_ret_j= -pos_ret_j;
		}
		ret_val= (temp_t)( voltConvTipoK[pos_ret_i].decenaCelsius + (pos_ret_j) );
	}

	return ret_val;
}
/*==================[irq handlers functions ]=========================*/








/** @} doxygen end group definition */

/*==================[end of file]============================================*/

