#include "communication_hm-10.h"
#include "communication_manager.h"
#include "soft_timer.h"

UART_HandleTypeDef* uart;
DMA_HandleTypeDef* dma_usart_tx;
TaskHandle_t commHandle;

//333ms Timer
#define AT_TIMEOUT 1
#define BT_DISABLE GPIO_PIN_4
#define BT_TX GPIO_PIN_2
#define BT_PORT GPIOA

volatile u8 rxByte;
volatile u32 lastAction;
volatile HCModule_t HCState;

const u8 textAT[] = { "AT" };
const u8 textATOK[] = { "OK" };
const u8 textATNameGET[] = { "AT+NAME?" };
const u8 textATNameGETOK[] = { "OK+NAME:MotoHelpeR" };
const u8 textATNameSET[] = { "AT+NAMEMotoHelpeR" };
const u8 textATNameSETOK[] = { "OK+Set:MotoHelpeR" };
const u8 textATPinGET[] = { "AT+PASS?" };
const u8 textATPinGETOK[] = { "OK+Get:202020" };
const u8 textATPinSET[] = { "AT+PASS202020" };
const u8 textATPinSETOK[] = { "OK+Set:202020" };
const u8 textATSpeed[] = { "AT+BAUD4" }; //115200
const u8 textATSpeedOK[] = { "OK+Set:4" };
const u8 textVersion[] = { "AT+VERS?" };

void COMM_Driver_Init(UART_HandleTypeDef* uart_, DMA_HandleTypeDef* hdma_usart_,
		TaskHandle_t taskHandle) {

	commHandle = taskHandle;
	uart = uart_;
	dma_usart_tx = hdma_usart_;
	CommState.HighSpeed = TRUE;
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

	HAL_StatusTypeDef result = HAL_UART_Receive_IT(uart, &rxByte, 1);
	if (result != HAL_OK)
	{
		CommState.RxResetRequired = TRUE;
	}

	COMM_ResumeTaskFromISR();
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	COMM_PeriodElapsedCallback();
}
void COMM_DRIVER_PeriodElapsedCallback() {
	HCState.ATTimeout = TRUE;
	HCState.softTimer = 0;
	COMM_ResumeTaskFromISR();
}

void HC_StartTimer() {
	lastAction = 3;
	HCState.softTimer = addTimer(2000, FALSE,
			&COMM_DRIVER_PeriodElapsedCallback);
}

void HC_StopTimer() {
	lastAction = 2;
	if (HCState.softTimer > 0) {
		lastAction = 7 | (HCState.softTimer<<8);
		removeTimer(HCState.softTimer);
		HCState.softTimer = 0;
	}
}

//delay between commands
void HC_Delay() {
	lastAction = 1;
	osDelay(300);
}

//as a rule not enough amount of bytes received
//await rest
void HC_Suspend() {
	lastAction = 4;
	const TickType_t xBlockTime = pdMS_TO_TICKS(500);
	ulTaskNotifyTake(pdFALSE, xBlockTime);
}


bool startRereiving(){
	if (HAL_UART_Receive_IT(uart, &rxByte, 1) == HAL_OK){
		return TRUE;
	}
	return FALSE;
}

