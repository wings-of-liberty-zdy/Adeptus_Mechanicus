#include "ax_ps2.h"
#include "main.h"

// PS2手柄的输入输出端口定义
#define DI_PORT     GPIOA
#define DI_PIN      GPIO_PIN_4

#define CMD_PORT    GPIOA
#define CMD_PIN     GPIO_PIN_5

#define CS_PORT     GPIOA
#define CS_PIN      GPIO_PIN_6

#define CLK_PORT    GPIOA
#define CLK_PIN     GPIO_PIN_7

// 宏定义简化IO操作
#define DI()        HAL_GPIO_ReadPin(DI_PORT, DI_PIN)
#define CMD_H()     HAL_GPIO_WritePin(CMD_PORT, CMD_PIN, GPIO_PIN_SET)
#define CMD_L()     HAL_GPIO_WritePin(CMD_PORT, CMD_PIN, GPIO_PIN_RESET)
#define CS_H()      HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET)
#define CS_L()      HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET)
#define CLK_H()     HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_SET)
#define CLK_L()     HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_RESET)

const uint8_t PS2_cmnd[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 请求获取数据命令
static uint8_t PS2_data[9] = {0};  // 接收的数据

/**
  * @brief  PS2初始化
  * @param  无
  * @retval 无
  */
void AX_PS2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // DATA引脚配置 - 输入上拉
    GPIO_InitStruct.Pin = DI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DI_PORT, &GPIO_InitStruct);

    // COMMAND引脚配置 - 推挽输出
    GPIO_InitStruct.Pin = CMD_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CMD_PORT, &GPIO_InitStruct);

    // CS引脚配置 - 推挽输出
    GPIO_InitStruct.Pin = CS_PIN;
    HAL_GPIO_Init(CS_PORT, &GPIO_InitStruct);

    // CLK引脚配置 - 推挽输出
    GPIO_InitStruct.Pin = CLK_PIN;
    HAL_GPIO_Init(CLK_PORT, &GPIO_InitStruct);

    // 关闭PS2手柄使能
    CS_H();
}

/**
  * @brief  PS2数据读写函数
  * @param  cmd: 要写入的命令
  * @retval 读出的数据
  */
static uint8_t PS2_ReadWriteData(uint8_t cmd)
{
    uint8_t res = 0;
    uint8_t ref;

    // 写入命令，并读取一个1字节数据
    for(ref = 0x01; ref > 0x00; ref <<= 1)
    {
        // 输出一位数据
        if(ref & cmd)
            CMD_H();
        else
            CMD_L();

        CLK_L();
//        HAL_Delay(1); // 微秒级延时可能需要使用定时器实现，此处是使用了毫秒级延时
        delay_us(16);

        // 读取一位数据
        if(DI())
            res |= ref;

        CLK_H();
//        HAL_Delay(1); // 微秒级延时可能需要使用定时器实现，此处是使用了毫秒级延时
        delay_us(16);
    }

    return res;
}

/**
  * @brief  PS2获取按键及摇杆数值
  * @param  *JoystickStruct 手柄键值结构体
  * @retval 无
  */
void AX_PS2_ScanKey(JOYSTICK_TypeDef *JoystickStruct)
{
    uint8_t i;

    // 使能手柄
    CS_L();

    // 读取PS2数据
    for(i = 0; i < 9; i++)
    {
        PS2_data[i] = PS2_ReadWriteData(PS2_cmnd[i]);
    }

    // 关闭使能
    CS_H();

    // 数值传递
    JoystickStruct->mode = PS2_data[1];
    JoystickStruct->btn1 = ~PS2_data[3];
    JoystickStruct->btn2 = ~PS2_data[4];
    JoystickStruct->RJoy_LR = PS2_data[5];
    JoystickStruct->RJoy_UD = PS2_data[6];
    JoystickStruct->LJoy_LR = PS2_data[7];
    JoystickStruct->LJoy_UD = PS2_data[8];
}

/**
  * @brief  自定义微秒级延时函数
  * @uint32_t  udelay: 要延时的微秒数
  * @retval 无
  */

void delay_us(uint32_t udelay)    //定义hal库us级延迟
{
  uint32_t startval,tickn,delays,wait;

  startval = SysTick->VAL;
  tickn = HAL_GetTick();
  //sysc = 72000;  //SystemCoreClock / (1000U / uwTickFreq);
  delays =udelay * 72; //sysc / 1000 * udelay;
  if(delays > startval)
    {
      while(HAL_GetTick() == tickn)
        {

        }
      wait = 72000000 + startval - delays;
      while(wait < SysTick->VAL)
        {

        }
    }
  else
    {
      wait = startval - delays;
      while(wait < SysTick->VAL && HAL_GetTick() == tickn)
        {

        }
    }
}
