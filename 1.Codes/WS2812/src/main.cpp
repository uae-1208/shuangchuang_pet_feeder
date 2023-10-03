#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>


#define WS_PIN  13	//定义选择引脚
#define WS_NUM  1	//定义led个数，决定数组长度
Adafruit_NeoPixel pixels(WS_NUM, WS_PIN, NEO_GRB + NEO_KHZ800);//第一个参数led个数，第二个引脚号，第三个rgb模式，第四个800khz-ws2812


void setup() 
{
	Serial.begin(115200);       //Debug串口
	pixels.begin();//初始化灯带
	pixels.setBrightness(50);

}

void loop() 
{
	static int station = 0;
	static int light = 100;	
	static int sign = 1;	
	

	if(0<=station && station<100)   //[0,100,0]->[100,0,0]   绿色到红色
	{
		if(light == 0)
			sign = -1;
		if(light == 100)
			sign = 1;
			
		light -= sign;
		pixels.setPixelColor(0,pixels.Color(100-light, light, 0));//注意是从0开始，第一个led对应0 
	}
	else if(150<=station && station<250)  //[100,0,0]->[0,0,100]   红色到蓝色
	{
		if(light == 0)
			sign = -1;
		if(light == 100)
			sign = 1;
			
		light -= sign;
		pixels.setPixelColor(0,pixels.Color(100-light, 0, light));
	}
	else if(300<=station && station<400)  //[0,0,100]->[0,100,0]   蓝色到绿色
	{
		if(light == 0)
			sign = -1;
		if(light == 100)
			sign = 1;
			
		light -= sign;
		pixels.setPixelColor(0,pixels.Color(0, 100-light, light));
	}

	if(station >= 449)
	{
		station = 0;
		light = 100;
	}  
	else
		station++;

	pixels.show();//刷新
	delay(10);
}