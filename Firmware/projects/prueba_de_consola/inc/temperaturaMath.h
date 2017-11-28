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

typedef int32_t  temp_t;
typedef float  volt_t;

typedef struct  coeficientes_conversion_Termocupla {
	temp_t decenaCelsius;
	volt_t coefPorUnidadCelsius[10];
} coefConvTer;

#endif /* TEMPERATURAMATH_H_ */
