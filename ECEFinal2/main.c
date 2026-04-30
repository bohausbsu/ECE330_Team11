/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c  LAB6
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
#include "usb_host.h"
#include "seg7.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s3;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim7;

/* USER CODE BEGIN PV */
int DelayValue = 50;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM7_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
//void Play_Note(int note,int size,int tempo,int space);
//extern void Seven_Segment_Digit (unsigned char digit, unsigned char hex_char, unsigned char dot);
//extern void Seven_Segment(unsigned int HexValue);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*---------------------------------------------------------------------------
 * Lab 6 Base Variables
 *---------------------------------------------------------------------------*/
char ramp = 0;
char RED_BRT = 0;
char GREEN_BRT = 0;
char BLUE_BRT = 0;
char RED_STEP = 1;
char GREEN_STEP = 2;
char BLUE_STEP = 3;
char DIM_Enable = 0;
char Music_ON = 0;
int  music_timer = 0;        /* ms countdown – auto-stops music at 0 */
int TONE = 0;
int COUNT = 0;
int INDEX = 0;
int Note = 0;
int Save_Note = 0;
int Vibrato_Depth = 1;
int Vibrato_Rate = 40;
int Vibrato_Count = 0;
char Animate_On = 0;
char Message_Length = 0;
char *Message_Pointer;
char *Save_Pointer;
int Delay_msec = 250;
int Delay_counter = 0;

/*---------------------------------------------------------------------------
 * Battleship Game Variables
 *---------------------------------------------------------------------------*/
GameState current_state = STATE_START;  /* FSM starts at the title screen        */
PlayerBoard p1 = {0, 0, 0, 0};         /* Player 1's board: all maps empty      */
PlayerBoard p2 = {0, 0, 0, 0};         /* Player 2's board: all maps empty      */
uint8_t cursor_x = 0;          /* Current cursor column position                */
uint8_t cursor_y = 0;          /* Current cursor row position                   */
uint8_t cursor_v_h = 0;        /* Orientation: 0=vertical grid, 1=horizontal    */
uint8_t segments_placed = 0;   /* How many segments current player has placed    */
uint8_t p1_hits = 0;           /* Total segments P1 has hit on P2's board        */
uint8_t p2_hits = 0;           /* Total segments P2 has hit on P1's board        */
uint8_t display_buffer[8]    = {0};  /* Segments to show at full brightness      */
uint8_t display_buffer_50[8] = {0};  /* Segments to show at 50% PWM brightness   */
uint8_t cursor_digit = 0;      /* Which physical digit (0-7) has the cursor     */
uint8_t cursor_seg  = 0;       /* Bitmask of cursor segment (e.g. 0x01 = seg a) */

