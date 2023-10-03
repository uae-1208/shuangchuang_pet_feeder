#define BLINKER_WIFI
#include <Blinker.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include "../lib/JQ8900.h"


#define PWM1_GPIO  26
#define PWM2_GPIO  27
#define PWM1_Chan  14
#define PWM2_Chan  15

#define flag  char
flag feed_lock = 0;     //按下喂食键后置1，此时再按无反应，直至当前喂食结束后复位

#define WS_PIN  13	//定义选择引脚
#define WS_NUM  1	//定义led个数，决定数组长度
Adafruit_NeoPixel pixels(WS_NUM, WS_PIN, NEO_GRB + NEO_KHZ800);//第一个参数led个数，第二个引脚号，第三个rgb模式，第四个800khz-ws2812



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
    Serial2.write(Buffer, DataLen);
}



char auth[] = "81f51910e9b2";   //双创摄像头
char ssid[] = "先贤显现";
char pswd[] = "a1208172292";


int weight = 3047;          //总重3kg
int FoodQuantity = 0;       //喂食份/每份10g
int Voice_num = 11;         //语音播报序号
int volume = 20;            //扬声器音量,0-30
//语言播报内容查询表    
String VoiceText_Lib[11] = { 
                                "快来吃饭啦!",                  //0
                                "不要调皮!",                    //1
                                "我要出去一下",                 //2
                                "我马上就回家!",                //3
                                "饿了没？快过来吃饭！",          //4
                                "过来过来！让我看看你在干嘛。",   //5
                                "暂无语音播报",                 //6
                                "暂无语音播报",                 //7
                                "暂无语音播报",                 //8
                                "暂无语音播报",                 //9
                                "暂无语音播报"                  //10
};

BlinkerNumber NumFood("num-food");
BlinkerNumber NumFoodLeft("num-food-left");
BlinkerText   TextVoice("tex-voice");



/****************************喂食量设置滑块********************************/
BlinkerSlider RanFood("ran-food");
void slider_food_callback(int32_t value)    //滑块回调函数
{
    if(feed_lock == 0)   //未上锁时，移动滑块才有效
    {
        FoodQuantity = value;
        NumFood.print(10*FoodQuantity);   //显示计划喂食量(g)
    }
    
    BLINKER_LOG("get slider value: ", value);
}

/********************************喂食按键********************************/
BlinkerButton BtnFood("btn-food");
void button_food_callback(const String & state)    //喂食按键回调函数
{
    BLINKER_LOG("get button state: ", state);

    if(feed_lock == 0)  //未上锁时
    {
        feed_lock = 1;     //上锁
        if(FoodQuantity)   //喂食份数不为0
        {
            ledcWrite(PWM1_Chan, 1000);   //喂食电机开始转动
            pixels.setPixelColor(0,pixels.Color(100, 0, 0));//注意是从0开始，第一个led对应0 
            pixels.show();//刷新
           //BtnFood.color("#FF0000");     //喂食按键图标改成红色
           //BtnFood.text("喂食中");       //喂食按键文字显示“喂食中” 
        }
    }
}



/*****************************音量调节滑块********************************/
BlinkerSlider RanVolume("ran-volume");
void slider_volume_callback(int32_t value)    //滑块回调函数
{
    volume = value;
    JQ8x00_Command_Data(SetVolume, volume);     //设置扬声器音量
    BLINKER_LOG("get slider value: ", value);
}


/***************************语音播报选择滑块******************************/
BlinkerSlider RanMusic("ran-music");
void slider_music_callback(int32_t value)    //滑块回调函数
{
    Voice_num = value;
    BLINKER_LOG("get slider value: ", value);
}




/******************************曲目播报按键******************************/
BlinkerButton BtnMusic("btn-music");
void button_music_callback(const String & state)    //喂食按键回调函数
{
    BLINKER_LOG("get button state: ", state);
    if(1<=Voice_num && Voice_num<=6)
    {
        JQ8x00_Command_Data(AppointTrack,Voice_num);    //播放指定曲目
        Blinker.delay(10);
    }
    TextVoice.print("语音播报词",VoiceText_Lib[Voice_num-1]);  //语音播报词

}



