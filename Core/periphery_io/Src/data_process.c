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
extern uint8_t cnt;
extern bool run_Сomparison;
extern char trans_str[];
extern double reserve_Current;

//Функция преобразования данных АЦП
//Принимает "ADC_value" - значение АЦП
//Возвращает преобразованное значение в амперах
double ConversionADC(uint16_t ADC_value)
{
	/*
	 * The average point is 1605mV
	 * Sensitivity 66mV
	 * Gain 1.56
	 */

	double Amps = 0;
	double Voltage = 0;
	const double Resolution = 0.8056640625;
	Voltage = ADC_value * Resolution;

	(Voltage >= 1605) ? (Amps += ((Voltage - 1605)/66)*1.56) : (Amps += ((1605 - Voltage)/66)*1.56);

	return Amps;
}
//Функция сравнения значения тока уставки с действующими значениями тока на фазах
void СurrentСomparison(void)
{
    if(cnt == 10)
    {
    	run_Сomparison = true;

    	Amps[0] = adcValue[0]/10;
    	Amps[1] = adcValue[1]/10;
    	Amps[2] = adcValue[2]/10;

      	adcValue[0] = 0.0;
      	adcValue[1] = 0.0;
      	adcValue[2] = 0.0;

    	cnt = 0;
    }

    if(run_Сomparison)
    	(Amps[0] > reserve_Current || Amps[1] > reserve_Current || Amps[2] > reserve_Current) ? (Stop = true) : (Stop = false);
}