//Marquee message strings.
char Msg_Start[] = {SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
    CHAR_B,CHAR_A,CHAR_T,CHAR_T,CHAR_L,CHAR_E,CHAR_S,CHAR_H,CHAR_I,CHAR_P,
    SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char Msg_P1Place[] = {SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
    CHAR_P,CHAR_1,SPACE,CHAR_P,CHAR_L,CHAR_A,CHAR_C,CHAR_E,
    SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char Msg_P2Place[] = {SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
    CHAR_P,CHAR_2,SPACE,CHAR_P,CHAR_L,CHAR_A,CHAR_C,CHAR_E,
    SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char Msg_P1Turn[] = {SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
    CHAR_P,CHAR_1,SPACE,CHAR_T,CHAR_U,CHAR_R,CHAR_N,
    SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char Msg_P2Turn[] = {SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
    CHAR_P,CHAR_2,SPACE,CHAR_T,CHAR_U,CHAR_R,CHAR_N,
    SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char Msg_P1Wins[] = {SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
    CHAR_P,CHAR_1,SPACE,CHAR_W,CHAR_I,CHAR_N,CHAR_S,
    SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

char Msg_P2Wins[] = {SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
    CHAR_P,CHAR_2,SPACE,CHAR_W,CHAR_I,CHAR_N,CHAR_S,
    SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE};

/* Song array for music playback */
Music Song[100];

/*---------------------------------------------------------------------------
 * Helper Functions
 *---------------------------------------------------------------------------*/

/** Set the marquee to scroll a given message array */
void Set_Marquee(char *msg, int len) {
    Message_Pointer = msg;
    Save_Pointer    = msg;
    Message_Length   = len;
    Delay_msec      = 250;
    Animate_On      = 1;
}

/** Compute the correct vert_boats/vert_shots bit position for a cursor position.
 *  Must match the display engine's mapping in seg7.c:
 *    odd  cx -> left segment  -> bit (2*digit)      for bot, (16 + 2*digit)     for top
 *    even cx -> right segment -> bit (2*digit + 1)  for bot, (16 + 2*digit + 1) for top */
uint32_t vert_bit(uint8_t cx, uint8_t cy) {
    uint8_t digit = cx / 2;
    uint8_t is_left = (cx % 2 == 1);  /* odd = left (matches cursor fix) */
    uint8_t offset = 2 * digit + (is_left ? 0 : 1);
    if (cy == 0) offset += 16;  /* top row is in upper 16 bits */
    return 1u << offset;
}

/** Debounced button press detection.
 *  Checks BOTH the onboard blue button (PA0, active HIGH on DISC1)
 *  AND the external switch on PC11 (polarity-agnostic).
 *  Returns 1 on a press event from either button. */
uint8_t Button_Pressed(void) {
    static uint8_t last_pa0  = 0;
    static uint8_t last_pc11 = 2;  /* 2 = uninitialized */
    static uint8_t rest_pc11 = 2;
    static uint8_t startup   = 0;  /* ignore first few calls */

    /* Discard the first 10 calls (~500ms) to avoid startup false triggers */
    if (startup < 10) {
        last_pa0 = (GPIOA->IDR & 1);
        startup++;
        return 0;
    }

    /* Check onboard blue button on PA0 (active HIGH: reads 1 when pressed) */
    uint8_t pa0_now = (GPIOA->IDR & 1);  /* Read PA0 directly via register */
    uint8_t pa0_pressed = (pa0_now == 1 && last_pa0 == 0);
    last_pa0 = pa0_now;

    /* Check external switch on PC11 (polarity-agnostic) */
    uint8_t pc11_now = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11);
    uint8_t pc11_pressed = 0;
    if (rest_pc11 == 2) {
        rest_pc11 = pc11_now;
        last_pc11 = pc11_now;
    } else {
        pc11_pressed = (pc11_now != rest_pc11 && last_pc11 == rest_pc11);
        last_pc11 = pc11_now;
    }

    return (pa0_pressed || pc11_pressed);
}

/** Play a short descending "explosion" sound effect for a hit */
void Play_Hit_Sound(void) {
    Song[0] = (Music){C5, _16th, 60, 5, 0};
    Song[1] = (Music){G4, _16th, 60, 5, 0};
    Song[2] = (Music){C4, _8th,  60, 5, 1};  /* end marker */
    INDEX = 0; COUNT = 0; Save_Note = Song[0].note; Music_ON = 1;
    music_timer = 2000;  /* Auto-stop after 2 seconds */
}

/** Play a low "splash" sound effect for a miss */
void Play_Miss_Sound(void) {
    Song[0] = (Music){C2, _8th, 60, 5, 0};
    Song[1] = (Music){rest, _8th, 60, 5, 1};
    INDEX = 0; COUNT = 0; Save_Note = Song[0].note; Music_ON = 1;
    music_timer = 0;     /* No auto-stop for miss sound */
}

/** Play a short beep when a boat segment is placed */
void Play_Place_Sound(void) {
    Song[0] = (Music){E4, _16th, 60, 5, 1};
    INDEX = 0; COUNT = 0; Save_Note = Song[0].note; Music_ON = 1;
    music_timer = 0;     /* No auto-stop for place beep */
}

/** Load the victory song */
void Init_Victory_Song(void) {
    Song[0]  = (Music){D4, quarter, 120, 10, 0};
    Song[1]  = (Music){D4, quarter, 120, 10, 0};
    Song[2]  = (Music){D4, quarter, 120, 10, 0};
    Song[3]  = (Music){D4, _8th,    120, 10, 0};
    Song[4]  = (Music){E4, _8th,    120, 10, 0};
    Song[5]  = (Music){F4, quarter, 120, 10, 0};
    Song[6]  = (Music){F4, quarter, 120, 10, 0};
    Song[7]  = (Music){F4, quarter, 120, 10, 0};
    Song[8]  = (Music){F4, _8th,    120, 10, 0};
    Song[9]  = (Music){E4, _8th,    120, 10, 0};
    Song[10] = (Music){D4, quarter, 120, 10, 0};
    Song[11] = (Music){D4, quarter, 120, 10, 0};
    Song[12] = (Music){F4, quarter, 120, 10, 0};
    Song[13] = (Music){A4, quarter, 120, 10, 0};
    Song[14] = (Music){A4, half,    120, 10, 0};
    Song[15] = (Music){G4, quarter, 120, 10, 0};
    Song[16] = (Music){F4, quarter, 120, 10, 0};
    Song[17] = (Music){E4, quarter, 120, 10, 0};
    Song[18] = (Music){D4, whole,   120, 10, 1}; /* end marker */
    INDEX = 0; COUNT = 0; Save_Note = Song[0].note; Music_ON = 1;
    music_timer = 0;     /* No auto-stop for victory song */
}

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
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  //MX_I2C1_Init();
  //MX_I2S3_Init();
  //MX_SPI1_Init();
  //MX_USB_HOST_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  /*** Configure GPIOs ***/
  GPIOD->MODER = 0x55555555;   /* Port D: all outputs (LEDs + speaker on PD0) */
  GPIOA->MODER |= 0x00000030;  /* Port A: PA2 analog (pot Y-axis) */
  GPIOA->MODER |= 0x0000000C;  /* Port A: PA1 analog (pot X-axis) */
  GPIOA->MODER &= ~(3u << 0);  /* Force PA0 to input (onboard blue button) */
  GPIOA->MODER &= ~(3u << 6);  /* Force PA3 to input (orientation switch) */
  GPIOE->MODER |= 0x55555555;  /* Port E: all outputs (7-seg data + digit select) */
  GPIOE->ODR = 0xFFFF;         /* All Port E high (7-seg blanked, common anode) */

  /*** Configure ADC1 for potentiometer reading ***/
  RCC->APB2ENR |= 1<<8;   /* Turn on ADC1 clock */
  ADC1->SMPR2 |= 1;       /* 15 clock cycles per sample */
  ADC1->CR2 |= 1;         /* Turn on ADC1 */

  /*** Timer 7: sound generation (125 KHz interrupt rate) ***/
  TIM7->PSC = 199;    /* 250 KHz timer clock = 50 MHz / 200 */
  TIM7->ARR = 1;      /* Divide by 2 → 125 KHz interrupt */
  TIM7->DIER |= 1;    /* Enable timer 7 interrupt */
  TIM7->CR1 |= 1;     /* Start timer counting */

  /* Startup blink: flash all 4 LEDs to confirm code is running */
  GPIOD->ODR |= (0xF << 12);
  HAL_Delay(300);
  GPIOD->ODR &= ~(0xF << 12);

  /* Start the game with the BATTLESHIP marquee */
  Set_Marquee(Msg_Start, sizeof(Msg_Start));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
      volatile int timeout;
      static uint32_t x_smooth = 2048;  /* Running average for X pot */
      static uint32_t y_smooth = 2048;  /* Running average for Y pot */

      /* PA1 (ADC Channel 1) → X-axis horizontal cursor */
      ADC1->SQR3 = 1;
      ADC1->CR2 |= (1 << 30);
      timeout = 100000;
      while(!(ADC1->SR & 2) && --timeout > 0);
      if (timeout > 0) {
          uint32_t sample = ADC1->DR;
          x_smooth = (x_smooth * 3 + sample) / 4;  /* 75/25 EMA filter */
      }
      uint32_t x_raw = x_smooth;

      /* PA2 (ADC Channel 2) → Y-axis vertical cursor */
      ADC1->SQR3 = 2;
      ADC1->CR2 |= (1 << 30);
      timeout = 100000;
      while(!(ADC1->SR & 2) && --timeout > 0);
      if (timeout > 0) {
          uint32_t sample = ADC1->DR;
          y_smooth = (y_smooth * 3 + sample) / 4;
      }
      uint32_t y_raw = y_smooth;

      /*--- Read Orientation Switch (PA3) ---*/
      cursor_v_h = !HAL_GPIO_ReadPin(ORIENT_SW_GPIO_Port, ORIENT_SW_Pin);

      /*--- Map ADC to cursor grid position ---*/
      /* Physical display: digit 7 = leftmost, digit 0 = rightmost */
      if (cursor_v_h == 0) {
          /* Vertical mode: 16 columns × 2 rows */
          cursor_x = 15 - (x_raw * 16) / 4096;  /* Reversed to match display */
          cursor_y = (y_raw * 2)  / 4096;
          if (cursor_x > 15) cursor_x = 0;
          if (cursor_y > 1)  cursor_y = 1;
      } else {
          /* Horizontal mode: 8 columns × 3 rows */
          cursor_x = 7 - (x_raw * 8) / 4096;    /* Reversed to match display */
          cursor_y = (y_raw * 3) / 4096;
          if (cursor_x > 7) cursor_x = 0;
          if (cursor_y > 2) cursor_y = 2;
      }

      /*=================================================================
       * GAME STATE MACHINE
       *=================================================================*/
      switch (current_state) {

          /*--- START: Scroll "BATTLESHIP", wait for button ---*/
          case STATE_START:
              if (Button_Pressed()) {
                  GPIOD->ODR ^= (1 << 12);  /* Toggle green LED for debug */
                  Set_Marquee(Msg_P1Place, sizeof(Msg_P1Place));
                  current_state = STATE_P1_PLACE_MSG;
              }
              break;

          /*--- P1 PLACE MSG: Announce "P1 PLACE", wait for button ---*/
          case STATE_P1_PLACE_MSG:
              if (Button_Pressed()) {
                  Animate_On = 0;
                  segments_placed = 0;
                  current_state = STATE_P1_PLACE;
              }
              break;

          /*--- P1 PLACE: Player 1 places 7 boat segments ---*/
          case STATE_P1_PLACE:
              Animate_On = 0;
              Update_Battleship_Display(&p1, NULL, cursor_x, cursor_y, cursor_v_h, 0);
              if (Button_Pressed()) {
                  uint32_t bit;
                  uint8_t already = 0;
                  if (cursor_v_h == 0) {
                      bit = vert_bit(cursor_x, cursor_y);
                      if (p1.vert_boats & bit) already = 1;
                      else p1.vert_boats |= bit;
                  } else {
                      /* Horizontal map: row*8 + col */
                      bit = 1u << (cursor_y * 8 + cursor_x);
                      if (p1.horiz_boats & bit) already = 1;
                      else p1.horiz_boats |= bit;
                  }
                  if (!already) {
                      segments_placed++;
                      Play_Place_Sound();
                      if (segments_placed >= MAX_SEGMENTS) {
                          Set_Marquee(Msg_P2Place, sizeof(Msg_P2Place));
                          current_state = STATE_P2_PLACE_MSG;
                      }
                  }
              }
              break;

          /*--- P2 PLACE MSG: Announce "P2 PLACE", wait for button ---*/
          case STATE_P2_PLACE_MSG:
              if (Button_Pressed()) {
                  Animate_On = 0;
                  segments_placed = 0;
                  current_state = STATE_P2_PLACE;
              }
              break;

          /*--- P2 PLACE: Player 2 places 7 boat segments ---*/
          case STATE_P2_PLACE:
              Animate_On = 0;
              Update_Battleship_Display(&p2, NULL, cursor_x, cursor_y, cursor_v_h, 0);
              if (Button_Pressed()) {
                  uint32_t bit;
                  uint8_t already = 0;
                  if (cursor_v_h == 0) {
                      bit = vert_bit(cursor_x, cursor_y);
                      if (p2.vert_boats & bit) already = 1;
                      else p2.vert_boats |= bit;
                  } else {
                      bit = 1u << (cursor_y * 8 + cursor_x);
                      if (p2.horiz_boats & bit) already = 1;
                      else p2.horiz_boats |= bit;
                  }
                  if (!already) {
                      segments_placed++;
                      Play_Place_Sound();
                      if (segments_placed >= MAX_SEGMENTS) {
                          Set_Marquee(Msg_P1Turn, sizeof(Msg_P1Turn));
                          current_state = STATE_P1_TURN_MSG;
                      }
                  }
              }
              break;

          /*--- P1 TURN MSG: Announce "P1 TURN", wait for button ---*/
          case STATE_P1_TURN_MSG:
              if (Button_Pressed()) {
                  Animate_On = 0;
                  current_state = STATE_P1_TURN;
              }
              break;

          /*--- P1 TURN: Player 1 fires at Player 2's board ---*/
          case STATE_P1_TURN:
              Animate_On = 0;
              /* Show P1's attack view of P2's board */
              Update_Battleship_Display(&p2, NULL, cursor_x, cursor_y, cursor_v_h, 1);
              if (Button_Pressed()) {
                  uint32_t bit;
                  uint8_t hit = 0;
                  if (cursor_v_h == 0) {
                      bit = vert_bit(cursor_x, cursor_y);
                      if (!(p2.vert_shots & bit)) {     /* Not already shot here */
                          p2.vert_shots |= bit;
                          if (p2.vert_boats & bit) hit = 1;
                      }
                  } else {
                      bit = 1u << (cursor_y * 8 + cursor_x);
                      if (!(p2.horiz_shots & bit)) {
                          p2.horiz_shots |= bit;
                          if (p2.horiz_boats & bit) hit = 1;
                      }
                  }
                  if (hit) {
                      p1_hits++;
                      Play_Hit_Sound();
                      if (p1_hits >= MAX_SEGMENTS) {
                          Set_Marquee(Msg_P1Wins, sizeof(Msg_P1Wins));
                          Init_Victory_Song();
                          current_state = STATE_GAME_OVER;
                          break;
                      }
                  } else {
                      Play_Miss_Sound();
                  }
                  /* Switch to P2's turn after a short delay */
                  HAL_Delay(1000);
                  Set_Marquee(Msg_P2Turn, sizeof(Msg_P2Turn));
                  current_state = STATE_P2_TURN_MSG;
              }
              break;

          /*--- P2 TURN MSG: Announce "P2 TURN", wait for button ---*/
          case STATE_P2_TURN_MSG:
              if (Button_Pressed()) {
                  Animate_On = 0;
                  current_state = STATE_P2_TURN;
              }
              break;

          /*--- P2 TURN: Player 2 fires at Player 1's board ---*/
          case STATE_P2_TURN:
              Animate_On = 0;
              /* Show P2's attack view of P1's board */
              Update_Battleship_Display(&p1, NULL, cursor_x, cursor_y, cursor_v_h, 1);
              if (Button_Pressed()) {
                  uint32_t bit;
                  uint8_t hit = 0;
                  if (cursor_v_h == 0) {
                      bit = vert_bit(cursor_x, cursor_y);
                      if (!(p1.vert_shots & bit)) {
                          p1.vert_shots |= bit;
                          if (p1.vert_boats & bit) hit = 1;
                      }
                  } else {
                      bit = 1u << (cursor_y * 8 + cursor_x);
                      if (!(p1.horiz_shots & bit)) {
                          p1.horiz_shots |= bit;
                          if (p1.horiz_boats & bit) hit = 1;
                      }
                  }
                  if (hit) {
                      p2_hits++;
                      Play_Hit_Sound();
                      if (p2_hits >= MAX_SEGMENTS) {
                          Set_Marquee(Msg_P2Wins, sizeof(Msg_P2Wins));
                          Init_Victory_Song();
                          current_state = STATE_GAME_OVER;
                          break;
                      }
                  } else {
                      Play_Miss_Sound();
                  }
                  HAL_Delay(1000);
                  Set_Marquee(Msg_P1Turn, sizeof(Msg_P1Turn));
                  current_state = STATE_P1_TURN_MSG;
              }
              break;

          /*--- GAME OVER: Winner announced, victory song plays ---*/
          case STATE_GAME_OVER:
              /* Marquee + song already running; just loop here */
              break;
      }

      HAL_Delay(50);  /* ~20 Hz polling rate for smooth cursor + debounce */

    /* USER CODE BEGIN 3 */
  }
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */


/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 0;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 65535;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_I2C_SPI_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PDM_OUT_Pin */
  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin (Action Button on PC11) */
  memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : ORIENT_SW_Pin (Orientation Switch on PA3) */
  memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK_IN_Pin */
  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                           Audio_RST_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */



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
