#include "communication_hc0x.h"
#include "communication_manager.h"

UART_HandleTypeDef* uart;
DMA_HandleTypeDef* dma_usart_tx;
TaskHandle_t commHandle;

//333ms Timer
#define AT_TIMEOUT 1
#define BT_DISABLE GPIO_PIN_4
#define BT_PORT GPIOA

volatile u8 rxByte;
volatile HCModule_t HCState;

const u8 textAT[] = { "AT" };
const u8 textATOK[] = { "OK" };
const u8 textATName[] = { "AT+NAMEMotoHelpeR" }; //
const u8 textATNameOK[] = { "+NAME=MotoHelpeR" };
const u8 textATPin[] = { "AT+PIN2020" };
const u8 textATPinOK[] = { "+PIN=2020" };
const u8 textATSpeed[] = { "AT+BAUD6" };//115200
const u8 textATSpeedOK[] = { "+BAUD=6" };
const u8 textVersion[] = { "AT+VERSION" };



void COMM_Driver_Init(UART_HandleTypeDef* uart_,
		DMA_HandleTypeDef* hdma_usart_, TaskHandle_t taskHandle) {

	commHandle = taskHandle;
	uart = uart_;
	dma_usart_tx = hdma_usart_;

	COMM_Driver_Configure();

}

void COMM_UartConfig(u32 speed) {
	HAL_UART_DeInit(uart);
	uart->Init.BaudRate = speed;
	HAL_UART_Init(uart);
	HAL_UART_Receive_IT(uart, &rxByte, 1);
}

void COMM_SendData(u16 size) {
	if (HAL_UART_Transmit_DMA(uart, (uint8_t *) &commOutBuf[0], size)
			== HAL_OK) {
		CommState.TxState = TxSending;
	} else {

	}

}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	CommState.TxState = TxIdle;
	COMM_ResumeTaskFromISR();
}

void COMM_RxCallback() {
	if (CommState.rxIndex < COMM_IN_BUF_SIZE - 1) {
		commInBuf[CommState.rxIndex] = rxByte;
		CommState.rxIndex++;
	}
	HAL_UART_Receive_IT(uart, &rxByte, 1);
	COMM_ResumeTaskFromISR();
}

void COMM_DRIVER_PeriodElapsedCallback() {
	HCState.ATTimeout = TRUE;
	HCState.softTimer = 0;
	COMM_ResumeTaskFromISR();
}

void HC_StartTimer() {
	HCState.softTimer = addTimer(1000, FALSE,
			&COMM_DRIVER_PeriodElapsedCallback);
}

void HC_StopTimer() {
	if (HCState.softTimer > 0) {
		removeTimer(HCState.softTimer);
		HCState.softTimer = 0;
	}
}

//delay between commands
void HC_Delay() {
	osDelay(300);
}

//as a rule not enough amount of bytes received
//await rest
void HC_Suspend() {
	const TickType_t xBlockTime = pdMS_TO_TICKS(5000);
	ulTaskNotifyTake(pdFALSE, xBlockTime);
}

bool hasText(const u8* searchText, u8 length) {
	int iterator = 0;
	volatile u8* bigBuf = &commInBuf[0];
	for (iterator = 0; iterator < length; iterator++) {
		if (*bigBuf++ != *searchText++)
			return FALSE;
	}
	return TRUE;
}

bool configSendCommand(const u8* text, u8 length) {
	if (CommState.TxState == TxIdle) {

		CommState.TxState = TxSending;

		u8 i = 0;
		if (length > 0) {
			for (i = 0; i < length; i++) {
				commOutBuf[i] = *(text + i);
			}
			commOutBuf[i] = 0x0D;
			commOutBuf[i + 1] = 0x0A;
			COMM_SendData(length+2);
		}

		HC_StartTimer();
		return TRUE;
	} else {
		osDelay(1);
		return FALSE;
	}
}

