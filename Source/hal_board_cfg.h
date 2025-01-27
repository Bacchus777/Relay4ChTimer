#ifndef HAL_BOARD_CFG_H
#define HAL_BOARD_CFG_H


/* ------------------------------------------------------------------------------------------------
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "zcl_app.h"

/* ------------------------------------------------------------------------------------------------
 *                                       CC2590/CC2591 support
 *
 *                        Define HAL_PA_LNA_CC2590 if CC2530+CC2590EM is used
 *                        Define HAL_PA_LNA if CC2530+CC2591EM is used
 *                        Note that only one of them can be defined
 * ------------------------------------------------------------------------------------------------
 */
#define xHAL_PA_LNA
#define xHAL_PA_LNA_CC2590
#define xHAL_PA_LNA_SE2431L
#define xHAL_PA_LNA_CC2592

/* ------------------------------------------------------------------------------------------------
 *                                          Clock Speed
 * ------------------------------------------------------------------------------------------------
 */

#define HAL_CPU_CLOCK_MHZ     32

/* This flag should be defined if the SoC uses the 32MHz crystal
 * as the main clock source (instead of DCO).
 */
#define HAL_CLOCK_CRYSTAL

/* 32 kHz clock source select in CLKCONCMD */
#if !defined (OSC32K_CRYSTAL_INSTALLED) || (defined (OSC32K_CRYSTAL_INSTALLED) && (OSC32K_CRYSTAL_INSTALLED == TRUE))
  #define OSC_32KHZ  0x00 /* external 32 KHz xosc */
#else
  #define OSC_32KHZ  0x80 /* internal 32 KHz rcosc */
#endif

#define HAL_CLOCK_STABLE()    st( while (CLKCONSTA != (CLKCONCMD_32MHZ | OSC_32KHZ)); )

/* ------------------------------------------------------------------------------------------------
 *                                       LED Configuration
 * ------------------------------------------------------------------------------------------------
 */

#if defined (HAL_BOARD_CHDTECH_DEV)
  #define HAL_NUM_LEDS            5
#elif defined(HAL_BOARD_TARGET)
  #define HAL_NUM_LEDS            5
#elif defined(HAL_BOARD_MODKAM)
  #define HAL_NUM_LEDS            5
#else
  #error Unknown Board Indentifier
#endif

#define HAL_LED_BLINK_DELAY()   st( { volatile uint32 i; for (i=0; i<0x5800; i++) { }; } )


//#if defined(HAL_BOARD_TARGET)

#define LED1_BV           BV(3)
#define LED1_SBIT         P1_3
#define LED1_DDR          P1DIR
#define LED1_POLARITY     ACTIVE_HIGH

#define LED2_BV           BV(2)
#define LED2_SBIT         P1_2
#define LED2_DDR          P1DIR
#define LED2_POLARITY     ACTIVE_HIGH

#define LED3_BV           BV(6)
#define LED3_SBIT         P1_6
#define LED3_DDR          P1DIR
#define LED3_POLARITY     ACTIVE_HIGH

#define LED4_BV           BV(5)
#define LED4_SBIT         P1_5
#define LED4_DDR          P1DIR
#define LED4_POLARITY     ACTIVE_HIGH

#define LED5_BV           BV(4)
#define LED5_SBIT         P1_4
#define LED5_DDR          P1DIR
#define LED5_POLARITY     ACTIVE_HIGH

/*#elif defined (HAL_BOARD_CHDTECH_DEV)

#define LED1_BV           BV(4)
#define LED1_SBIT         P1_4
#define LED1_DDR          P1DIR
#define LED1_POLARITY     ACTIVE_LOW

#define LED2_BV           BV(1)
#define LED2_SBIT         P1_1
#define LED2_DDR          P1DIR
#define LED2_POLARITY     ACTIVE_LOW

#define LED3_BV           BV(0)
#define LED3_SBIT         P1_0
#define LED3_DDR          P1DIR
#define LED3_POLARITY     ACTIVE_LOW

#define LED4_BV           BV(4)
#define LED4_SBIT         P1_4
#define LED4_DDR          P1DIR
#define LED4_POLARITY     ACTIVE_LOW

#define LED5_BV           BV(5)
#define LED5_SBIT         P1_5
#define LED5_DDR          P1DIR
#define LED5_POLARITY     ACTIVE_HIGH

#endif
*/


