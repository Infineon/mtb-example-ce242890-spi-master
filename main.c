/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the PSOC Control C1 MCU: SPI Master Example
*              for ModusToolbox. This code example shows how to transfer 3 bytes of
*              data using SPI in PSOC Control C1 MCU. The successful transfer of bytes is
*              indicated by the toggling of an LED.
*
* Related Document: See README.md
*
*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "cybsp.h"
#include "cy_utils.h"
#include <stdio.h>
#include "cy_retarget_io.h"

/*******************************************************************************
* Defines
*******************************************************************************/

/* Declarations for LED toggle and SPI transmission timing */
#define TICKS_PER_SECOND       (1000u)
#define TICKS_WAIT             (1000u)

/* Define macro to enable/disable printing of debug messages */
#define ENABLE_DEBUG_PRINT     (0)

/* Define macro to check if loop is entered once */
#if ENABLE_DEBUG_PRINT
static bool ENTER_LOOP = false;
static uint8_t LOOP_NUM = 0;
#endif

/* Declaration of array to store the message to be transmitted */
const uint8_t data[3] = {0x84, 0x0F, 0x84};


/*******************************************************************************
* Function Name: SysTick_Handler
********************************************************************************
* Summary:
* This is the interrupt handler function for the SysTick timer interrupt.
* It counts the time elapsed in milliseconds since the timer started.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void SysTick_Handler(void)
{
    /* Variables Initialization */
    uint8_t i = 0;
    static uint32_t ticks = 0;

    ticks++;
    if (ticks == TICKS_WAIT)
    {
        /* Enable the selected slave Select line 0 */
        Cy_SPI_CH_EnableSlaveSelect(SPI0_HW, CY_SPI_CH_SLAVE_SELECT_0);
        /* Sending 3 messages from data array */
        while(i < 3)
        {
            Cy_SPI_CH_Transmit(SPI0_HW, data[i++], CY_SPI_CH_MODE_STANDARD);
            while((Cy_SPI_CH_GetStatusFlag(SPI0_HW) & CY_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION) == 0U);
            Cy_SPI_CH_ClearStatusFlag(SPI0_HW, CY_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION);
        }
        /* Transfer completed successfully. Toggle the LED */
        Cy_GPIO_ToggleOutput(CYBSP_USER_LED1_PORT, CYBSP_USER_LED1_PIN);

        /* Disable Slave Select line */
        Cy_SPI_CH_DisableSlaveSelect(SPI0_HW);

        #if ENABLE_DEBUG_PRINT
        ENTER_LOOP = true;
        #endif

        ticks = 0;
    }
}

/*******************************************************************************
 * Function Name: main
 ********************************************************************************
 * Summary:
 * This is the main function. It initializes the SPI interface and configures
 * SPI block for transmitting data.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  int
 *
*******************************************************************************/

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }


    result = cy_retarget_io_init(CYBSP_DEBUG_UART_HW);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    #if ENABLE_DEBUG_PRINT
    printf("Initialization done\r\n");
    #endif

    /* Start the SPI Channel */
    Cy_SPI_CH_Start( SPI0_HW );

    /* System timer configuration */
    SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);

    while(1)
    {
        #if ENABLE_DEBUG_PRINT
        if(ENTER_LOOP && LOOP_NUM == 0)
        {
            printf("Data sent\r\n");
            LOOP_NUM++;
        }
        #endif
    }
}

/* [] END OF FILE */
