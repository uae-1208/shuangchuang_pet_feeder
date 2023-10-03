#include "include.h"


/*---------函数声明区---------*/
void BSP_Init(void);
void GetSensorData(void);
	
/*---------变量声明区---------*/ 
uint8_t g_CurrentTime[15];		//存放时间数据
uint8_t g_SecTemp = 255,g_MinTemp = 255;
int main(void)
{
	BSP_Init();		
    while(1)
    {
		GetSensorData();		//获取传感器数据
    }		
}

void BSP_Init()
{
	delay_init();	    	 //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	uart_init(9600);		//串口一初始化，波特率9600
	OLED_Init();
	OLED_Clear();
	DS1302_Init();
	DS1302_Write_Time();
	delay_ms(500);		//等待系统稳定
	JQ8x00_Command_Data(SetVolume,30);		//设定音量30
}

/************************************************************************
功能描述：数字组合
入口参数： 无
返 回 值： none
其他说明： *p为换成数组，dat为要组合的数据
**************************************************************************/
uint8_t NumMakeUp(uint8_t *p,uint8_t dat)
{
	uint8_t count = 0;
	
	if(dat == 0)
	{
		*p++ = 99;			//语音：0
		count = 1;
	}
	else if(dat <= 9)		//时
	{
		*p++ = dat;
		count = 1;
	}
	else if(dat <= 19)
	{
		if(dat%10 == 0)			//如果是10的倍数
		{
			*p++ = dat/10;
			*p++ = 10;					//语音:十
		}
		else
		{
			*p++ = 10;					//语音:十
			*p++ = dat%10;		
		}
		count = 2;		
	}
	else if(dat <= 99)
	{
		if(dat%10 == 0)			//如果是10的倍数
		{
			*p++ = dat/10;
			*p++ = 10;					//语音:十
			count = 2;
		}
		else
		{
			*p++ = dat/10;
			*p++ = 10;					//语音:十
			*p++ = dat%10;
			count = 3;		
		}				
	}
	return count;	
}

/************************************************************************
功能描述：播放处理函数
入口参数： 无
返 回 值： none
其他说明： 注意数组buffer大小，别指针溢出
**************************************************************************/
void PlayHandle(void)
{
	uint8_t buffer[20] = {0};
	uint8_t DatLen = 0,Pcount = 0;
	uint8_t *p_temp;

		
		buffer[0] = 11;		//语音:当前时间
		DatLen = 1;
		p_temp = &buffer[1];		//指针始终指向下一位
		
		Pcount = NumMakeUp(p_temp,g_CurrentTime[4]);		//当前时
		p_temp += Pcount;				//地址递增
		DatLen += Pcount;				//数据长度
		*p_temp++ = 12;		//语音:时
		DatLen++;
		
		Pcount = NumMakeUp(p_temp,g_CurrentTime[5]);		//当前分
		p_temp += Pcount;				//地址递增
		DatLen += Pcount;
		*p_temp++ = 13;		//语音:分
		DatLen++;
		
		JQ8x00_ZuHeBoFang(buffer,DatLen);			//送入语音模块播放
		
}

void GetSensorData(void)
{
    DS1302_Get_Time(g_CurrentTime);		//读取时间
    if(g_SecTemp != g_CurrentTime[6])		//秒变了
    {
        g_SecTemp = g_CurrentTime[6];
     		
        //时间显示
        OLED_ShowNum(0,6,g_CurrentTime[0],2,16);		//年
        OLED_ShowNum(16,6,g_CurrentTime[1],2,16);
        OLED_ShowChar(32,6,'/',16);

        OLED_ShowNum(40,6,g_CurrentTime[2],2,16);		//月
        OLED_ShowChar(56,6,'/',16);

        OLED_ShowNum(64,6,g_CurrentTime[3],2,16);		//日
        OLED_ShowChar(80,6,' ',16);

        OLED_ShowNum(88,6,g_CurrentTime[4],2,16);		//时
        OLED_ShowChar(104,6,':',16);

        OLED_ShowNum(112,6,g_CurrentTime[5],2,16);		//分
		
		if(g_MinTemp != g_CurrentTime[5])		//一分钟播报一次
		{
			g_MinTemp = g_CurrentTime[5];
			PlayHandle();
		}
    }
}


