#include <Arduino.h>
#include "../lib/JQ8900.h"


/************************************************************************
  功能描述：起始码-指令类型-数据长度-校验和
  入口参数：   MODE：模式
  返 回 值： none
  其他说明： 不带数据的指令播放函数.无数数据传入
**************************************************************************/
void  JQ8x00_Command(UartCommand Command)
{
    u8 Buffer[4] = {0xaa};

    Buffer[1] = Command;            //指令类型
    Buffer[2] = 0x00;           //数据长度
    Buffer[3] = Buffer[0] +  Buffer[1] +  Buffer[2];            //校验和

    if (JQ8X00_BusyCheck)
    {
        delay(10);
        while (digitalRead(IO_JQ8X00_BusyCheck) == HIGH);     //忙检测
    }
    Serial.write(Buffer, 4);
}

/************************************************************************
  功能描述：起始码-指令类型-数据长度-数据-校验和
  入口参数：   *DAT：字符串指针,Len字符串长度
  返 回 值： none
  其他说明：
**************************************************************************/
void  JQ8x00_Command_Data(UartCommandData Command, u8 DATA)
{
    u8 Buffer[6] = {0xaa};
    u8 DataLen = 0;         //数据长度
    Buffer[1] = Command;       //指令类型
    if((Command != AppointTrack) && (Command != SetCycleCount) && (Command != SelectTrackNoPlay) && (Command != AppointTimeBack) && (Command != AppointTimeFast))        //只含一个数据指令
    {
        Buffer[2] = 1;      //数据长度
        Buffer[3] = DATA;       //数据
        Buffer[4] = Buffer[0] +  Buffer[1] +  Buffer[2] + Buffer[3];            //校验和
        DataLen = 5;
    }
    else                                                                                        //含两个数据指令
    {
        Buffer[2] = 2;      //数据长度
        Buffer[3] = DATA/256;       //数据
        Buffer[4] = DATA%256;       //数据
        Buffer[5] = Buffer[0] +  Buffer[1] +  Buffer[2] + Buffer[3] + Buffer[4];
        DataLen = 6;
    }

    if (JQ8X00_BusyCheck)
    {
        delay(10);
        while (digitalRead(IO_JQ8X00_BusyCheck) == HIGH);     //忙检测
    }
    Serial.write(Buffer, DataLen);
}

/************************************************************************
  功能描述：播放根目录指定文件名的音频文件
  入口参数：JQ8X00_Symbol:系统盘符，*DATA:需要播放的文件名
  返 回 值： none
  其他说明：播放FLASH根目录下文件名为00001.mp3的音频，JQ_8x00_AppointPlay(JQ8X00_FLASH,"00001");
**************************************************************************/
void JQ8x00_AppointPlay(JQ8X00_Symbol MODE, char *DATA)
{
    u8 Buffer[30] = {0xaa, 0x08};
    u8 i, j;
    Buffer[2] = 1 + strlen(DATA) + 5;       //长度
    Buffer[3] = MODE;        //盘符
    Buffer[4] = '/';        //路径
    i = 5;
    while (*DATA)
    {
        Buffer[i++] =  *DATA;
        DATA++;
    }
    Buffer[i++] = '*';
    Buffer[i++] = '?';
    Buffer[i++] = '?';
    Buffer[i++] = '?';
    for (j = 0; j < i; j++)
    {
        Buffer[i] = Buffer[i] + Buffer[j];      //计算校验合
    }
    i++;

    if (JQ8X00_BusyCheck)
    {
        delay(10);
        while (digitalRead(IO_JQ8X00_BusyCheck) == HIGH);     //忙检测
    }

    Serial.write(Buffer, i);
}

/************************************************************************
  功能描述：组合播报函数
  入口参数：   *DAT：字符串指针,Len字符串长度
  返 回 值： none
  其他说明： 将需要播报的文件名放入数组中作为形参即可,例：组合播放01,02文件，则DATA内容为{1,2}，JQ8x00_ZuHeBoFang（DATA，2）
**************************************************************************/
void  JQ8x00_ZuHeBoFang(unsigned char *DATA, unsigned char Len)
{
    unsigned char CRC_data = 0, i = 3;
    unsigned char Buffer[30] = {0xaa, 0x1b};

    Buffer[2] = Len * 2;    //计算长度
    CRC_data = CRC_data + 0xaa + 0x1b + Buffer[2];
    while (Len--)
    {
        Buffer[i] = *DATA / 10 + 0x30;  //取出十位
        CRC_data = CRC_data + Buffer[i];
        i++;
        Buffer[i] = *DATA % 10 + 0x30;  //取出个位
        CRC_data = CRC_data + Buffer[i];
        i++;
        DATA++;
    }
    Buffer[i] = CRC_data;
    i++;
    if (JQ8X00_BusyCheck)
    {
        delay(10);
        while (digitalRead(IO_JQ8X00_BusyCheck) == HIGH);     //忙检测
    }

    Serial.write(Buffer, i);
}

#define BUILTIN_LED 2

void setup()
{
    Serial.begin(9600);                                        
    pinMode(BUILTIN_LED, OUTPUT);     // 设置板上LED引脚为输出模式
    digitalWrite(BUILTIN_LED, HIGH);  // 启动后开启板上LED

    delay(10);                                              
    JQ8x00_Command_Data(SetVolume, 20);     //设置音量为30
    JQ8x00_Command_Data(AppointTrack,2);    //播放指定曲目2
}

void loop()
{
    delay(3000);                                              
    digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));  
    //JQ8x00_AppointPlay(JQ8X00_FLASH, "00002");     //播放FLASH下，文件名为00002的音频文件
    JQ8x00_Command_Data(AppointTrack,2);    //播放指定曲目2
}


