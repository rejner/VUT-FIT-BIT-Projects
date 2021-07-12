/* ###################################################################
**     Filename    : main.c
**     Project     : IMP_FreeRTOS_Project
**     Processor   : MK60DN512VLQ10
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2020-12-17, 11:30, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @author Michal Rein (xreinm00)
** @version 01.01
** @brief
**         Modul Main.
**         Obsahuje implementaci všech potøebných metod a vstupní bod programu.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "FRTOS1.h"
#include "UTIL1.h"
#include "MCUC1.h"
#include "GPIO1.h"
#include "GPIO2.h"
#include "GPIO3.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include <stdio.h>


/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))

// Enum módù pro konfiguraci blikání LED diod
enum {
	MODE0,
	MODE1,
	MODE2,
	MODE3,
	MODE4
};

#define MAX_MODE 3

int mode = 0; // urèuje aktuálnì aktivní mód urèující konfiguraci blikání LED diod

// Zapnout/vypnout ledku s daným Field name
static void switchLED(LDD_GPIO_TBitField Field){
	GPIO1_ToggleFieldBits(GPIO1_DeviceData, Field, 0xff);
}

// Zmìna módu blikání LED diod
// Pøi znovustisknutí pøedchozího tlaèítka dojde k pøechodu do módu 0 (nebliká)
static void switchLEDMode(LDD_GPIO_TBitField Field){
	switch(Field){
	case SW2:
		if (mode == MODE1){
			mode = MODE0;
		} else {
			mode = MODE1;
		}
		break;

	case SW3:
		if (mode == MODE2){
			mode = MODE0;
		} else {
			mode = MODE2;
		}
		break;

	case SW4:
		if (mode == MODE3){
			mode = MODE0;
		} else {
			mode = MODE3;
		}
		break;

	case SW5:
		if (mode == MODE4){
			mode = MODE0;
		} else {
			mode = MODE4;
		}
		break;

	}


}

// Task pro bzuèení piezza
// Jestliže je stisknuto tlaèítko SW6, dojde k rozkmitání piezza a dojde ke zvuku pípnutí
static void buzzerHandlerTask(){
	for(;;){
		if (GPIO2_GetFieldValue(GPIO2_DeviceData, SW6) == 0){
			for (int i = 0; i <= 200000; i++){
				GPIO3_ToggleFieldBits(GPIO3_DeviceData, SPEAKER, 0xff);
			}
		}
	}
}

// Task pro blikání diod podle zvoleného módu
// Zvolený mód odpovídá hodnotì v globální promìnné 'int mode'
static void blinkingLEDTask(void *param)
{
	int i = 0;
	for(;;){
		switch(mode){

		case MODE0:
			break;

		case MODE1:
			for (i = 3; i >= 0; i--){
				FRTOS1_vTaskDelay(500);
				GPIO1_ToggleFieldBits(GPIO1_DeviceData, i, 0xff);
			}
			break;

		case MODE2:
			for (i = 0; i <= 3; i++){
				FRTOS1_vTaskDelay(500);
				GPIO1_ToggleFieldBits(GPIO1_DeviceData, i, 0xff);
			}
			break;

		case MODE3:
			for (i = 0; i < 2; i++){
				FRTOS1_vTaskDelay(500);
				GPIO1_ToggleFieldBits(GPIO1_DeviceData, i, 0xff);
				GPIO1_ToggleFieldBits(GPIO1_DeviceData, MAX_MODE-i, 0xff);
			}
			break;

		case MODE4:
			FRTOS1_vTaskDelay(500);
			GPIO1_ToggleFieldBits(GPIO1_DeviceData, LED_D9, 0xff);
			GPIO1_ToggleFieldBits(GPIO1_DeviceData, LED_D10, 0xff);
			GPIO1_ToggleFieldBits(GPIO1_DeviceData, LED_D11, 0xff);
			GPIO1_ToggleFieldBits(GPIO1_DeviceData, LED_D12, 0xff);
			break;
		}

	}
}

// Task pro kontrolu stisku tlaèítka a spuštìní adekvátních metod
// Jestliže je nìkteré z tlaèítek stisknuto, dojde k volání pøepnutí módu blikání,
// což je vynuceno metodou switchLEDMode(ButtonFieldName)
static void btnPressCheckTask(void *param)
{
	for(;;){

		if (GPIO2_GetFieldValue(GPIO2_DeviceData, SW2) == 0){
			switchLEDMode(SW2);

		} else if (GPIO2_GetFieldValue(GPIO2_DeviceData, SW3) == 0){
			switchLEDMode(SW3);

		} else if (GPIO2_GetFieldValue(GPIO2_DeviceData, SW4) == 0){
			switchLEDMode(SW4);

		} else if (GPIO2_GetFieldValue(GPIO2_DeviceData, SW5) == 0){
			switchLEDMode(SW5);
		}

		vTaskDelay(1000);
	}
}


/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /*--- install FreeRTOS tasks ---*/
  xTaskCreate(blinkingLEDTask, "BLT", 50, NULL, 1, NULL);
  xTaskCreate(btnPressCheckTask, "BCT", 50, NULL, 1, NULL);
  xTaskCreate(buzzerHandlerTask, "BHT", 50, NULL, 1, NULL);


  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
