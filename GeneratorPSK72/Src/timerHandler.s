	 .data
aindex:				@	текущий шаг
	.word 0

	.text
	.globl TIM3_IRQHandler
	.p2align 2
	.type TIM3_IRQHandler,%function
	.equ PERIPH_BASE,           0x40000000UL
	.equ APB2PERIPH_BASE,       (PERIPH_BASE + 0x00010000UL)
	.equ GPIOA_BASE,            (APB2PERIPH_BASE + 0x00000800UL)
	.equ GPIOA_BSRR,            (GPIOA_BASE + 0x00000010UL)
	.equ GPIOA_BRR,				(GPIOA_BASE + 0x00000014UL)
	.equ GPIO_PIN_HI,			(0x0080)
	.equ GPIO_PIN_LOW,			(0x0100)
	.equ GPIO_PINS,				(0x0180)


TIM3_IRQHandler: 
	.fnstart
	ldr r0, A_BRR
	movw r1, GPIO_PINS
	str	r1, [r0]				@ GPIOA->BRR = (GPIO_PIN_LOW)|(GPIO_PIN_HI)
	ldr r0, =aindex
	bx	lr
	.fnend


	.p2align 4
A_BRR:
	.word GPIOA_BRR


