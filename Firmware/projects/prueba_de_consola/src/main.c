/* Copyright 2015, Pablo Ridolfi
 * All rights reserved.
 *
 * This file is part of lpc1769_template.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @brief Blinky using FreeRTOS.
 *
 *
 * NOTE: It's interesting to check behavior differences between standard and
 * tickless mode. Set @ref configUSE_TICKLESS_IDLE to 1, increment a counter
 * in @ref vApplicationTickHook and print the counter value every second
 * inside a task. In standard mode the counter will have a value around 1000.
 * In tickless mode, it will be around 25.
 *
 */

/** \addtogroup rtos_blink FreeRTOS blink example
 ** @{ */

/*==================[inclusions]=============================================*/
#include <main.h>
/*==================[macros and definitions]=================================*/
#define MAX_UINT32  (2^32)-1

#define TWAIT	   	20
#define PERIODO	  	1000

/*==================[internal data declaration]==============================*/
xQueueHandle queueKeyPad, queueSigLed;

/*==================[internal functions declaration]=========================*/
static void taskControlLed(void * a)
{
	signal_t dataRecLed;
	int32_t  tOnLeds [4];
	char sToSend[30];
	uint8_t  i;

	while (1)
	{
		if( pdTRUE == xQueueReceive( queueSigLed, &dataRecLed, TWAIT )){
			if(dataRecLed.deltaT >0){
				tOnLeds[dataRecLed.led-FIRSTLED]= (int32_t)dataRecLed.deltaT;
				Board_LED_Set( dataRecLed.led, true );

				sprintf(sToSend, "[P%d, %lu mS]\r\n", dataRecLed.led-FIRSTLED+1, dataRecLed.deltaT);
				dbgPrint(sToSend);
			}
		}
		for (i = 0; i < 4; ++i) {
			if( tOnLeds[i] > 0 )
				tOnLeds[i] -= TWAIT;
			else
				Board_LED_Set( i+FIRSTLED, false );
		}
	}
}

static void taskDetectPulse(void * a)
{
	TickType_t tStart, tEnd;
	queue_t dataRecKey;
	signal_t dataToSend;
	TickType_t tOnKeys [4];

	while (1)
	{
		if( pdTRUE == xQueueReceive( queueKeyPad, &dataRecKey, portMAX_DELAY )){
			if( HIGH == dataRecKey.state ){
				tOnKeys[dataRecKey.key]= dataRecKey.ticktimes;
			}
			if( LOW == dataRecKey.state ){
				tEnd = dataRecKey.ticktimes;
				tStart= tOnKeys[dataRecKey.key];

				if( tEnd>tStart )
					dataToSend.deltaT= tEnd-tStart;
				else {
					tStart= (MAX_UINT32)-tStart;
					dataToSend.deltaT= tEnd+tStart;
				}
				switch (dataRecKey.key) {
				case TECL1:
					dataToSend.led= LED1;
					break;
				case TECL2:
					dataToSend.led= LED2;
					break;
				case TECL3:
					dataToSend.led= LED3;
					break;
				case TECL4:
					dataToSend.led= LED4;
					break;
				}
				xQueueSend(queueSigLed, &dataToSend, portMAX_DELAY);
			}
		}
	}
}

/*==================[irq handlers functions ]=========================*/

void GPIO0_IRQHandler(void)
{
	uint32_t countFall, countRise;
	queue_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	countFall= Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT);
	if( PININTCH0 == countFall ) {
		dataToSend.key= TECL1;
		dataToSend.state= HIGH;
		dataToSend.ticktimes= xTaskGetTickCountFromISR();
		xQueueSendFromISR(queueKeyPad, &dataToSend, &xSwitchRequired);

		Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT, countFall);
	    //Chip_PININT_EnableIntLow(LPPININTCH0C_GPIO_PIN_INT, PININTCH0);
	}

	countRise= Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT);
	if( PININTCH0 == countRise) {
		dataToSend.key= TECL1;
		dataToSend.state= LOW;
		dataToSend.ticktimes= xTaskGetTickCountFromISR();
		xQueueSendFromISR(queueKeyPad, &dataToSend, &xSwitchRequired);

		Chip_PININT_ClearRiseStates(LPC_GPIO_PIN_INT, countRise);
	}
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
	portEND_SWITCHING_ISR( xSwitchRequired );
}

void GPIO1_IRQHandler(void)
{
	uint32_t countFall, countRise;
	queue_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	countFall= Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT);
	if( PININTCH1 == countFall ) {
		dataToSend.key= TECL2;
		dataToSend.state= HIGH;
		dataToSend.ticktimes= xTaskGetTickCountFromISR();
		xQueueSendFromISR(queueKeyPad, &dataToSend, &xSwitchRequired);

		Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT, countFall);
	    //Chip_PININT_EnableIntLow(LPPININTCH0C_GPIO_PIN_INT, PININTCH0);
	}

	countRise= Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT);
	if( PININTCH1 == countRise) {
		dataToSend.key= TECL2;
		dataToSend.state= LOW;
		dataToSend.ticktimes= xTaskGetTickCountFromISR();
		xQueueSendFromISR(queueKeyPad, &dataToSend, &xSwitchRequired);

		Chip_PININT_ClearRiseStates(LPC_GPIO_PIN_INT, countRise);
	}
	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
	portEND_SWITCHING_ISR( xSwitchRequired );
}

