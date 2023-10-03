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
    pause                           = 0x03,         /*暂停*/
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


/******************函数定义区**************************/

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


void setup()
{
    if (JQ8X00_BusyCheck)
        pinMode(IO_JQ8X00_BusyCheck, INPUT_PULLUP);                         //忙检测引脚初始化

    Serial.begin(9600);                                         //波特率9600

    delay(10);                                                //延时10ms
    JQ8x00_Command_Data(SetVolume, 30);                      //设置音量为30
    delay(10);                                              //延时10ms
   JQ8x00_Command_Data(AppointTrack,2);                    //播放指定曲目2
//    JQ8x00_AppointPlay(JQ8X00_FLASH, "00002");               //播放FLASH下，文件名为00002的音频文件
}

void loop()
{

}