/* ------------------------------------------------------------------------------------------------
 *                                    Push Button Configuration
 * ------------------------------------------------------------------------------------------------
 */

#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!    /* double negation forces result to be '1' */

/* S1 */
/*#define PUSH1_BV          BV(3)
#define PUSH1_SBIT        P1_3
#define PUSH1_POLARITY    ACTIVE_HIGH
*/



/* ------------------------------------------------------------------------------------------------
 *                         OSAL NV implemented by internal flash pages.
 * ------------------------------------------------------------------------------------------------
 */

// Flash is partitioned into 8 banks of 32 KB or 16 pages.
#define HAL_FLASH_PAGE_PER_BANK    16
// Flash is constructed of 128 pages of 2 KB.
#define HAL_FLASH_PAGE_SIZE        2048
#define HAL_FLASH_WORD_SIZE        4

// CODE banks get mapped into the XDATA range 8000-FFFF.
#define HAL_FLASH_PAGE_MAP         0x8000

// The last 16 bytes of the last available page are reserved for flash lock bits.
// NV page definitions must coincide with segment declaration in project *.xcl file.
#if defined NON_BANKED
#define HAL_FLASH_LOCK_BITS        16
#define HAL_NV_PAGE_END            30
#define HAL_NV_PAGE_CNT            2
#else
#define HAL_FLASH_LOCK_BITS        16
#define HAL_NV_PAGE_END            126
#define HAL_NV_PAGE_CNT            6
#endif

// Re-defining Z_EXTADDR_LEN here so as not to include a Z-Stack .h file.
#define HAL_FLASH_IEEE_SIZE        8
#define HAL_FLASH_IEEE_PAGE       (HAL_NV_PAGE_END+1)
#define HAL_FLASH_IEEE_OSET       (HAL_FLASH_PAGE_SIZE - HAL_FLASH_LOCK_BITS - HAL_FLASH_IEEE_SIZE)
#define HAL_INFOP_IEEE_OSET        0xC

#define HAL_FLASH_DEV_PRIVATE_KEY_OSET     0x7D2
#define HAL_FLASH_CA_PUBLIC_KEY_OSET       0x7BC
#define HAL_FLASH_IMPLICIT_CERT_OSET       0x78C

#define HAL_NV_PAGE_BEG           (HAL_NV_PAGE_END-HAL_NV_PAGE_CNT+1)
// Used by DMA macros to shift 1 to create a mask for DMA registers.
#define HAL_NV_DMA_CH              0
#define HAL_DMA_CH_RX              3
#define HAL_DMA_CH_TX              4

#define HAL_NV_DMA_GET_DESC()      HAL_DMA_GET_DESC0()
#define HAL_NV_DMA_SET_ADDR(a)     HAL_DMA_SET_ADDR_DESC0((a))

/* ------------------------------------------------------------------------------------------------
 *  Serial Boot Loader: reserving the first 4 pages of flash and other memory in cc2530-sb.xcl.
 * ------------------------------------------------------------------------------------------------
 */

#define HAL_SB_IMG_ADDR       0x2000
#define HAL_SB_CRC_ADDR       0x2090
// Size of internal flash less 4 pages for boot loader, 6 pages for NV, & 1 page for lock bits.
#define HAL_SB_IMG_SIZE      (0x40000 - 0x2000 - 0x3000 - 0x0800)

/* ------------------------------------------------------------------------------------------------
 *                                            Macros
 * ------------------------------------------------------------------------------------------------
 */

/* ----------- RF-frontend Connection Initialization ---------- */
#if defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590 || \
    defined HAL_PA_LNA_SE2431L || defined HAL_PA_LNA_CC2592
extern void MAC_RfFrontendSetup(void);
#define HAL_BOARD_RF_FRONTEND_SETUP() MAC_RfFrontendSetup()
#else
#define HAL_BOARD_RF_FRONTEND_SETUP()
#endif

/* ----------- Cache Prefetch control ---------- */
#define PREFETCH_ENABLE()     st( FCTL = 0x08; )
#define PREFETCH_DISABLE()    st( FCTL = 0x04; )