/**********************************输入框*********************************/
/* void dataRead(const String & data)
{  
    char num[2];        //序号只能为0-9
    memcpy(num,data.c_str(),1);

    BLINKER_LOG("input=",data);

    if('0' <= num[0] && num[0] <= '9')
    {
        Voice_num = num[0] - '0';
    }

    TextVoice.print("语音播报词",VoiceText_Lib[Voice_num]);
} */




//在blinker app上，点击设备图标进入设备控制页面时，app会向设备发送一个状态查询指令（心跳包）。此后，WiFi设备每59秒会发送一次心跳包；
void heartbeat()    // 心跳包函数
{
    NumFood.print(10*FoodQuantity);       //计划喂食量
    NumFoodLeft.print(weight);
    RanFood.print(FoodQuantity);          //喂食亮滑块
    RanVolume.print(volume);              //音量滑块
    TextVoice.print("语音播报词",VoiceText_Lib[Voice_num-1]);  //语音播报词
    
    BtnFood.color("#00ff00");           //喂食按键图标改成蓝色
    BtnFood.text("abccssdas");               //喂食按键显示“喂食”
}



void dataStorage()
{
    int data = (random(0,3)-1)*10 + 50;
    Blinker.dataStorage("food_num", data);
    Serial.println(data);
}




void setup()
{
    Serial.begin(115200);       //Debug串口
    Serial2.begin(9600);        //扬声器串口

    
    pixels.begin();//初始化灯带
    pixels.setBrightness(50);
    pixels.setPixelColor(0,pixels.Color(0, 0, 100));//注意是从0开始，第一个led对应0 
    pixels.show();//刷新
    

    ledcSetup(PWM1_Chan, 10000, 10);        //PWM通道14，频率10kHz，分辨率为10位=1024           
    ledcAttachPin(PWM1_GPIO, PWM1_Chan);                    
    ledcSetup(PWM2_Chan, 10000, 10);        //PWM通道15，频率10kHz，分辨率为10位=1024                   
    ledcAttachPin(PWM2_GPIO, PWM2_Chan);                  
    ledcWrite(PWM1_Chan, 0);
    ledcWrite(PWM2_Chan, 0);


    JQ8x00_Command_Data(SetVolume, 20);     //设置扬声器音量为20
    delay(10); 


    BLINKER_DEBUG.stream(Serial);
    Blinker.begin(auth, ssid, pswd);         //初始化blinker
    Blinker.attachHeartbeat(heartbeat);      //注册心跳包，用于初始化UI
    Blinker.attachDataStorage(dataStorage);  //上传喂食量数据

    BtnFood.attach(button_food_callback);    //为喂食按键注册回调函数
    BtnMusic.attach(button_music_callback);  //为曲目播报按键注册回调函数
    RanFood.attach(slider_food_callback);    //为喂食量滑块注册回调函数
    RanVolume.attach(slider_volume_callback);//为音量滑块注册回调函数
    RanMusic.attach(slider_music_callback);  //为播报选择滑块注册回调函数
}


void loop() 
{
    static int dataStorage_time = 0;    //数据存储计时时间
    static int feeding_time = 0;        //喂食计时时间
    
    Blinker.run();

    if(dataStorage_time >= 500)
    {
        dataStorage_time = 0;

        int data = (random(0,3)-1)*10 + 50;
        Blinker.dataStorage("food_num", data);
        Serial.println(data);
    }

    if(feed_lock)  //上锁时
    {
        if(FoodQuantity)                 //当未达到计划喂食份数时
        {
            feeding_time++;

            if(feeding_time >= 15)       //喂食1s后
            {
                FoodQuantity--;          //喂食份数减1
                feeding_time = 0;        //喂食计时时间清零
                weight -= 10;            //减去10g
                NumFoodLeft.print(weight);
            }
        }
        else//喂食结束
        {
            feed_lock = 0;                      //解锁
            FoodQuantity = 0;           
            ledcWrite(PWM1_Chan, 0);            //喂食电机停止转动

            //BtnFood.color("#0000FF");           //喂食按键图标改成蓝色
           // BtnFood.text("喂食");               //喂食按键显示“喂食”
            NumFood.print(10*FoodQuantity);     //计划喂食量清零
            RanFood.print(FoodQuantity);        //喂食量滑块归0
            pixels.setPixelColor(0,pixels.Color(0, 0, 100));//注意是从0开始，第一个led对应0 
            pixels.show();//刷新
        }


        //Serial.println("喂食中");  用来计时
    }

    dataStorage_time++;
    Blinker.delay(10);
} 