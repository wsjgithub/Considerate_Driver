#include "math.h";
// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"

// This #include statement was automatically added by the Particle IDE.
#include "TinyGPS++/TinyGPS++.h"
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;
//test location43.101203, -77.625767
static const double T_LAT = 43.101267, T_LON = -77.625872;

#define PIXEL_PIN D2
#define PIXEL_COUNT 1
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
String destination = "unavailable";
String location = "unavailable";
unsigned long distance = 0;

void setup() {
     strip.begin();
     strip.setPixelColor(0,strip.Color(0,0,250));
    strip.show();
    Serial.begin(9600);
    Serial1.begin(GPSBaud);
    Serial.begin(9600);
    Serial.println("testing");
    Particle.subscribe("hook-response/closerFurther",getData,MY_DEVICES);

}
int count = 0;

void loop() {
    //*****************************************
    double test1 = testDistance(43.101267,-77.625872,43.101267,-77.625872);
    Serial.println("MOTHER FUCKER TESTING FUCK YOU PHOTON"+String(test1));
    //     double test2 = testDistance(43.101267,-77.625872,43.101297,-77.625072);
    // Serial.println("MOTHER FUCKER TESTING FUCK YOU PHOTON"+String(test2));
    //     double test3 = testDistance(43.101267,-77.625872,43.101297,77.625072);
    // Serial.println("MOTHER FUCKER TESTING FUCK YOU PHOTON"+String(test3));
      //*****************************************
     count++;
    if(count==9){
    Particle.publish("closerFurther");

    }else if(count>16){
        count = 0;
    }
    location = getLocation();
    Serial.println("current location");
    Serial.println(location);

    // //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    // if(!destination.equals("unavailable")){
    //     int comma = destination.indexOf(",");

    //           String latString =  destination.substring(0,comma);
    //     String lonString =  destination.substring(comma+1);
    //     Serial.println(String(latString.toFloat())+" "+String(lonString.toFloat()));
    // }
    // //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    if(destination.equals("unavailable")||!gps.location.isValid()){
        Serial.println("no gps");
        strip.setPixelColor(0,strip.Color(0,0,250));
    }else{
        int comma = destination.indexOf(",");
        String latString =  destination.substring(0,comma);
        String lonString =  destination.substring(comma+1);
        // Serial.println(String(latString.toFloat()),String(lonString.toFloat()));
        // char[]* latChar;
        // char[]* lonChar;
        // latString.toCharArray(latChar,latString.length());
        // lonString.toCharArray(lonChar,lonString.length());
        double newDistance =  getDistance(latString.toFloat(),lonString.toFloat());
        Serial.println("Distance");
        Serial.println(String(newDistance));
        if(newDistance>distance){
            strip.setPixelColor(0,strip.Color(250,0,0));
        }else{
            strip.setPixelColor(0,strip.Color(0,250,0));
        }
        distance = newDistance;
    }


    strip.show();
    //calculate distance to a test location

    smartDelay(2000);
}

String getLocation(){
    String location = "unavailable";
    if(gps.location.isValid()){
       location = "Latitude: "+String(gps.location.lat())+", Longigtude: "+String(gps.location.lng());
    }
    smartDelay(0);
    return location;
}

void getData(const char *event, const char *data){
    Serial.println(event);
    if(data){
        String result = getString(String(data),"field1\":\"","\"}]}");
         Serial.println("*******************************************\n");
        Serial.println(result);
        destination = result;

    }else{
        Serial.println("No data");
    }

}

double getDistance(double lat, double lon){
    double lat1 = gps.location.lat();
    double lon1 = gps.location.lng();
    double lat2 = lat;
    double lon2 = lon;

    // unsigned long distance = 0;
    // if(gps.location.isValid()){
    //   distance =(unsigned long)TinyGPSPlus::distanceBetween(gps.location.lat(),gps.location.lng(),lat,lon);
    // }
    // smartDelay(0);
    // return distance;
    double p = 0.017453292519943;

     double a = 0.5 - cos((lat2 - lat1) * p)/2 +
            cos(lat1 * p) * cos(lat2 * p) *
            (1 - cos((lon2 - lon1) * p))/2;

    return  12742 * asin(sqrt(a));

}
double testDistance(double lat1,double lon1,double lat2, double lon2){



    // unsigned long distance = 0;
    // if(gps.location.isValid()){
    //   distance =(unsigned long)TinyGPSPlus::distanceBetween(gps.location.lat(),gps.location.lng(),lat,lon);
    // }
    // smartDelay(0);
    // return distance;
    double p = 0.017453292519943;

     double a = 0.5 - cos((lat2 - lat1) * p)/2 +
            cos(lat1 * p) * cos(lat2 * p) *
            (1 - (double)cos((lon2 - lon1) * p))/2;

    return  12742 * asin(sqrt(a));

}



//look for the last occurence
String getString(String input, String start,String end){
    if(input==NULL){
        return NULL;
    }
    int startIndex = input.lastIndexOf(start);
    int endIndex = input.lastIndexOf(end);
    String substring = input.substring(startIndex+start.length(),endIndex);
    return substring;
}



static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}
