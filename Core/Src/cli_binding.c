/*
 * 文件名: cli_binding.c
 * 作者: kuhn xin
 * 邮件： kunlong.xin@outlook.com
 * 创建时间: Oct 31, 2023
 * 许可证: MIT许可证
 * MIT许可证
 *
 * 版权所有（2023）年份 作者 kuhn xin
 *
 * 本软件及相关文档文件（“软件”）的著作权归作者所有。
 *
 * 根据以下条件，授予任何人免费使用、复制、修改、合并、发布、分发、再许可和/或销售本软件的副本：
 *
 * 上述版权声明和本许可声明应包含在本软件的所有副本或主要部分中。
 *
 * 本软件按“原样”提供，无任何明示或暗示的保证，包括但不限于对适销性、特定用途的适用性和非侵权性的保证。
 * 在任何情况下，作者或版权持有人均不对任何索赔、损害赔偿或其他责任承担任何责任，
 * 无论是在合同诉讼、侵权行为还是其他方面，即使是在使用本软件的可能性下。
 */

#include <stdio.h>

#include "cli_setup.h"
#include "cli_binding.h"

extern CAN_HandleTypeDef hcan1;
static uint8_t  get_time_seg1( uint32_t ts_reg );
static uint8_t  get_time_seg2( uint32_t ts_reg );

static uint8_t  get_time_seg1( uint32_t ts_reg )
{
    uint8_t time_seg;
    switch (ts_reg)
    {
        case CAN_BS1_1TQ  :  time_seg = 1u; break;
        case CAN_BS1_2TQ  :  time_seg = 2u; break;
        case CAN_BS1_3TQ  :  time_seg = 3u; break;
        case CAN_BS1_4TQ  :  time_seg = 4u; break;
        case CAN_BS1_5TQ  :  time_seg = 5u; break;
        case CAN_BS1_6TQ  :  time_seg = 6u; break;
        case CAN_BS1_7TQ  :  time_seg = 7u; break;
        case CAN_BS1_8TQ  :  time_seg = 8u; break;
        case CAN_BS1_9TQ  :  time_seg = 9u; break;
        case CAN_BS1_10TQ :  time_seg = 10u; break;
        case CAN_BS1_11TQ :  time_seg = 11u; break;
        case CAN_BS1_12TQ :  time_seg = 12u; break;
        case CAN_BS1_13TQ :  time_seg = 12u; break;
        case CAN_BS1_14TQ :  time_seg = 14u; break;
        case CAN_BS1_15TQ :  time_seg = 15u; break;
        case CAN_BS1_16TQ :  time_seg = 16u; break;
        default: time_seg = 0xFFu; break;
    }

    return time_seg;
}

static uint8_t  get_time_seg2( uint32_t ts_reg )
{
    uint8_t time_seg;
    switch (ts_reg)
    {
        case CAN_BS2_1TQ : time_seg = 1u; break;
        case CAN_BS2_2TQ : time_seg = 2u; break;
        case CAN_BS2_3TQ : time_seg = 3u; break;
        case CAN_BS2_4TQ : time_seg = 4u; break;
        case CAN_BS2_5TQ : time_seg = 5u; break;
        case CAN_BS2_6TQ : time_seg = 6u; break;
        case CAN_BS2_7TQ : time_seg = 7u; break;
        case CAN_BS2_8TQ : time_seg = 8u; break;
        default: time_seg = 0xFFu; break;
    }

    return time_seg;
}


void onClearCLI(EmbeddedCli *cli, char *args, void *context) {
	(void)cli;
	(void)args;
	(void)context;
    cli_printf("\33[2J");
}

void get_can_baud_rate(EmbeddedCli *cli, char *args, void *context)
{
	(void)cli;
	(void)args;
	(void)context;
	uint32_t clk ,baud_rate;
	uint8_t ts1,ts2;
	clk = HAL_RCC_GetPCLK1Freq();
	ts1 = get_time_seg1(hcan1.Init.TimeSeg1);
	ts2 = get_time_seg2(hcan1.Init.TimeSeg2);
	baud_rate = clk / (hcan1.Init.Prescaler * (ts1 + ts2 + 1u));
	cli_printf("CAN clock: %d Hz", clk);
	cli_printf("Prescaler: %d",hcan1.Init.Prescaler);
	cli_printf("TimeSeg1: %d",ts1);
	cli_printf("TimeSeg2: %d",ts2);
	cli_printf("CAN baud rate: %d bit/s",baud_rate);
}

void initCliBinding() {
    // Define bindings as local variables, so we don't waste static memory

    // Command binding for the clear command
    CliCommandBinding clear_binding = {
            .name = "clear",
            .help = "Clears the console",
            .tokenizeArgs = true,
            .context = NULL,
            .binding = onClearCLI
    };

    // Command binding for the led command
    CliCommandBinding baudrate_binding = {
            .name = "get-baudrate",
            .help = "Get CAN baud rate",
            .tokenizeArgs = true,
            .context = NULL,
            .binding = get_can_baud_rate
    };

    EmbeddedCli *cli = getCliPointer();
    embeddedCliAddBinding(cli, clear_binding);
    embeddedCliAddBinding(cli, baudrate_binding);
}
