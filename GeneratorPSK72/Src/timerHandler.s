
	 .data
aindex:				@	текущий шаг
	.word 0
aconfig:
	.space 24
bconfig:
	.space 24

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

	.equ APB1PERIPH_BASE,		(PERIPH_BASE)
	.equ TIM3_BASE,				(APB1PERIPH_BASE + 0x00000400UL)
	.equ TIM3_SR,				(TIM3_BASE + 0x00000010UL)
	.equ TIMER_UPDATE_FLAG,		0x00000001
	.equ TIMER_CC1_FLAG,		0x00000002


TIM3_IRQHandler: 
	.fnstart
	ldr r0, A_BRR
	movw r1, GPIO_PINS
	str	r1, [r0]				@ GPIOA->BRR = (GPIO_PIN_LOW)|(GPIO_PIN_HI)

	@load SR of TIM3
	ldr r2, T3
	ldr r4, [r2]
	movw r5, TIMER_UPDATE_FLAG
	tst r4, r5			//r1 == 1
	bne  period
	b	half_period


	@ldr r0, =aindex

period:
	ldr r0, A_BSRR
	movw r1, GPIO_PIN_LOW
	str	r1, [r0]
	b exit

half_period:
	ldr r0, A_BSRR		@ GPIOA->BSRR = (GPIO_PIN_LOW)|(GPIO_PIN_HI)
	movw r1, GPIO_PIN_HI
	str	r1, [r0]


exit:
	movw r0, #0
	str r0, [r2]	// clear bit UIF (1)
	ldr r1, [r2]
	cmp r1, r0
	//bne exit
	bx	lr
	.fnend



	.p2align 4
A_BRR:
	.word GPIOA_BRR
A_BSRR:
	.word GPIOA_BSRR
T3:
	.word TIM3_SR

	.globl GET_CONFIGA
	.p2align 2
	.type GET_CONFIGA,%function
GET_CONFIGA:
	ldr r0, =aconfig
	bx lr


