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

/* Public functions ================================================ */
/*
 * @brief Calcula la resistividad del termistor NTC a partir de los coeficientes de la
 * 		de Steinhart {A,B,C} para esa termistor y la temperatura medida en °C.
 */
double tstorTempToRes (double dT, double dCoeff[]);

/*
 * @brief Calcula la temperatura del termistor NTC a partir de los coficientes de la
 * 		de Steinhart {A,B,C} de ese termistor y la resistencia  medida en ohms.
 */

double tstorResToTemp (double dR, double dCoeff[]);

/*
 * @brief Calcula la temperatura de la termocupla tipo K a partir de la tension medida
 * 		en sus terminales en mV y la temperatura ambiente o del recinto de medicion en °C.
 */
temp_t tcuplaKVoltToTemp (volt_t dVolt, temp_t dTempRef);

/*
 * @brief Calcula la tension que equivalente en los terminales en mV de la termocupla a partir 
 *		de la temperatura mediada y la temperatura ambiente en °C.
 */
volt_t tcuplaKTempToVolt (temp_t dTemp, temp_t dTempRef);




#endif /* TEMPERATURAMATH_H_ */