/* ----------- Board Initialization ---------- */
#if defined (HAL_BOARD_CHDTECH_DEV) || (!defined(HAL_PA_LNA) && !defined(HAL_PA_LNA_CC2592))
#define HAL_BOARD_INIT()                                         \
{                                                                \
  uint16 i;                                                      \
                                                                 \
  SLEEPCMD &= ~OSC_PD;                       /* turn on 16MHz RC and 32MHz XOSC */                \
  while (!(SLEEPSTA & XOSC_STB));            /* wait for 32MHz XOSC stable */                     \
  asm("NOP");                                /* chip bug workaround */                            \
  for (i=0; i<504; i++) asm("NOP");          /* Require 63us delay for all revs */                \
  CLKCONCMD = (CLKCONCMD_32MHZ | OSC_32KHZ); /* Select 32MHz XOSC and the source for 32K clock */ \
  while (CLKCONSTA != (CLKCONCMD_32MHZ | OSC_32KHZ)); /* Wait for the change to be effective */   \
  SLEEPCMD |= OSC_PD;                        /* turn off 16MHz RC */                              \
                                                                 \
  /* Turn on cache prefetch mode */                              \
  PREFETCH_ENABLE();                                             \
                                                                 \
  LED1_DDR |= LED1_BV;                                           \
  LED2_DDR |= LED2_BV;                                           \
  LED3_DDR |= LED3_BV;                                           \
  LED4_DDR |= LED4_BV;                                           \
  LED5_DDR |= LED5_BV;                                           \
                                                                 \
}

#elif defined (HAL_PA_LNA)
#define HAL_BOARD_INIT()                                         \
{                                                                \
  uint16 i;                                                      \
                                                                 \
  SLEEPCMD &= ~OSC_PD;                       /* turn on 16MHz RC and 32MHz XOSC */                \
  while (!(SLEEPSTA & XOSC_STB));            /* wait for 32MHz XOSC stable */                     \
  asm("NOP");                                /* chip bug workaround */                            \
  for (i=0; i<504; i++) asm("NOP");          /* Require 63us delay for all revs */                \
  CLKCONCMD = (CLKCONCMD_32MHZ | OSC_32KHZ); /* Select 32MHz XOSC and the source for 32K clock */ \
  while (CLKCONSTA != (CLKCONCMD_32MHZ | OSC_32KHZ)); /* Wait for the change to be effective */   \
  SLEEPCMD |= OSC_PD;                        /* turn off 16MHz RC */                              \
                                                                 \
  /* Turn on cache prefetch mode */                              \
  PREFETCH_ENABLE();                                             \
                                                                 \
  /* set direction for GPIO outputs  */                          \
  /* For SE2431L PA LNA this sets ANT_SEL to output */           \
  /* For CC2592 this enables LNA */                              \
  P1DIR |= BV(0) | BV(1);                                        \
                                                                 \
  /* Set PA/LNA HGM control P0_7 */                              \
  P0DIR |= BV(7);                                                \
                                                                 \
                                                                 \
  /* setup RF frontend if necessary */                           \
  HAL_BOARD_RF_FRONTEND_SETUP();                                 \
  LED1_DDR |= LED1_BV;                                           \
  LED2_DDR |= LED2_BV;                                           \
  LED3_DDR |= LED3_BV;                                           \
  LED4_DDR |= LED4_BV;                                           \
  LED5_DDR |= LED5_BV;                                           \
                                                                 \
}

#elif defined (HAL_PA_LNA_CC2592) || defined (HAL_PA_LNA_SE2431L)
#define HAL_BOARD_INIT()                                         \
{                                                                \
  uint16 i;                                                      \
                                                                 \
  SLEEPCMD &= ~OSC_PD;                       /* turn on 16MHz RC and 32MHz XOSC */                \
  while (!(SLEEPSTA & XOSC_STB));            /* wait for 32MHz XOSC stable */                     \
  asm("NOP");                                /* chip bug workaround */                            \
  for (i=0; i<504; i++) asm("NOP");          /* Require 63us delay for all revs */                \
  CLKCONCMD = (CLKCONCMD_32MHZ | OSC_32KHZ); /* Select 32MHz XOSC and the source for 32K clock */ \
  while (CLKCONSTA != (CLKCONCMD_32MHZ | OSC_32KHZ)); /* Wait for the change to be effective */   \
  SLEEPCMD |= OSC_PD;                        /* turn off 16MHz RC */                              \
                                                                 \
  /* Turn on cache prefetch mode */                              \
  PREFETCH_ENABLE();                                             \
                                                                 \
  /* set direction for GPIO outputs  */                          \
  /* For SE2431L PA LNA this sets ANT_SEL to output */           \
  /* For CC2592 this enables LNA */                              \
  P1DIR |= BV(0) | BV(1);                                        \
                                                                 \
  /* Set PA/LNA HGM control P0_7 */                              \
  P0DIR |= BV(7);                                                \
                                                                 \
                                                                 \
  /* setup RF frontend if necessary */                           \
  HAL_BOARD_RF_FRONTEND_SETUP();                                 \
  LED1_DDR |= LED1_BV;                                           \
  LED2_DDR |= LED2_BV;                                           \
  LED3_DDR |= LED3_BV;                                           \
  LED4_DDR |= LED4_BV;                                           \
  LED5_DDR |= LED5_BV;                                           \
                                                                 \
}
#endif

