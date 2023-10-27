/*
 * 文件名: cdc.c
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

#include "cdc.h"

extern CAN_HandleTypeDef hcan1;

static void cdc_pc_info_to_can_bus( void );
static void cdc_can_info_to_pc( void );

static void cdc_pc_info_to_can_bus( void )
{
    uint32_t tx_mailbox;
    can_tx_mail_state_t  tx_mail_state;
    tx_mail_state = SLCAN_get_tx_mail_state();
    slcan_tx_info_to_bus_t *slcan_tx_info_to_bus_p;
    slcan_tx_info_to_bus_p = SLCAN_get_tx_info_to_bus();
    uint8_t tail = slcan_tx_info_to_bus_p->tail;
    CAN_TxHeaderTypeDef CAN_TxHeader = slcan_tx_info_to_bus_p->can_packets[tail].CAN_TxHeader;

    if( tail != slcan_tx_info_to_bus_p->head )
    {
        if( CAN_TX_FREE == tx_mail_state )
        {
            (void)HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeader, slcan_tx_info_to_bus_p->can_packets[tail].can_data, &tx_mailbox);
            slcan_tx_info_to_bus_p->tail = (slcan_tx_info_to_bus_p->tail + 1u) % SLCAN_TX_FIFO_NUM;
        }
    }
}


static void cdc_can_info_to_pc( void )
{
    slcan_tx_info_to_pc_t * slcan_tx_info_to_pc_p;
    slcan_tx_info_to_pc_p = SLCAN_get_tx_info_to_pc();
    uint8_t tail = slcan_tx_info_to_pc_p->tail;
    if( tail != slcan_tx_info_to_pc_p->head )
    {
        (void)CDC_Transmit_FS(slcan_tx_info_to_pc_p->can_info[tail],sizeof(slcan_tx_info_to_pc_p->can_info[tail]));
        slcan_tx_info_to_pc_p->tail = (slcan_tx_info_to_pc_p->tail + 1u) % SLCAN_TX_FIFO_NUM;
    }
}
/*
 *
 *        PC <----------> slcan<----------> CAN bus
 *
 * */

/* PC <----------> slcan */
void CDC_process_pc_slcan(void)
{
    char cmd;
    uint32_t len;
    if( usb_rx_info_g.head != usb_rx_info_g.tail )
    {
        cmd = usb_rx_info_g.rx_buff[usb_rx_info_g.tail][0];
        len = usb_rx_info_g.rx_buff_len[usb_rx_info_g.tail];

        if(len > 1u)
        {
            SLCAN_pc_cmd_process(cmd, &usb_rx_info_g.rx_buff[usb_rx_info_g.tail][1]);
        }

        usb_rx_info_g.tail = (usb_rx_info_g.tail + 1u) % NUM_RX_BUFS;
    }
}

/* slcan<----------> CAN bus */
void CDC_process_slcan_bus( void )
{
    cdc_pc_info_to_can_bus();
    cdc_can_info_to_pc();
}


