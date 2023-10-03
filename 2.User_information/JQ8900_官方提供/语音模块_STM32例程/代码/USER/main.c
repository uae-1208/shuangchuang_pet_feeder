#include "include.h"


/*---------����������---------*/
void BSP_Init(void);
void GetSensorData(void);
	
/*---------����������---------*/ 
uint8_t g_CurrentTime[15];		//���ʱ������
uint8_t g_SecTemp = 255,g_MinTemp = 255;
int main(void)
{
	BSP_Init();		
    while(1)
    {
		GetSensorData();		//��ȡ����������
    }		
}

void BSP_Init()
{
	delay_init();	    	 //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	uart_init(9600);		//����һ��ʼ����������9600
	OLED_Init();
	OLED_Clear();
	DS1302_Init();
	DS1302_Write_Time();
	delay_ms(500);		//�ȴ�ϵͳ�ȶ�
	JQ8x00_Command_Data(SetVolume,30);		//�趨����30
}

/************************************************************************
�����������������
��ڲ����� ��
�� �� ֵ�� none
����˵���� *pΪ�������飬datΪҪ��ϵ�����
**************************************************************************/
uint8_t NumMakeUp(uint8_t *p,uint8_t dat)
{
	uint8_t count = 0;
	
	if(dat == 0)
	{
		*p++ = 99;			//������0
		count = 1;
	}
	else if(dat <= 9)		//ʱ
	{
		*p++ = dat;
		count = 1;
	}
	else if(dat <= 19)
	{
		if(dat%10 == 0)			//�����10�ı���
		{
			*p++ = dat/10;
			*p++ = 10;					//����:ʮ
		}
		else
		{
			*p++ = 10;					//����:ʮ
			*p++ = dat%10;		
		}
		count = 2;		
	}
	else if(dat <= 99)
	{
		if(dat%10 == 0)			//�����10�ı���
		{
			*p++ = dat/10;
			*p++ = 10;					//����:ʮ
			count = 2;
		}
		else
		{
			*p++ = dat/10;
			*p++ = 10;					//����:ʮ
			*p++ = dat%10;
			count = 3;		
		}				
	}
	return count;	
}

/************************************************************************
�������������Ŵ�����
��ڲ����� ��
�� �� ֵ�� none
����˵���� ע������buffer��С����ָ�����
**************************************************************************/
void PlayHandle(void)
{
	uint8_t buffer[20] = {0};
	uint8_t DatLen = 0,Pcount = 0;
	uint8_t *p_temp;

		
		buffer[0] = 11;		//����:��ǰʱ��
		DatLen = 1;
		p_temp = &buffer[1];		//ָ��ʼ��ָ����һλ
		
		Pcount = NumMakeUp(p_temp,g_CurrentTime[4]);		//��ǰʱ
		p_temp += Pcount;				//��ַ����
		DatLen += Pcount;				//���ݳ���
		*p_temp++ = 12;		//����:ʱ
		DatLen++;
		
		Pcount = NumMakeUp(p_temp,g_CurrentTime[5]);		//��ǰ��
		p_temp += Pcount;				//��ַ����
		DatLen += Pcount;
		*p_temp++ = 13;		//����:��
		DatLen++;
		
		JQ8x00_ZuHeBoFang(buffer,DatLen);			//��������ģ�鲥��
		
}

void GetSensorData(void)
{
    DS1302_Get_Time(g_CurrentTime);		//��ȡʱ��
    if(g_SecTemp != g_CurrentTime[6])		//�����
    {
        g_SecTemp = g_CurrentTime[6];
     		
        //ʱ����ʾ
        OLED_ShowNum(0,6,g_CurrentTime[0],2,16);		//��
        OLED_ShowNum(16,6,g_CurrentTime[1],2,16);
        OLED_ShowChar(32,6,'/',16);

        OLED_ShowNum(40,6,g_CurrentTime[2],2,16);		//��
        OLED_ShowChar(56,6,'/',16);

        OLED_ShowNum(64,6,g_CurrentTime[3],2,16);		//��
        OLED_ShowChar(80,6,' ',16);

        OLED_ShowNum(88,6,g_CurrentTime[4],2,16);		//ʱ
        OLED_ShowChar(104,6,':',16);

        OLED_ShowNum(112,6,g_CurrentTime[5],2,16);		//��
		
		if(g_MinTemp != g_CurrentTime[5])		//һ���Ӳ���һ��
		{
			g_MinTemp = g_CurrentTime[5];
			PlayHandle();
		}
    }
}