/* ----------- Debounce ---------- */
#define HAL_DEBOUNCE(expr)    { int i; for (i=0; i<50; i++) { if (!(expr)) i = 0; } }


/* ----------- Push Buttons ---------- */

/////////////////////// SW1 /////////////////////// 

/* SW1 is at P0.4 */
#define HAL_KEY_SW_1_PORT   P0
#define HAL_KEY_SW_1_SBIT   P0_5
#define HAL_KEY_SW_1_BIT    BV(5)
#define HAL_KEY_SW_1_SEL    P0SEL
#define HAL_KEY_SW_1_DIR    P0DIR
#define HAL_KEY_SW_1_POLARITY      ACTIVE_LOW

/* edge interrupt */
#define HAL_KEY_SW_1_EDGEBIT  BV(0)
#define HAL_KEY_SW_1_EDGE     HAL_KEY_FALLING_EDGE

/* SW_1 interrupts */
#define HAL_KEY_SW_1_IEN      IEN1  /* CPU interrupt mask register */
#define HAL_KEY_SW_1_IENBIT   BV(1) /* Mask bit for all of Port_0 */
#define HAL_KEY_SW_1_ICTL     P0IEN /* Port Interrupt Control register */
#define HAL_KEY_SW_1_ICTLBIT  BV(3) /* P0IEN - P0.1 enable/disable bit */
#define HAL_KEY_SW_1_PXIFG    P0IFG /* Interrupt flag at source */


/////////////////////// SW2 /////////////////////// 


/* SW2 is at P2.0 */
#define HAL_KEY_SW_2_PORT   P0
#define HAL_KEY_SW_2_SBIT   P0_4
#define HAL_KEY_SW_2_BIT    BV(4)
#define HAL_KEY_SW_2_SEL    P0SEL
#define HAL_KEY_SW_2_DIR    P0DIR
#define HAL_KEY_SW_2_POLARITY      ACTIVE_LOW

/* edge interrupt */
#define HAL_KEY_SW_2_EDGEBIT  BV(0)
#define HAL_KEY_SW_2_EDGE     HAL_KEY_FALLING_EDGE

/* SW_2 interrupts */
#define HAL_KEY_SW_2_IEN      IEN1  /* CPU interrupt mask register */
#define HAL_KEY_SW_2_IENBIT   BV(1) /* Mask bit for all of Port_0 */
#define HAL_KEY_SW_2_ICTL     P0IEN /* Port Interrupt Control register */
#define HAL_KEY_SW_2_ICTLBIT  BV(3) /* P0IEN - P0.1 enable/disable bit */
#define HAL_KEY_SW_2_PXIFG    P0IFG /* Interrupt flag at source */


/////////////////////// SW3 /////////////////////// 

/* SW3 is at P0.5 */
#define HAL_KEY_SW_3_PORT   P0
#define HAL_KEY_SW_3_SBIT   P0_1
#define HAL_KEY_SW_3_BIT    BV(1)
#define HAL_KEY_SW_3_SEL    P0SEL
#define HAL_KEY_SW_3_DIR    P0DIR
#define HAL_KEY_SW_3_POLARITY      ACTIVE_LOW

/* edge interrupt */
#define HAL_KEY_SW_3_EDGEBIT  BV(0)
#define HAL_KEY_SW_3_EDGE     HAL_KEY_FALLING_EDGE