void checkOverRun(){
	FlagStatus tmp1 = __HAL_UART_GET_FLAG(uart, UART_FLAG_ORE);
	FlagStatus tmp2 = __HAL_UART_GET_IT_SOURCE(uart, UART_IT_ERR);
	/* UART Over-Run interrupt occurred ----------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_OREFLAG(uart);
		uart->ErrorCode |= HAL_UART_ERROR_ORE;
	}
}

void COMM_Driver_HealthCheck(){
	if (CommState.RxResetRequired == TRUE){
		checkOverRun();
		__HAL_UART_FLUSH_DRREGISTER(uart);
		CommState.RxResetRequired = !startRereiving();
	}

}
void resetRxBuf(){
	lastAction = 5;
	int i=0;
	CommState.rxIndex=0;
	for(i=0;i<COMM_IN_BUF_SIZE;i++){
		commInBuf[i]=0;
	}
}

bool hasText(const u8* searchText, u8 length) {
	lastAction = 6;
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
		lastAction = 14;
		CommState.TxState = TxSending;
		CommState.rxIndex = 0;
		u8 i = 0;
		if (length > 0) {
			for (i = 0; i < length; i++) {
				commOutBuf[i] = *(text + i);
			}
			COMM_SendData(length);
		}
		lastAction = 15;
		HC_StartTimer();
		lastAction = 16;
		return TRUE;
	} else {
		lastAction = 17;
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
		lastAction = 10;
		if (configSendCommand(&textAT[0], 2) == TRUE) {
			lastAction = 12;
			resetRxBuf();
			HCState.ATState = ATAnswerWait;
		}else{
			lastAction = 11;
		}

	} else if (HCState.ATState == ATAnswerWait && CommState.TxState == TxIdle) { //Tx Idle means everything sent
		if (CommState.rxIndex >= 2) {
			HC_StopTimer();
			if (hasText(&textATOK[0], 2) == TRUE) {
				/*HCState.ATState = ATVersion;
				configSendCommand(&textVersion[0], sizeof(textVersion) - 1);
				HC_StopTimer();
				HC_Delay();
				 */
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
		if (configSendCommand(&textATNameGET[0], sizeof(textATNameGET) - 1)
				== TRUE) {
			resetRxBuf();
			HCState.ATState = ATNameGetAnswerWait;
		}
	} else if (HCState.ATState == ATNameGetAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex >= sizeof(textATNameGETOK) - 1) {
			HC_StopTimer();
			if (hasText(&textATNameGETOK[0], sizeof(textATNameGETOK) - 1)
					== TRUE) {
				//имя было установлено уже
				HCState.ATState = ATPinGet;
			} else {
				HCState.ATState = ATNameSet;
			}
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	//Name set
	if (HCState.ATState == ATNameSet) {
		if (configSendCommand(&textATNameSET[0], sizeof(textATNameSET) - 1)
				== TRUE) {
			resetRxBuf();
			HCState.ATState = ATNameSetAnswerWait;
		}
	} else if (HCState.ATState == ATNameSetAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex >= sizeof(textATNameSETOK) - 1) {
			HC_StopTimer();
			if (hasText(&textATNameSETOK[0], sizeof(textATNameSETOK) - 1)
					== TRUE) {
				HCState.ATState = ATPinGet;
			} else {
				//no text
				HCState.ATState = ATNameSet;
			}
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	if (HCState.ATState == ATPinGet) {
		if (configSendCommand(&textATPinGET[0], sizeof(textATPinGET) - 1)
				== TRUE) {
			resetRxBuf();
			HCState.ATState = ATPinGetAnswerWait;
		}
	} else if (HCState.ATState == ATPinGetAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex >= sizeof(textATPinGETOK) - 1) {
			HC_StopTimer();
			if (hasText(&textATPinGETOK[0], sizeof(textATPinGETOK) - 1)
					== TRUE) {
				HCState.ATState = ATSpeed;
			} else {
				HCState.ATState = ATPin;
			}
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	//Set pin
	if (HCState.ATState == ATPin) {
		if (configSendCommand(&textATPinSET[0], sizeof(textATPinSET) - 1)
				== TRUE) {
			resetRxBuf();
			HCState.ATState = ATPinAnswerWait;
		}
	} else if (HCState.ATState == ATPinAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex >= sizeof(textATPinSETOK) - 1) {
			HC_StopTimer();
			if (hasText(&textATPinSETOK[0], sizeof(textATPinSETOK) - 1)
					== TRUE) {
				HCState.ATState = ATWaitStream; //ATSpeed;
			} else {
				HCState.ATState = ATPin;
			}
			HC_Delay();
		} else {
			HC_Suspend();
		}
	}

	if (HCState.ATState == ATSpeed) {
		//if it is already high speed
		if (CommState.HighSpeed == TRUE) {
			HCState.ATState = ATWaitStream;
			return;
		}
		if (configSendCommand(&textATSpeed[0], sizeof(textATSpeed) - 1)
				== TRUE) {
			resetRxBuf();
			HCState.ATState = ATSpeedAnswerWait;
			HC_Delay();
		}
	} else if (HCState.ATState == ATSpeedAnswerWait
			&& CommState.TxState == TxIdle) {
		if (CommState.rxIndex >= sizeof(textATSpeedOK) - 1) {
			HC_StopTimer();
			if (hasText(&textATSpeedOK[0], sizeof(textATSpeedOK) - 1) == TRUE) {
				CommState.HighSpeed = TRUE;
				COMM_UartConfig(115200);
				HCState.ATState = ATWaitStream;
			} else {
				//no text
				HCState.ATState = AT;
				configSendCommand(&textVersion[0], sizeof(textVersion) - 1);
				HC_Delay();
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
		HAL_GPIO_WritePin(BT_PORT, BT_DISABLE, GPIO_PIN_RESET);
		HAL_UART_DeInit(uart);
		//pull down uart tx
		GPIO_InitTypeDef GPIO_InitStruct = { 0 };
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_GPIO_WritePin(BT_PORT, BT_TX, GPIO_PIN_RESET);
		//discharge capacitors
		osDelay(2000);

		//set speed
		if (CommState.HighSpeed == TRUE) {
			COMM_UartConfig(115200);
		} else {
			COMM_UartConfig(9600);
		}

		//enable bluetooth;
		HAL_GPIO_WritePin(BT_PORT, BT_DISABLE, GPIO_PIN_SET);
		osDelay(1000);

		HCState.ATState = AT;
		CommState.TxState = TxIdle;

		while (TRUE) {
			configModuleAT();
			osDelay(1);
			if (HCState.ATState == ATWaitStream) {
				CommState.CommDriverReady = TRUE;
				break;
			} else if (HCState.ATTimeout == TRUE) {
				CommState.CommDriverReady = FALSE;
				HCState.ATTimeout = FALSE;

				//no OK even for AT
				//switch speed
				if (HCState.ATState == ATAnswerWait) {
					if (CommState.HighSpeed == TRUE) {
						CommState.HighSpeed = FALSE;
					} else {
						CommState.HighSpeed = TRUE;
					}
				}
				break;
			}
		}
	}
}

