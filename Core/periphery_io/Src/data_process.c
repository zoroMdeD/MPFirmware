/*
 * data_process.c
 *
 *  Created on: 6 авг. 2022 г.
 *      Author: moroz
 */
#include "../periphery_io/Inc/data_process.h"

extern double adcValue[];
extern double Current;
extern double Amps[];
extern bool Stop;

//Функция преобразования данных АЦП
//Принимает "ADC_value" - значение АЦП
//Возвращает преобразованное значение в амперах
double Conversion_ADC1(uint16_t ADC_value)
{
	//The average point is 1605mV
	//Sensitivity 66mV
	//Gain 1.56

	double Amps = 0;
	double Voltage = 0;
	const double Resolution = 0.8056640625;
	Voltage = ADC_value * Resolution;

	(Voltage >= 1605) ? (Amps += ((Voltage - 1605)/66)*1.56) : (Amps += ((1605 - Voltage)/66)*1.56);

	return Amps;
}
//Функция сравнения значения тока уставки с действующими значениями тока на фазах
void Сurrent_Сomparison(void)
{
	double reserve_Current = Current * 1.20;	//Если значение тока превысило уставку на 20%
	(Amps[0] > reserve_Current || Amps[1] > reserve_Current || Amps[2] > reserve_Current) ? (Stop = true) : (Stop = false);
}