void GPIO2_IRQHandler(void)
{
	uint32_t countFall, countRise;
	queue_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	countFall= Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT);
	if( PININTCH2 == countFall ) {
		dataToSend.key= TECL3;
		dataToSend.state= HIGH;
		dataToSend.ticktimes= xTaskGetTickCountFromISR();
		xQueueSendFromISR(queueKeyPad, &dataToSend, &xSwitchRequired);

		Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT, countFall);
	    //Chip_PININT_EnableIntLow(LPPININTCH0C_GPIO_PIN_INT, PININTCH0);
	}

	countRise= Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT);
	if( PININTCH2 == countRise) {
		dataToSend.key= TECL3;
		dataToSend.state= LOW;
		dataToSend.ticktimes= xTaskGetTickCountFromISR();
		xQueueSendFromISR(queueKeyPad, &dataToSend, &xSwitchRequired);

		Chip_PININT_ClearRiseStates(LPC_GPIO_PIN_INT, countRise);
	}
	NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
	portEND_SWITCHING_ISR( xSwitchRequired );
}

void GPIO3_IRQHandler(void)
{
	uint32_t countFall, countRise;
	queue_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	countFall= Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT);
	if( PININTCH3 == countFall ) {
		dataToSend.key= TECL4;
		dataToSend.state= HIGH;
		dataToSend.ticktimes= xTaskGetTickCountFromISR();
		xQueueSendFromISR(queueKeyPad, &dataToSend, &xSwitchRequired);

		Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT, countFall);
	    //Chip_PININT_EnableIntLow(LPPININTCH0C_GPIO_PIN_INT, PININTCH0);
	}

	countRise= Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT);
	if( PININTCH3 == countRise) {
		dataToSend.key= TECL4;
		dataToSend.state= LOW;
		dataToSend.ticktimes= xTaskGetTickCountFromISR();
		xQueueSendFromISR(queueKeyPad, &dataToSend, &xSwitchRequired);

		Chip_PININT_ClearRiseStates(LPC_GPIO_PIN_INT, countRise);
	}
	NVIC_ClearPendingIRQ(PIN_INT3_IRQn);
	portEND_SWITCHING_ISR( xSwitchRequired );
}
/*==================[external functions definition]==========================*/
static void initHardware(void)
{
    SystemCoreClockUpdate();

    Board_Init();
    //Board_Buttons_Init();
    ciaaUARTInit();
    ciaaIOInit();


    Chip_PININT_Init( LPC_GPIO_PIN_INT );
    Chip_SCU_GPIOIntPinSel( ID_IRQ_PIN_INT0, ID_PORT_TEC1, ID_PIN_TEC1 );
    //Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH0);
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_EnableIntLow  ( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_EnableIntHigh ( LPC_GPIO_PIN_INT, PININTCH0 );
	/* Set lowest priority for RIT */
	NVIC_SetPriority(PIN_INT0_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    Chip_SCU_GPIOIntPinSel( ID_IRQ_PIN_INT1, ID_PORT_TEC2, ID_PIN_TEC2 );
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH1 );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH1 );
    Chip_PININT_EnableIntLow  ( LPC_GPIO_PIN_INT, PININTCH1 );
    Chip_PININT_EnableIntHigh ( LPC_GPIO_PIN_INT, PININTCH1 );
	/* Set lowest priority for RIT */
	NVIC_SetPriority(PIN_INT1_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    Chip_SCU_GPIOIntPinSel( ID_IRQ_PIN_INT2, ID_PORT_TEC3, ID_PIN_TEC3 );
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH2 );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH2 );
    Chip_PININT_EnableIntLow  ( LPC_GPIO_PIN_INT, PININTCH2 );
    Chip_PININT_EnableIntHigh ( LPC_GPIO_PIN_INT, PININTCH2 );
	/* Set lowest priority for RIT */
	NVIC_SetPriority(PIN_INT2_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    Chip_SCU_GPIOIntPinSel( ID_IRQ_PIN_INT3, ID_PORT_TEC4, ID_PIN_TEC4 );
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH3 );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH3 );
    Chip_PININT_EnableIntLow  ( LPC_GPIO_PIN_INT, PININTCH3 );
    Chip_PININT_EnableIntHigh ( LPC_GPIO_PIN_INT, PININTCH3 );
	/* Set lowest priority for RIT */
	NVIC_SetPriority(PIN_INT3_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    PULSE_COUNT_TASK_Init();
    //Board_LED_Set(3, false);
    //Board_LED_Set(4, false);

	/* Enable IRQ for RIT */
	NVIC_EnableIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT3_IRQn);

}


int main(void)
{
	xTaskCreate(taskControlLed, (const char *)"taskControlLed", configMINIMAL_STACK_SIZE*2, 0, tskIDLE_PRIORITY+1, 0);
	xTaskCreate(taskDetectPulse, (const char *)"taskDetectPulse", configMINIMAL_STACK_SIZE*2, 0, tskIDLE_PRIORITY+1, 0);
	queueKeyPad= xQueueCreate(2, sizeof(queue_t));
	queueSigLed= xQueueCreate(2, sizeof(signal_t));

	initHardware();
	vTaskStartScheduler();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