/* SW_3 interrupts */
#define HAL_KEY_SW_3_IEN      IEN1  /* CPU interrupt mask register */
#define HAL_KEY_SW_3_IENBIT   BV(1) /* Mask bit for all of Port_0 */
#define HAL_KEY_SW_3_ICTL     P0IEN /* Port Interrupt Control register */
#define HAL_KEY_SW_3_ICTLBIT  BV(3) /* P0IEN - P0.1 enable/disable bit */
#define HAL_KEY_SW_3_PXIFG    P0IFG /* Interrupt flag at source */


/////////////////////// SW4 /////////////////////// 

/* SW4 is at P0.6 */
#define HAL_KEY_SW_4_PORT   P0
#define HAL_KEY_SW_4_SBIT   P0_0
#define HAL_KEY_SW_4_BIT    BV(0)
#define HAL_KEY_SW_4_SEL    P0SEL
#define HAL_KEY_SW_4_DIR    P0DIR
#define HAL_KEY_SW_4_POLARITY      ACTIVE_LOW

/* edge interrupt */
#define HAL_KEY_SW_4_EDGEBIT  BV(0)
#define HAL_KEY_SW_4_EDGE     HAL_KEY_FALLING_EDGE

/* SW_4 interrupts */
#define HAL_KEY_SW_4_IEN      IEN1  /* CPU interrupt mask register */
#define HAL_KEY_SW_4_IENBIT   BV(1) /* Mask bit for all of Port_0 */
#define HAL_KEY_SW_4_ICTL     P0IEN /* Port Interrupt Control register */
#define HAL_KEY_SW_4_ICTLBIT  BV(3) /* P0IEN - P0.1 enable/disable bit */
#define HAL_KEY_SW_4_PXIFG    P0IFG /* Interrupt flag at source */






#define HAL_PUSH_BUTTON1()        (HAL_KEY_SW_1_POLARITY (HAL_KEY_SW_1_SBIT))
#define HAL_PUSH_BUTTON2()        (HAL_KEY_SW_2_POLARITY (HAL_KEY_SW_2_SBIT))
#define HAL_PUSH_BUTTON3()        (HAL_KEY_SW_3_POLARITY (HAL_KEY_SW_3_SBIT))
#define HAL_PUSH_BUTTON4()        (HAL_KEY_SW_4_POLARITY (HAL_KEY_SW_4_SBIT))
#define HAL_PUSH_BUTTON5()        (HAL_KEY_SW_5_POLARITY (HAL_KEY_SW_5_SBIT))
#define HAL_PUSH_BUTTON6()        (HAL_KEY_SW_6_POLARITY (HAL_KEY_SW_6_SBIT))

/* ----------- LED's ---------- */
#define HAL_TURN_OFF_LED1()       st( LED1_SBIT = LED1_POLARITY (0); )
#define HAL_TURN_OFF_LED2()       st( LED2_SBIT = LED2_POLARITY (0); )
#define HAL_TURN_OFF_LED3()       st( LED3_SBIT = LED3_POLARITY (0); )
#define HAL_TURN_OFF_LED4()       st( LED4_SBIT = LED4_POLARITY (0); )
#define HAL_TURN_OFF_LED5()       st( LED5_SBIT = LED5_POLARITY (0); )

#define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )
#define HAL_TURN_ON_LED2()        st( LED2_SBIT = LED2_POLARITY (1); )
#define HAL_TURN_ON_LED3()        st( LED3_SBIT = LED3_POLARITY (1); )
#define HAL_TURN_ON_LED4()        st( LED4_SBIT = LED4_POLARITY (1); )
#define HAL_TURN_ON_LED5()        st( LED5_SBIT = LED5_POLARITY (1); )

#define HAL_TOGGLE_LED1()         st( if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} )
#define HAL_TOGGLE_LED2()         st( if (LED2_SBIT) { LED2_SBIT = 0; } else { LED2_SBIT = 1;} )
#define HAL_TOGGLE_LED3()         st( if (LED3_SBIT) { LED3_SBIT = 0; } else { LED3_SBIT = 1;} )
#define HAL_TOGGLE_LED4()         st( if (LED4_SBIT) { LED4_SBIT = 0; } else { LED4_SBIT = 1;} )
#define HAL_TOGGLE_LED5()         st( if (LED5_SBIT) { LED5_SBIT = 0; } else { LED5_SBIT = 1;} )

