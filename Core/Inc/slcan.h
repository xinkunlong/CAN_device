/*
 * 文件名: slcan.h
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

#ifndef INC_SLCAN_H_
#define INC_SLCAN_H_

#include "stm32f7xx_hal.h"
#include "usbd_cdc_if.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define SLCAN_TX_FIFO_NUM 10u

typedef uint8_t (*cmd_fun_p)( uint8_t *cmd_buff );

typedef struct
{
    char cmd;
    cmd_fun_p cmd_fun;
}slcan_cmd_table_t;

typedef struct
{
    char cmd;
    uint32_t prescaler;
}slcan_baud_rate_map_t;



typedef struct
{
    CAN_TxHeaderTypeDef CAN_TxHeader;
    uint8_t can_data[8];
}slcan_byte_packets_t;

typedef struct
{
    uint8_t head;
    uint8_t tail;
    slcan_byte_packets_t can_packets[SLCAN_TX_FIFO_NUM];
}slcan_tx_info_to_bus_t;

typedef struct
{
    uint8_t slcan_str[27];
    uint32_t str_len;
}slcan_str_info_packets_t;

typedef struct
{
    uint8_t head;
    uint8_t tail;
    slcan_str_info_packets_t info_pkg[SLCAN_TX_FIFO_NUM];/* DATA MAX:8 * 2,  CANID MAX : 8, CAN DLC :1, '\r'*/
}slcan_tx_info_to_pc_t;

typedef enum
{
    CAN_TX_FREE,
    CAN_TX_BUSY
}can_tx_mail_state_t;

can_tx_mail_state_t SLCAN_get_tx_mail_state( void );
uint8_t SLCAN_pc_cmd_process(char cmd, uint8_t* cmd_buff);
slcan_tx_info_to_bus_t * SLCAN_get_tx_info_to_bus( void );
slcan_tx_info_to_pc_t * SLCAN_get_tx_info_to_pc( void );
#endif /* INC_SLCAN_H_ */
