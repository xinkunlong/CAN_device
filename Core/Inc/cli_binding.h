/*
 * 文件名: cli_binding.h
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

#ifndef INC_CLI_BINDING_H_
#define INC_CLI_BINDING_H_

/**
 * Clears the whole terminal.
 */
void onClearCLI(EmbeddedCli *cli, char *args, void *context);

/**
 * Example callback function, that also parses 2 arguments,
 * and has an 'incorrect usage' output.
 */
void get_can_baud_rate(EmbeddedCli *cli, char *args, void *context);

/**
 * Function to bind command bindings you'd like to be added at setup of the CLI.
 */
void initCliBinding();

#endif /* INC_CLI_BINDING_H_ */
