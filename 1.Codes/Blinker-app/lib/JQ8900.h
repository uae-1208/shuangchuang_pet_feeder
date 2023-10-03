#ifndef JQ8900_h
#define JQ8900_h

#include <Arduino.h>


/******************IO定义**************************/
int IO_JQ8X00_BusyCheck = 4;        //忙检测IO

/******************全局变量**************************/
int JQ8X00_BusyCheck = 0;     //忙检测，0表示不做忙检测，1表做忙检测

typedef enum {
    OverallCycle                = 0X00,         /*全盘循环 按顺序播放全盘曲目,播放完后循环播放*/
    SingleCycle                 = 0x01,         /*单曲循环 一直循环播放当前曲目*/
    SingleStop                  = 0x02,         /*单曲停止 播放完当前曲目一次停止*/
    OverallRandom               = 0X03,         /*全盘随机 随机播放盘符内曲目*/
    CatalogCycle                = 0X04,          /*目录循环 按顺序播放当前文件夹内曲目,播放完后循环播放，目录不包含子目录*/
    CatalogRandom               = 0x05,          /*目录随机 在当前目录内随机播放，目录不包含子目录*/
    CatalogTurnPlay             = 0x06,         /*目录顺序播放 按顺序播放当前文件夹内曲目,播放完后停止，目录不包含子目录*/
    OverallTurnPlay             = 0x07,         /*全盘顺序播放 按顺序播放全盘曲目,播放完后停止*/ 
}LoopModeSelect;      //循环模式选择

/**********************串口控制**************************/
typedef enum
{
    CheckPlayState                  = 0x01,         /*查询播报状态*/
    Play                            = 0x02,         /*播放*/
    paused                           = 0x03,         /*暂停*/
    Stop                            = 0x04,         /*停止*/
    LastSong                        = 0x05,         /*上一曲*/
    NextSong                        = 0x06,         /*下一曲*/
    CheckOnelineDisksign            = 0x09,         /*查询当前在线盘符*/
    CheckCurrentDisksign            = 0X0A,         /*查询当前播放盘符*/
    CheckTotalTrack                 = 0x0C,         /*查询总曲目*/
    CurrentTrack                    = 0x0D,         /*当前曲目*/
    LastFloder                      = 0x0E,         /*上一个文件夹目录*/
    NextFloder                      = 0x0F,         /*下一个文件夹目录*/
    EndPlay                         = 0x10,         /*结束播放*/
    CheckFloderFirstTrack           = 0x11,         /*查询文件目录首曲目*/
    CheckFloderAllTrack             = 0x12,         /*查询文件目录总曲目*/
    AddVolume                       = 0x14,         /*音量加*/
    DecVolume                       = 0x15,         /*音量减*/
    EndZHPlay                       = 0X1C,         /*结束组合播报*/
    CheckSongShortName              = 0x1E,         /*查询歌曲短文件名*/
    EndLoop                         = 0x21,         /*结束复读*/
    GetTotalSongTime                = 0x24,         /*获取当前曲目总时间*/
    OpenPlayTime                    = 0x25,         /*播放时间开发送*/
    ClosePlayTime                   = 0x26,         /*关闭播放时间发送*/
} UartCommand;    //无数据的指令,起始码-指令类型-数据长度-校验和

typedef enum
{
    AppointTrack                    = 0x07,                 /*指定曲目播放*/
    SetCycleCount                   = 0x19,                 /*设置循环次数*/
    SetEQ                           = 0X1A,                 /*EQ设置*/
    SelectTrackNoPlay               = 0x19,                 /*选曲不播放*/
    GoToDisksign                    = 0X0B,                 /*切换指定盘符*/
    SetVolume                       = 0x13,                 /*音量设置*/
    SetLoopMode                     = 0x18,                 /*设置循环模式*/
    SetChannel                      = 0x1D,                 /*设置通道*/
    AppointTimeBack                 = 0x22,                 /*指定时间快退*/
    AppointTimeFast                 = 0x23,                 /*指定时间快退*/
}UartCommandData;      //包含多个数据的指令,起始码-指令类型-数据长度-数据1-...-校验和

typedef enum
{
    JQ8X00_USB                      = 0X00,                 /*UPANND*/
    JQ8X00_SD                       = 0x01,                 /*SD*/
    JQ8X00_FLASH                 = 0x02,                    /*FLASH*/
}JQ8X00_Symbol;      //系统盘符


#define u8 unsigned char
/******************函数定义区**************************/
void  JQ8x00_Command(UartCommand Command);
void  JQ8x00_Command_Data(UartCommandData Command, u8 DATA);
void JQ8x00_AppointPlay(JQ8X00_Symbol MODE, char *DATA);
void  JQ8x00_ZuHeBoFang(unsigned char *DATA, unsigned char Len);



#endif 

