/*
 * 文件名: slcan.c
 * 作者: kuhn xin
 * 邮件： kunlong.xin@outlook.com
 * 创建时间: Oct 26, 2023
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

#include "slcan.h"

const char firmware_version[] = {'V','1','0','1','0','\r'};

extern CAN_HandleTypeDef hcan1;

static uint8_t slcan_can_deinit( uint8_t* cmd_buff);
static uint8_t slcan_can_set_baud_rate( uint8_t* cmd_buff);
static uint8_t slcan_can_init( uint8_t* cmd_buff);
static uint8_t slcan_can_get_version( uint8_t* cmd_buff);
static uint8_t slcan_pc_send_normal_frame_to_bus( uint8_t* cmd_buff);
static uint8_t slcan_pc_send_extended_frame_to_bus( uint8_t* cmd_buff);

static slcan_tx_info_to_bus_t slcan_tx_info_to_bus_g;
static slcan_tx_info_to_pc_t slcan_tx_info_to_pc_g;

const slcan_baud_rate_map_t slcan_baud_rate_map_c[] =
{
    {.cmd = '0',.prescaler = 600u},/* 10000 */
    {.cmd = '1',.prescaler = 300u},/* 20000 */
    {.cmd = '2',.prescaler = 120u},/* 50000 */
    {.cmd = '3',.prescaler =  60u},/* 100000 */
    {.cmd = '4',.prescaler =  48u},/* 125000 */
    {.cmd = '5',.prescaler =  24u},/* 250000 */
    {.cmd = '6',.prescaler =  12u},/* 500000 */
    {.cmd = '7',.prescaler =  8u},/* 500000 */
    {.cmd = '8',.prescaler =  6u},/* 500000 */
};


const slcan_cmd_table_t slcan_cmd_table_c[]=
{
    {
        .cmd = 'C', /* Close channel command */
        .cmd_fun = slcan_can_deinit,
    },
    {
        .cmd = 'S', /* Set bitrate command */
        .cmd_fun = slcan_can_set_baud_rate,
    },
    {
        .cmd = 'V', /* firmware version */
        .cmd_fun = slcan_can_get_version,
    },
    {
        .cmd = 'O', /* Open channel command */
        .cmd_fun = slcan_can_init,
    },
    {
        .cmd = 't', /* Transmit normal data frame command */
        .cmd_fun = slcan_pc_send_normal_frame_to_bus,
    },
    {
        .cmd = 'T', /* Transmit extended data frame command */
        .cmd_fun = slcan_pc_send_extended_frame_to_bus,
    },
};


static uint8_t slcan_can_deinit( uint8_t* cmd_buff)
{
    uint8_t ret;
    if( *cmd_buff != '\r' )
    {
        ret = HAL_ERROR;
    }
    else
    {
        ret = HAL_CAN_DeInit(&hcan1);
    }

    return ret;
}

static uint8_t slcan_can_set_baud_rate( uint8_t* cmd_buff)
{
    uint16_t i;
    uint8_t ret = HAL_ERROR;
    if( *cmd_buff != '\r' )
    {
        ret = HAL_ERROR;
    }
    else
    {
        for( i = 0u; i < (sizeof(slcan_baud_rate_map_c)/sizeof(slcan_baud_rate_map_c[0])); i++ )
        {
            if( *cmd_buff == slcan_baud_rate_map_c[i].cmd )
            {
                hcan1.Init.Prescaler = slcan_baud_rate_map_c[i].prescaler;
                ret = HAL_OK;
                break;
            }
        }
    }

    if( HAL_OK != ret )
    {
        hcan1.Init.Prescaler = 12u;
    }
    return ret;
}