#define HAL_STATE_LED1()          (LED1_POLARITY (LED1_SBIT))
#define HAL_STATE_LED2()          (LED2_POLARITY (LED2_SBIT))
#define HAL_STATE_LED3()          (LED3_POLARITY (LED3_SBIT))
#define HAL_STATE_LED4()          (LED4_POLARITY (LED4_SBIT))
#define HAL_STATE_LED5()          (LED5_POLARITY (LED5_SBIT))

/* ----------- Minimum safe bus voltage ---------- */

// Vdd/3 / Internal Reference X ENOB --> (Vdd / 3) / 1.15 X 127
#define VDD_2_0  74   // 2.0 V required to safely read/write internal flash.
#define VDD_2_7  100  // 2.7 V required for the Numonyx device.
#define VDD_MIN_RUN  (VDD_2_0+4)  // VDD_MIN_RUN = VDD_MIN_NV
#define VDD_MIN_NV   (VDD_2_0+4)  // 5% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_GOOD (VDD_2_0+8)  // 10% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_XNV  (VDD_2_7+5)  // 5% margin over minimum to survive a page erase and compaction.

/* ------------------------------------------------------------------------------------------------
 *                                     Driver Configuration
 * ------------------------------------------------------------------------------------------------
 */

/* Set to TRUE enable H/W TIMER usage, FALSE disable it */
#ifndef HAL_TIMER
#define HAL_TIMER FALSE
#endif

/* Set to TRUE enable ADC usage, FALSE disable it */
#ifndef HAL_ADC
#define HAL_ADC TRUE
#endif

/* Set to TRUE enable DMA usage, FALSE disable it */
#ifndef HAL_DMA
#define HAL_DMA TRUE
#endif

/* Set to TRUE enable Flash access, FALSE disable it */
#ifndef HAL_FLASH
#define HAL_FLASH TRUE
#endif

/* Set to TRUE enable AES usage, FALSE disable it */
#ifndef HAL_AES
#define HAL_AES TRUE
#endif

#ifndef HAL_AES_DMA
#define HAL_AES_DMA TRUE
#endif

/* Set to TRUE enable LCD usage, FALSE disable it */
#ifndef HAL_LCD
#define HAL_LCD TRUE
#endif

/* Set to TRUE enable LED usage, FALSE disable it */
#ifndef HAL_LED
#define HAL_LED TRUE
#endif
#if (!defined BLINK_LEDS) && (HAL_LED == TRUE)
#define BLINK_LEDS
#endif

/* Set to TRUE enable KEY usage, FALSE disable it */
#ifndef HAL_KEY
#define HAL_KEY TRUE
#endif

/* Set to TRUE enable UART usage, FALSE disable it */
#ifndef HAL_UART
#if (defined ZAPP_P1) || (defined ZAPP_P2) || (defined ZTOOL_P1) || (defined ZTOOL_P2)
#define HAL_UART TRUE
#else
#define HAL_UART FALSE
#endif
#endif

#if HAL_UART
#ifndef HAL_UART_DMA
#if HAL_DMA
#if (defined ZAPP_P2) || (defined ZTOOL_P2)
#define HAL_UART_DMA  2
#else
#define HAL_UART_DMA  1
#endif
#else
#define HAL_UART_DMA  0
#endif
#endif

#ifndef HAL_UART_ISR
#if HAL_UART_DMA           // Default preference for DMA over ISR.
#define HAL_UART_ISR  0
#elif (defined ZAPP_P2) || (defined ZTOOL_P2)
#define HAL_UART_ISR  2
#else
#define HAL_UART_ISR  1
#endif
#endif

#if (HAL_UART_DMA && (HAL_UART_DMA == HAL_UART_ISR))
#error HAL_UART_DMA & HAL_UART_ISR must be different.
#endif

// Used to set P2 priority - USART0 over USART1 if both are defined.
#if ((HAL_UART_DMA == 1) || (HAL_UART_ISR == 1))
#define HAL_UART_PRIPO             0x00
#else
#define HAL_UART_PRIPO             0x40
#endif

#else
#define HAL_UART_DMA  0
#define HAL_UART_ISR  0
#endif

/* USB is not used for CC2530 configuration */
#define HAL_UART_USB  0
#endif
/*******************************************************************************************************
*/