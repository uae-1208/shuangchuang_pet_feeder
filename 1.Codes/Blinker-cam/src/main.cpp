#define BLINKER_WIFI

#include <Blinker.h>
#include "../lib/ESP32_CAM_SERVER.h"

char auth[] = "c5dfb70987fa";
char ssid[] = "先贤显现";
char pswd[] = "a1208172292";
bool setup_camera = false;

BlinkerButton Button1("btn-cnt");
BlinkerText   TextVideo("tex-video");


void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);
}



void button1_callback(const String & state)
{
    BLINKER_LOG("get button state: ", state);
    Blinker.printObject("video", "{\"str\":\"mjpg\",\"url\":\"http://"+ WiFi.localIP().toString() + "\"}");
    
}



void setup()
{
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    Blinker.begin(auth, ssid, pswd);    // 初始化blinker
    Button1.attach(button1_callback);
    Blinker.attachData(dataRead);

}

void loop()
{
    Blinker.run();

    if (Blinker.connected() && !setup_camera)
    {
        setupCamera();
        setup_camera = true;

        Blinker.printObject("video", "{\"str\":\"mjpg\",\"url\":\"http://"+ WiFi.localIP().toString() + "\"}");
        TextVideo.print("视频IP地址",WiFi.localIP().toString());  
    }
}
