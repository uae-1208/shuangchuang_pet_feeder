#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "JQ8X00.h"

/*****************************************************************************
*版权信息：深圳市佳仁科技有限公司
*当前版本：V3.0
*MCU 型号：STM32F1全系
*开发环境：Keil uVision5
*完成日期：2019-10-18
*作    者:JW
*程序功能:JQ8X00系列例程
*其他说明：1:JQ8X00系列语音芯片的驱动全部包含于JQ8X00.c文件中，如需对文件IO做更改或者移植至
            自己的工程文件，只需修改JQ8X00.h文件中的定义。JQ8X00.c内调用函数全部在JQ8X00.h中替换
          2：连续发送指令时，请加入一定时间的延时间隔发送
          3：指定曲目播放，系统按照文件放入先后顺讯，依次编号曲目1 2 3 ...不按照文件名00001 000002...
********************************************************************************/

int main(void)
{
    char Path_Buffer[] = {"/测试1*/测试2*/00002"};         //播放路径： 根目录->测试1->测试2 下文件名为00002的音频文件
//    uint8_t ZH_Buffer[3] = {1,2,3};     
    delay_init();	    	 //延时函数初始化
    JQ8x00_Init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    uart_init(9600);	 //串口初始化为9600
    delay_ms(500);              //等待模块稳定

    JQ8x00_Command_Data(SetVolume,30);         //设置音量
    delay_ms(10);                               //连续发送指令，加入一定的延时等待模块处理完成上一条指令
//    JQ8x00_Command_Data(AppointTrack,2);      //播放曲目2
//    JQ8x00_ZuHeBoFang(ZH_Buffer,3);             //将ZH文件夹下名字为01 02 03的3个音频文件依次播放
//    delay_ms(10);
    JQ8x00_RandomPathPlay(JQ8X00_FLASH,Path_Buffer);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件

    while(1)
    {

    }
}