void configModuleAT() {
	//Test connection Send AT - OK
	//Cheching name
	//Setting up Name AT+NAME=MotoHelpeR\r\n
	//Check pin and set it if nessesary
	//Set up the PIN AT+PIN2020

	//AT-OK
	if (HCState.ATState == AT) {
		if (configSendCommand(&textAT[0], 2) == TRUE) {
			resetRxBuf();
			HC_Delay();
			HCState.ATState = ATAnswerWait;
		}
	} else if (HCState.ATState == ATAnswerWait && CommState.TxState == TxIdle) { //Tx Idle means everything sent
		if (CommState.rxIndex > 2) {
			if (hasText(&textATOK[0], 2) == TRUE) {
				HC_StopTimer();
				HCState.ATState = ATNameGet;
				//между командами паузу делать надо
				HC_Delay();
			} else {
				//no text
				HCState.ATTimeout = TRUE;
				HC_StopTimer();
			}
		} else {
			HC_Suspend();
		}
	}

	//Name check
	if (HCState.ATState == ATNameGet) {
		if (configSendCommand(&textATName[0], 7) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATNameGetAnswerWait;
		}
	} else if (HCState.ATState == ATNameGetAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex > 10) {
			if (hasText(&textATNameOK[0], 10) == TRUE) {
				//имя было установлено уже
				HCState.ATState = ATPinGet;
			} else {
				HCState.ATState = ATNameSet;
			}
			HC_StopTimer();
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	//Name set
	if (HCState.ATState == ATNameSet) {
		if (configSendCommand(&textATName[0], sizeof(textATName)) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATNameSetAnswerWait;
		}
	} else if (HCState.ATState == ATNameSetAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex > 5) {
			if (hasText(&textATNameOK[0], 5) == TRUE) {
				HCState.ATState = ATPinGet;
			} else {
				//no text
				HCState.ATState = ATNameSet;
			}
			HC_StopTimer();
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	if (HCState.ATState == ATPinGet) {
		if (configSendCommand(&textATPin[0], 6) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATPinGetAnswerWait;
		}
	} else if (HCState.ATState == ATPinGetAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex >= 9) {
			if (hasText(&textATPinOK[0], sizeof(textATPinOK)) == TRUE) {
				HCState.ATState = ATSpeed;
			} else {
				HCState.ATState = ATPin;
			}
			HC_StopTimer();
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	//Set pin
	if (HCState.ATState == ATPin) {
		if (configSendCommand(&textATPin[0], sizeof(textATPin)) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATPinAnswerWait;
		}
	} else if (HCState.ATState == ATPinAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex > 4) {
			if (hasText(&textATPinOK[0], 4) == TRUE) {
				HCState.ATState = ATSpeed;
			} else {
				HCState.ATState = ATPin;
			}
			HC_StopTimer();
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	if (HCState.ATState == ATSpeed) {
		if (configSendCommand(&textATSpeed[0], sizeof(textATSpeed)) == TRUE) {
			resetRxBuf();
			HCState.ATState = ATSpeedAnswerWait;
			HC_Delay();
		}
	} else if (HCState.ATState == ATSpeedAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex > 2) {
			if (hasText(&textATSpeedOK[0], 7) == TRUE) {
				HC_StopTimer();
				HCState.ATState = ATWaitStream;
				COMM_UartConfig(115200);
			} else {
				//no text
				HCState.ATState = AT;
				configSendCommand(&textVersion[0], sizeof(textVersion));
				HC_Delay();
				HCState.ATState = ATWaitStream;
			}
		} else {
			HC_Suspend();
		}
	}

}

//setup pin, speed, etc
void COMM_Driver_Configure() {

	while (CommState.CommDriverReady == FALSE) {

		HC_StopTimer();

		//reset bluetooth
		HAL_GPIO_WritePin(BT_PORT, BT_DISABLE, GPIO_PIN_SET);
		osDelay(700);

		//reset speed
		COMM_UartConfig(9600);

		//enable bluetooth;
		HAL_GPIO_WritePin(BT_PORT, BT_DISABLE, GPIO_PIN_RESET);
		osDelay(300);
		HCState.ATState = AT;

		while (TRUE) {
			configModuleAT();
			if (HCState.ATState == ATWaitStream) {
				CommState.CommDriverReady = TRUE;
				break;
			} else if (HCState.ATTimeout == TRUE) {
				CommState.CommDriverReady = FALSE;
				HCState.ATTimeout = FALSE;
				break;
			}
		}
	}
}