static uint8_t slcan_can_init( uint8_t* cmd_buff)
{
    uint8_t ret;

    CAN_FilterTypeDef  sFilterConfig;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;

    memset( (uint8_t *)&slcan_tx_info_to_bus_g, 0u, sizeof(slcan_tx_info_to_bus_g) );
    memset( (uint8_t *)&slcan_tx_info_to_pc_g, 0u, sizeof(slcan_tx_info_to_pc_g) );

    if( *cmd_buff != '\r' )
    {
        ret = HAL_ERROR;
    }
    else
    {
        if (HAL_CAN_Init(&hcan1) != HAL_OK)
        {
            Error_Handler();
        }

        if (HAL_CAN_Start(&hcan1) != HAL_OK)
        {
            Error_Handler();
        }

        if (HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
        {
            Error_Handler();
        }

        if ( HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
        {
            Error_Handler();
        }

    }

    return ret;
}

static uint8_t slcan_can_get_version( uint8_t* cmd_buff)
{
    uint8_t ret;
    if( *cmd_buff != '\r' )
    {
        ret = HAL_ERROR;
    }
    else
    {
        CDC_Transmit_FS((uint8_t *)firmware_version,sizeof(firmware_version));
    }
    return ret;
}

static uint8_t slcan_pc_send_normal_frame_to_bus(uint8_t* cmd_buff)
{
    uint32_t can_id;
    char can_id_str[4];

    char can_dlc_str[2];
    uint32_t can_dlc;

    char can_data_str[3];

    uint8_t head = slcan_tx_info_to_bus_g.head;

    /* CAN ID */
    strncpy(can_id_str, (char *)cmd_buff, 3);
    can_id_str[3] = '\0';
    can_id = strtol(can_id_str, NULL, 16);

    if ( can_id > 0x7FFu )
    {
        return HAL_ERROR;
    }
    
    /* CAN DLC */
    can_dlc_str[0] = (char)cmd_buff[3];
    can_dlc_str[1] = '\0';
    can_dlc = strtol(can_dlc_str, NULL, 16);

    if ( can_dlc > 8u )
    {
        return HAL_ERROR;
    }

    if( *( cmd_buff + can_dlc * 2u + 4u ) != '\r')
    {
        return HAL_ERROR;
    }

    /* CAN Data */
    for( uint8_t i = 0u; i < can_dlc; i++ )
    {
        can_data_str[0] = (char)cmd_buff[ 4u + i * 2u ];
        can_data_str[1] = (char)cmd_buff[ 4u + i * 2u + 1u ];
        can_data_str[2] = '\0';
        slcan_tx_info_to_bus_g.can_packets[head].can_data[i] = strtol(can_data_str, NULL, 16);
    }

    slcan_tx_info_to_bus_g.can_packets[head].CAN_TxHeader.StdId = can_id;
    slcan_tx_info_to_bus_g.can_packets[head].CAN_TxHeader.DLC = can_dlc;
    slcan_tx_info_to_bus_g.can_packets[head].CAN_TxHeader.RTR = CAN_RTR_DATA;
    slcan_tx_info_to_bus_g.can_packets[head].CAN_TxHeader.IDE = CAN_ID_STD;

    slcan_tx_info_to_bus_g.head = (slcan_tx_info_to_bus_g.head + 1u) % SLCAN_TX_FIFO_NUM;
    return HAL_OK;
}

static uint8_t slcan_pc_send_extended_frame_to_bus( uint8_t* cmd_buff)
{
    uint32_t can_id;
    char can_id_str[9];

    char can_dlc_str[2];
    uint32_t can_dlc;

    char can_data_str[3];

    uint8_t head = slcan_tx_info_to_bus_g.head;

    /* CAN ID */
    strncpy(can_id_str, (char *)cmd_buff, 8);
    can_id_str[9] = '\0';
    can_id = strtol(can_id_str, NULL, 16);

    if ( can_id > 0x1FFFFFFFu )
    {
        return HAL_ERROR;
    }
    
    /* CAN DLC */
    can_dlc_str[0] = (char)cmd_buff[8];
    can_dlc_str[1] = '\0';
    can_dlc = strtol(can_dlc_str, NULL, 16);

    if ( can_dlc > 8u )
    {
        return HAL_ERROR;
    }

    if( *( cmd_buff + can_dlc * 2u + 9u ) != '\r')
    {
        return HAL_ERROR;
    }

    /* CAN Data */
    for( uint8_t i = 0u; i < can_dlc; i++ )
    {
        can_data_str[0] = (char)cmd_buff[ 9u + i * 2u ];
        can_data_str[1] = (char)cmd_buff[ 9u + i * 2u + 1u ];
        can_data_str[2] = '\0';
        slcan_tx_info_to_bus_g.can_packets[head].can_data[i] = strtol(can_data_str, NULL, 16);
    }

    slcan_tx_info_to_bus_g.can_packets[head].CAN_TxHeader.ExtId = can_id;
    slcan_tx_info_to_bus_g.can_packets[head].CAN_TxHeader.DLC = can_dlc;
    slcan_tx_info_to_bus_g.can_packets[head].CAN_TxHeader.RTR = CAN_RTR_DATA;
    slcan_tx_info_to_bus_g.can_packets[head].CAN_TxHeader.IDE = CAN_ID_EXT;

    slcan_tx_info_to_bus_g.head = (slcan_tx_info_to_bus_g.head + 1u) % SLCAN_TX_FIFO_NUM;
    return HAL_OK;
}

can_tx_mail_state_t SLCAN_get_tx_mail_state( void )
{
    can_tx_mail_state_t can_tx_mail_state = CAN_TX_BUSY;
    if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0u)
    {
        can_tx_mail_state = CAN_TX_FREE;
    }

    return can_tx_mail_state;
}

uint8_t SLCAN_pc_cmd_process(char cmd, uint8_t* cmd_buff)
{
    uint16_t i;
    uint8_t ret = HAL_ERROR;
    for(i = 0u; i < sizeof(slcan_cmd_table_c)/sizeof(slcan_cmd_table_c[0]);i++)
    {
        if(cmd == slcan_cmd_table_c[i].cmd)
        {
            ret = slcan_cmd_table_c[i].cmd_fun( cmd_buff );
            break;
        }
    }

    return ret;
}

slcan_tx_info_to_bus_t * SLCAN_get_tx_info_to_bus( void )
{
    return &slcan_tx_info_to_bus_g;
}

slcan_tx_info_to_pc_t * SLCAN_get_tx_info_to_pc( void )
{
    return &slcan_tx_info_to_pc_g;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef CAN_RxHeader;
    uint8_t can_data[8];
    uint8_t info_to_pc[30];
    uint8_t head;
    uint8_t data_of_start;
    uint16_t info_len;
    HAL_CAN_GetRxMessage( hcan, CAN_RX_FIFO0, &CAN_RxHeader, can_data );
    uint32_t dlc = CAN_RxHeader.DLC;
    if( CAN_ID_STD == CAN_RxHeader.IDE )
    {
        info_to_pc[0] = 't';
        sprintf((char *)&info_to_pc[1], "%03X", (uint16_t)CAN_RxHeader.StdId);
        sprintf((char *)&info_to_pc[4], "%d", (uint8_t)dlc);
        data_of_start = 5u;
    }
    else
    {
        info_to_pc[0] = 'T';
        sprintf((char *)&info_to_pc[1], "%08X", (uint16_t)CAN_RxHeader.ExtId);
        sprintf((char *)&info_to_pc[9], "%d", (uint8_t)dlc);
        data_of_start = 10u;

    }

    for(uint8_t i = 0u; i < dlc; i++)
    {
        sprintf((char *)&info_to_pc[data_of_start + i*2], "%02X", can_data[i]);
    }
    info_len = dlc * 2 + data_of_start;
    info_to_pc[ info_len ] = '\r';
    info_len++;
    head = slcan_tx_info_to_pc_g.head;
    memcpy(slcan_tx_info_to_pc_g.can_info[head],info_to_pc,info_len);

    slcan_tx_info_to_pc_g.head = (slcan_tx_info_to_pc_g.head + 1u) % SLCAN_TX_FIFO_NUM;

}
