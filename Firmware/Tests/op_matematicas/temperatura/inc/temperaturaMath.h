/*
 *
 *  Created on: Nov 26, 2017
 *      Author: fran
 * @file temperaturaMath.h
 * @brief
 *
 */

#ifndef TEMPERATURAMATH_H_
#define TEMPERATURAMATH_H_

#define ERROR_TEMP			-200
#define ERROR_VOLT			-100


typedef int32_t  temp_t;
typedef float  volt_t;

typedef struct  coeficientes_conversion_Termocupla
{
	// Decada de grados.
	temp_t decenaCelsius;
	// Vector de tensiones por unidad de grado en milivolts.
	volt_t coefPorUnidadCelsius[10];
} coefConvTer;

#endif /* TEMPERATURAMATH_H_ */
