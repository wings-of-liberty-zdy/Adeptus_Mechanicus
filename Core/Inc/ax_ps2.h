//
// Created by houdezaiwu on 2025/10/23.
//

#ifndef RESURGENCE_AX_PS2_H
#define RESURGENCE_AX_PS2_H
#ifndef __AX_PS2_H
#define __AX_PS2_H

#include "stm32f1xx_hal.h"

// 手柄键值结构体
typedef struct {
    uint8_t mode;       // 手柄工作模式
    uint8_t btn1;       // 按钮组1 (SLCT, JOYR, JOYL, STRT, UP, RIGHT, DOWN, LEFT)
    uint8_t btn2;       // 按钮组2 (L2, R2, L1, R1, Y, B, A, X)
    uint8_t RJoy_LR;    // 右边摇杆左右 0x00=左 0xff=右
    uint8_t RJoy_UD;    // 右边摇杆上下 0x00=上 0xff=下
    uint8_t LJoy_LR;    // 左边摇杆左右 0x00=左 0xff=右
    uint8_t LJoy_UD;    // 左边摇杆上下 0x00=上 0xff=下
} JOYSTICK_TypeDef;

void AX_PS2_Init(void);
void AX_PS2_ScanKey(JOYSTICK_TypeDef *JoystickStruct);
void delay_us(uint32_t udelay);

#endif
#endif //RESURGENCE_AX_PS2_H