/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "myprintf.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FILA4 (GPIOA->IDR & GPIO_IDR_IDR3)
#define FILA3 (GPIOA->IDR & GPIO_IDR_IDR2)
#define FILA1 (GPIOA->IDR & GPIO_IDR_IDR1)
#define FILA2 (GPIOA->IDR & GPIO_IDR_IDR0)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
 osThreadId dTask1;//TX Keypad
 osThreadId dTask2;//RX Keypad
 osThreadId dTask3;//RX LCD
 osThreadId dTask4;//TX LCD
 osMessageQId TaskQueue; // Queue definition.
 osMessageQId TaskQueueuwu; // Queue definition.
 char varTemp; //Global Assist Variable
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
 /*Clock configuration and GPIO initiators*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/*LCD Control*/
void Usart(void);
char USER_USART1_Receive(void);

/*Keypad Control*/
void USER_RCC_Init(void);
void USER_GPIO_Init(void);
void USER_USART1_Init(void);
void senderTask1(void const * argument);
void receiverTask2(void const * argument);
void senderTask3(void const * argument);
void receiverTask4(void const * argument);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  /* USER CODE BEGIN Init */
  HAL_Init();
  RCC -> APB2ENR |= RCC_APB2ENR_IOPBEN;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
   MX_GPIO_Init();

   Usart();
   LCD_Init();
   LCD_Cursor_ON();

   USER_RCC_Init( );
   USER_GPIO_Init( );
   USER_USART1_Init();

  /* USER CODE BEGIN 2 */
   RCC -> APB2ENR |= RCC_APB2ENR_IOPBEN;
   GPIOB->CRL= GPIOB->CRL &~ GPIO_CRL_CNF6 & ~GPIO_CRL_MODE6_1;
   GPIOB->CRL = GPIOB->CRL| GPIO_CRL_MODE6_0;

   GPIOB->CRL= GPIOB->CRL &~ GPIO_CRL_CNF5 & ~GPIO_CRL_MODE5_1;
   GPIOB->CRL = GPIOB->CRL | GPIO_CRL_MODE5_0;
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */

  osMessageQDef(TaskQueue, 1, char);
  TaskQueue = osMessageCreate(osMessageQ(TaskQueue), NULL);

  osMessageQDef(TaskQueueuwu, 1, char);
  TaskQueueuwu = osMessageCreate(osMessageQ(TaskQueueuwu), NULL);

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of tasks */

  osThreadDef(dTask1, senderTask1, osPriorityAboveNormal, 0, 128); //osThreadDef FOR Task1
  dTask1 = osThreadCreate(osThread(dTask1), NULL); //Thread creation

  osThreadDef(dTask2, receiverTask2, osPriorityNormal, 0, 128);//osThreadDef FOR Task2
  dTask2 = osThreadCreate(osThread(dTask2), NULL);//Thread creation

  osThreadDef(dTask3, senderTask3, osPriorityBelowNormal, 0 , 128);//osThreadDef FOR Task3
  dTask3 = osThreadCreate(osThread(dTask3), NULL);//Thread creation

  osThreadDef(dTask4, receiverTask4, osPriorityLow, 0, 128);//osThreadDef FOR Task4
  dTask4 = osThreadCreate(osThread(dTask4), NULL);//Thread creation

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */

  /* USER CODE BEGIN WHILE */
  while(1)
  {
	  senderTask1(dTask1);
	  receiverTask2(dTask2);
	  senderTask3(dTask3);
	  receiverTask4(dTask4);

  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
void USER_RCC_Init(void){
	RCC->APB2ENR	|=	 RCC_APB2ENR_USART1EN
			| 	 RCC_APB2ENR_IOPAEN;
}

void USER_GPIO_Init(void){
	GPIOA->BSRR = GPIO_BSRR_BS0;
	GPIOA->CRL &= ~GPIO_CRL_CNF0_0 & ~GPIO_CRL_MODE0;
	GPIOA->CRL |= GPIO_CRL_CNF0_1;

	GPIOA->BSRR = GPIO_BSRR_BS1;
	GPIOA->CRL &= ~GPIO_CRL_CNF1_0 & ~GPIO_CRL_MODE1;
	GPIOA->CRL |= GPIO_CRL_CNF1_1;

	GPIOA->BSRR = GPIO_BSRR_BS2;
	GPIOA->CRL &= ~GPIO_CRL_CNF2_0 & ~GPIO_CRL_MODE2;
	GPIOA->CRL |= GPIO_CRL_CNF2_1;

	GPIOA->BSRR = GPIO_BSRR_BS3;
	GPIOA->CRL &= ~GPIO_CRL_CNF3_0 & ~GPIO_CRL_MODE3;
	GPIOA->CRL |= GPIO_CRL_CNF3_1;

	GPIOA->BSRR = GPIO_BSRR_BS4;
	GPIOA->CRL &= ~GPIO_CRL_CNF4 & ~GPIO_CRL_MODE4_1;
	GPIOA->CRL |= GPIO_CRL_MODE4_0;

	GPIOA->BSRR = GPIO_BSRR_BS5;
	GPIOA->CRL &= ~GPIO_CRL_CNF5 & ~GPIO_CRL_MODE5_1;
	GPIOA->CRL |= GPIO_CRL_MODE5_0;

	GPIOA->BSRR = GPIO_BSRR_BS6;
	GPIOA->CRL &= ~GPIO_CRL_CNF6 & ~GPIO_CRL_MODE6_1;
	GPIOA->CRL |= GPIO_CRL_MODE6_0;

	GPIOA->BSRR = GPIO_BSRR_BS7;
	GPIOA->CRL &= ~GPIO_CRL_CNF7 & ~GPIO_CRL_MODE7_1;
	GPIOA->CRL |= GPIO_CRL_MODE7_0;

	GPIOA->CRH	&=	~GPIO_CRH_CNF9_0 & ~GPIO_CRH_MODE9_1;
	GPIOA->CRH	|=	 GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0;

}

char USER_USART1_Receive(void){
	while ((USART1->SR & USART_SR_RXNE)==0){
		return ',';
	}
	return USART1->DR;
}

void USER_USART1_Init(void){
  //complete the configurations for 9600 baud rate, 1 start_8-bit data_1 stop bits
	USART1->BRR	=	0X1D4C;//			9600 BPS -> 468.75, 0% ERROR
		USART1->CR1	&=	~USART_CR1_M//		1 start bit, 8 data bits
					&	~USART_CR1_WAKE//	idle line
					&	~USART_CR1_PCE//	parity control disabled
					&	~USART_CR1_TXEIE//	interrupt disabled (empty transmit reg)
					&	~USART_CR1_TCIE//	interrupt disabled (transmission complete)
					&	~USART_CR1_RXNEIE//	interrupt disabled (data received)
					&	~USART_CR1_IDLEIE//	interrupt disabled (idle line detected)
					&	~USART_CR1_RWU//	receiver active mode
					&	~USART_CR1_SBK;//	no break character transmitted
		USART1->CR1	|=	USART_CR1_UE//		USART enabled
					|	USART_CR1_TE//		transmitter enabled
					|	USART_CR1_RE;//		receiver enabled
		USART1->CR2	&=	~USART_CR2_STOP;//	1 stop bit
}

void senderTask1(void const * argument)
{
  osStatus dTask1;
  /* Infinite loop */
  int a;

  for(;;){

	//FILA 1
    a = 0;
    GPIOA->ODR &= ~GPIO_ODR_ODR4; //ESTO ES 1
    GPIOA->ODR |= GPIO_ODR_ODR5;
    GPIOA->ODR |= GPIO_ODR_ODR6;
    GPIOA->ODR |= GPIO_ODR_ODR7;
    if (FILA1 == 0){
        HAL_Delay(50);
        a = 1;
    }

    GPIOA->ODR |= GPIO_ODR_ODR4; //ESTO ES 2
    GPIOA->ODR &= ~GPIO_ODR_ODR5;
    GPIOA->ODR |= GPIO_ODR_ODR6;
    GPIOA->ODR |= GPIO_ODR_ODR7;
     if (FILA1 == 0){
        HAL_Delay(50);
        a = 2;
     }

     GPIOA->ODR |= GPIO_ODR_ODR4; //ESTO ES 3
     GPIOA->ODR |= GPIO_ODR_ODR5;
     GPIOA->ODR &= ~GPIO_ODR_ODR6;
     GPIOA->ODR |= GPIO_ODR_ODR7;
     if (FILA1 == 0){
         HAL_Delay(50);
         a = 3;
     }

     GPIOA->ODR |= GPIO_ODR_ODR4; //ESTO ES A
     GPIOA->ODR |= GPIO_ODR_ODR5;
     GPIOA->ODR |= GPIO_ODR_ODR6;
     GPIOA->ODR &= ~GPIO_ODR_ODR7;
      if (FILA1 == 0){
         HAL_Delay(50);
         a = 12;
      }
      //FINAL FILA 1
      //FILA 2
      GPIOA->ODR &= ~GPIO_ODR_ODR4; //ESTE ES 4
      GPIOA->ODR |= GPIO_ODR_ODR5;
      GPIOA->ODR |= GPIO_ODR_ODR6;
      GPIOA->ODR |= GPIO_ODR_ODR7;
      if (FILA2 == 0){
    	  HAL_Delay(50);
    	  a = 4;
      }

      GPIOA->ODR |= GPIO_ODR_ODR4; //ESTE ES 5
      GPIOA->ODR &= ~GPIO_ODR_ODR5;
      GPIOA->ODR |= GPIO_ODR_ODR6;
      GPIOA->ODR |= GPIO_ODR_ODR7;
      if (FILA2 == 0){
    	  HAL_Delay(50);
    	  a = 5;
      }

      GPIOA->ODR |= GPIO_ODR_ODR4; //ESTE ES 6
      GPIOA->ODR |= GPIO_ODR_ODR5;
      GPIOA->ODR &= ~GPIO_ODR_ODR6;
      GPIOA->ODR |= GPIO_ODR_ODR7;
      if (FILA2 == 0){
    	  HAL_Delay(50);
    	  a = 6;
      }

      GPIOA->ODR |= GPIO_ODR_ODR4; //ESTE ES B
      GPIOA->ODR |= GPIO_ODR_ODR5;
      GPIOA->ODR |= GPIO_ODR_ODR6;
      GPIOA->ODR &= ~GPIO_ODR_ODR7;
      if (FILA2 == 0){
    	  HAL_Delay(50);
    	  a = 13;
      }
      //FINAL FILA 2
      //FILA 3
      GPIOA->ODR &= ~GPIO_ODR_ODR4; //EST0 ES 7
      GPIOA->ODR |= GPIO_ODR_ODR5;
      GPIOA->ODR |= GPIO_ODR_ODR6;
      GPIOA->ODR |= GPIO_ODR_ODR7;

      if (FILA3 == 0){
        HAL_Delay(50);
        a = 7;
      }

      GPIOA->ODR |= GPIO_ODR_ODR4; //EST0 ES 8
      GPIOA->ODR  &= ~GPIO_ODR_ODR5;
      GPIOA->ODR |= GPIO_ODR_ODR6;
      GPIOA->ODR |= GPIO_ODR_ODR7;

      if (FILA3 == 0){
		HAL_Delay(50);
		a = 8;
      }

      GPIOA->ODR |= GPIO_ODR_ODR4; //EST0 ES 9
      GPIOA->ODR |= GPIO_ODR_ODR5;
      GPIOA->ODR &= ~GPIO_ODR_ODR6;
      GPIOA->ODR |= GPIO_ODR_ODR7;

      if (FILA3 == 0){
		HAL_Delay(50);
		a = 9;
      }

      GPIOA->ODR |= GPIO_ODR_ODR4; //EST0 ES C
      GPIOA->ODR |= GPIO_ODR_ODR5;
      GPIOA->ODR |= GPIO_ODR_ODR6;
      GPIOA->ODR &= ~GPIO_ODR_ODR7;
      if (FILA3 == 0){
		HAL_Delay(50);
		a = 14;
      }
      //FINAL FILA 3
      //FILA 4
      GPIOA->ODR &= ~GPIO_ODR_ODR4; //ESTO ES *
      GPIOA->ODR |= GPIO_ODR_ODR5;
      GPIOA->ODR |= GPIO_ODR_ODR6;
      GPIOA->ODR |= GPIO_ODR_ODR7;
      if (FILA4 == 0){
        HAL_Delay(50);
        a = 11;
      }

      GPIOA->ODR |= GPIO_ODR_ODR4; //ESTO ES 0
      GPIOA->ODR &= ~GPIO_ODR_ODR5;
      GPIOA->ODR |= GPIO_ODR_ODR6;
      GPIOA->ODR |= GPIO_ODR_ODR7;
      if (FILA4 == 0){
    	HAL_Delay(50);
    	a = 16;
      }
      GPIOA->ODR |= GPIO_ODR_ODR4; //ESTO ES #
      GPIOA->ODR |= GPIO_ODR_ODR5;
      GPIOA->ODR &= ~GPIO_ODR_ODR6;
      GPIOA->ODR |= GPIO_ODR_ODR7;
      if (FILA4 == 0){
    	HAL_Delay(50);
    	a = 10;
      }

      GPIOA->ODR |= GPIO_ODR_ODR4; //ESTO ES D
      GPIOA->ODR |= GPIO_ODR_ODR5;
      GPIOA->ODR |= GPIO_ODR_ODR6;
      GPIOA->ODR &= ~GPIO_ODR_ODR7;
      if (FILA4 == 0){
    	HAL_Delay(50);
    	a = 15;
      }
      //FINAL FILA 3

      if(a!=0){
    	 dTask1 = osMessagePut(TaskQueue, a, 1000);
    	 osDelay(500);
        }
  }
  osDelay(5);
  }

void receiverTask2(void const * argument)
{
osEvent dTask2;
  /* Infinite loop */
for(;;){
	  dTask2 = osMessageGet(TaskQueue, 1000);
	  if( dTask2.status == osEventMessage ){
		  if(dTask2.value.v == 16)
			  myprintf("0");
		  else if(dTask2.value.v == 1)
			  myprintf("1");
		  else if(dTask2.value.v == 2)
			  myprintf("2");
		  else if(dTask2.value.v == 3)
			  myprintf("3");
		  else if(dTask2.value.v == 4)
			  myprintf("4");
		  else if(dTask2.value.v == 5)
			  myprintf("5");
		  else if(dTask2.value.v == 6)
			  myprintf("6");
		  else if(dTask2.value.v == 7)
			  myprintf("7");
		  else if(dTask2.value.v == 8)
			  myprintf("8");
		  else if(dTask2.value.v == 9)
			  myprintf("9");
		  else if(dTask2.value.v == 10)
			  myprintf("#");
		  else if(dTask2.value.v == 11)
			  myprintf("*");
		  else if(dTask2.value.v == 12)
			  myprintf("A");
		  else if(dTask2.value.v == 13)
			  myprintf("B");
		  else if(dTask2.value.v == 14)
			  myprintf("C");
		  else if(dTask2.value.v == 15)
			  myprintf("D");
	  //dTask2 = osMessagePut(TaskQueue, "1", 1000);
	  }

	}
}

/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void senderTask3(void const * argument){
	osStatus dTask3;
	for(;;){
	varTemp = USER_USART1_Receive();
	if(varTemp != '0'){
	    dTask3 = osMessagePut(TaskQueueuwu, varTemp, 500);
	    //osDelay(500);
	    }
	}
}
/* USER CODE END 5 */

/* USER CODE BEGIN 6 */
void Usart(void){
	//USART1 clock port enable
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN
				  | RCC_APB2ENR_IOPAEN;
	// Configure TX/RX pins
	// PA9 (TX) alternative function output push-pull, 10 MHz
	// PA10 (RX) input floating
	GPIOA->CRH &= ~GPIO_CRH_CNF9_0 & ~GPIO_CRH_MODE9_1
			   & ~GPIO_CRH_CNF10_1 & ~GPIO_CRH_MODE10;
	GPIOA->CRH |= GPIO_CRH_CNF9_1  | GPIO_CRH_MODE9_0
				|  GPIO_CRH_CNF10_0;

	// Configure USART protocol
	USART1->BRR = 0x1D4C; // 9600 bps

	USART1->CR1 &= ~USART_CR1_M // word length: 1 Start bit, 8 Data bits
				&  ~USART_CR1_WAKE // Wake-up method: Idle Line
				&  ~USART_CR1_PCE // Parity control Disable
				&  ~USART_CR1_TXEIE // TXE interrupt inhibited
				&  ~USART_CR1_TCIE // Transmission complete interrupt inhibited
				&  ~USART_CR1_RXNEIE // RXNE interrupt inhibited
				&  ~USART_CR1_IDLEIE // IDLE interrupt inhibited
				&  ~USART_CR1_RWU // Receiver wake-up in active mode
				&  ~USART_CR1_SBK; //No break character is transmitted

	USART1->CR1 |= 	USART_CR1_UE //USART enable
				| 	USART_CR1_TE //Transmitter enable
				| 	USART_CR1_RE; //Receiver enable

	USART1->CR2 &= 	USART_CR2_STOP; // 00: 1 stop bit

}
/* USER CODE END 6 */

/*USER CODE START 7*/
void receiverTask4(void const * argument){
	osEvent dTask4;
	for(;;){
	dTask4 = osMessageGet(TaskQueueuwu, 500);
		  if(dTask4.value.v=='0'){
		  			LCD_Clear( );//Borra la pantalla
		  			LCD_Set_Cursor( 1, 0 );//Posición cursor [ 1 , 0 ].
		  			LCD_Put_Str("Just like Heaven -The Cure");
		  			/*LCD_Set_Cursor( 2, 0 );//Posición cursor [ 2 , 0 ].
		  			LCD_Put_Str("The Cure");*/
		  		} else if(dTask4.value.v=='1'){
		  			LCD_Clear( );//Borra la pantalla
		  	    	LCD_Set_Cursor( 1, 0 );//Posición cursor [ 1 , 0 ].
		  			LCD_Put_Str("La Dama y el");
		  			LCD_Set_Cursor( 2, 0 );//Posición cursor [ 2 , 0 ].
		  			LCD_Put_Str("Moribundo - Jose Madero");
		  		} else if(dTask4.value.v=='2'){
		  			LCD_Clear( );//Borra la pantalla
		  	    	LCD_Set_Cursor( 1, 0 );//Posición cursor [ 1 , 0 ].
		  			LCD_Put_Str("This Charming Man");
		  			LCD_Set_Cursor( 2, 0 );//Posición cursor [ 2 , 0 ].
		  			LCD_Put_Str("The Smiths");
		  		} else if(dTask4.value.v=='3'){
		  			LCD_Clear( );//Borra la pantalla
		  			LCD_Set_Cursor( 1, 0 ); //Posición cursor [ 1 , 0 ].
		  			LCD_Put_Str("All Those Friendly People");
		  			LCD_Set_Cursor( 2, 0 ); //Posición cursor [ 2 , 0 ].
		  			LCD_Put_Str("Funeral Suits");
		  		} else if(dTask4.value.v=='4'){
		  			LCD_Clear( );//Borra la pantalla
		  			LCD_Set_Cursor( 1, 0 );//Posición cursor [ 1 , 0 ].
		  			LCD_Put_Str("Time to Pretend");
		  			LCD_Set_Cursor( 2, 0 );//Posición cursor [ 2 , 0 ].
		  			LCD_Put_Str("MGMT");
		  		} else if(dTask4.value.v=='5'){
		  			LCD_Clear( );//Borra la pantalla
		  			LCD_Set_Cursor( 1, 0 );//Posición cursor [ 1 , 0 ].
		  			LCD_Put_Str("Everlong");
		  			LCD_Set_Cursor( 2, 0 );//Posición cursor [ 2 , 0 ].
		  			LCD_Put_Str("FooFighters");
		  		} else if(dTask4.value.v=='6'){
		  			LCD_Clear( );//Borra la pantalla
		  			LCD_Set_Cursor( 1, 0 );//Posición cursor [ 1 , 0 ].
		  			LCD_Put_Str("Eye of The Tiger");
		  			LCD_Set_Cursor( 2, 0 );//Posición cursor [ 2 , 0 ].
		  			LCD_Put_Str("Survivor");
		  		} else if(dTask4.value.v=='7'){
		  			LCD_Clear( );//Borra la pantalla
		  			LCD_Set_Cursor( 1, 0 );//Posición cursor [ 1 , 0 ].
		  			LCD_Put_Str("Another One Bites");
		  			LCD_Set_Cursor( 2, 0 );//Posición cursor [ 2 , 0 ].
		  			LCD_Put_Str("the Dust -Queen");
		  		} else if(dTask4.value.v=='8'){
		  			LCD_Clear( );//Borra la pantalla
		  			LCD_Set_Cursor( 1, 0 );//Posición cursor [ 1 , 0 ].
		  			LCD_Put_Str("American Woman");
		  			LCD_Set_Cursor( 2, 0 );//Posición cursor [ 2 , 0 ].
		  			LCD_Put_Str("Guess Who");
		  		} else if(dTask4.value.v=='9'){
		  			LCD_Clear( );//Borra la pantalla
		  			LCD_Set_Cursor( 1, 0 );//Posición cursor [ 1 , 0 ].
		  			LCD_Put_Str("Rocketman");
		  			LCD_Set_Cursor( 2, 0 );//Posición cursor [ 2 , 0 ].
		  			LCD_Put_Str("Elton John");
		  		} //else{
		  			//LCD_Clear( );//borra la pantalla
		  			//LCD_Set_Cursor( 1, 0 );//Posición cursor [ 1 , 0 ].
		  			//LCD_Put_Str("Waiting for Song");
		  		//}
	  	  }

}
/*USER CODE END 7*/

/*USER CODE START 8*/
/*USER CODE END 8*/

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
