#include "communication_hc0x.h"
#include "communication_manager.h"


UART_HandleTypeDef* uart;
DMA_HandleTypeDef* dma_usart_tx;
volatile u8 rxByte;

extern void COMM_TxComplete(DMA_HandleTypeDef * hdma);

void COMM_Configure_Driver(UART_HandleTypeDef* uart_,
		DMA_HandleTypeDef* hdma_usart_) {
	uart = uart_;
	dma_usart_tx = hdma_usart_;
	dma_usart_tx->XferCpltCallback = COMM_TxComplete;
	HAL_UART_Receive_IT(uart, &rxByte, 1);
}

void COMM_SendData(u16 size) {
	HAL_UART_Transmit_DMA(uart, &commOutBuf, size);
	CommState.TxState = TxSending;
}

void COMM_TxComplete(DMA_HandleTypeDef * hdma) {
	CommState.TxState=TxIdle;
	if (commHandle != NULL) {
		xTaskResumeFromISR(commHandle);
	}
}

void COMM_RxCallback() {
	if (CommState.rxIndex < COMM_IN_BUF_SIZE - 1) {
		commInBuf[CommState.rxIndex] = rxByte;
		CommState.rxIndex++;
		HAL_UART_Receive_IT(uart, &rxByte, 1);
		if (commHandle != NULL) {
			xTaskResumeFromISR(commHandle);
		}
	}
}
