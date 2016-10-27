// This #include statement was automatically added by the Particle IDE.
#include "TinyGPS++/TinyGPS++.h"

// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_LSM9DS0/Adafruit_LSM9DS0.h"



/* Assign a unique base ID for this sensor */
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(1000);  // Use I2C, ID #1000



#define LSM9DS0_XM_CS 10
#define LSM9DS0_GYRO_CS 9


#define LSM9DS0_SCLK 13
#define LSM9DS0_MISO 12
#define LSM9DS0_MOSI 11


/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t accel, mag, gyro, temp;
  lsm.getSensor(&accel, &mag, &gyro, &temp);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(accel.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(accel.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(accel.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(accel.max_value); Serial.println(F(" m/s^2"));
  Serial.print  (F("Min Value:    ")); Serial.print(accel.min_value); Serial.println(F(" m/s^2"));
  Serial.print  (F("Resolution:   ")); Serial.print(accel.resolution); Serial.println(F(" m/s^2"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  Serial.println(F(""));
  delay(500);
}


void configureSensor(void)
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^/


//Neopixel
/**************************************************************************/
#define PIXEL_PIN D2
#define PIXEL_COUNT 1
#define PIXEL_TYPE WS2812B
bool trigger = false;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
uint32_t color = strip.Color(0,50,200);
SYSTEM_MODE(AUTOMATIC);
int pixelCount = 0;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^/

//GPS
/**************************************************************************/
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
//String globalLocation = "unavailable"; ///latitude,longitude
//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload
String globalLocation = "unavailable";

String getLocation(){
    String location = "unavailable";
    if(gps.location.isValid()){
       location = String(gps.location.lat())+","+String(gps.location.lng());
    }
    smartDelay(0);
    globalLocation = location;
    return location;
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

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//counters
int uploadCount = 1;



void setup(void)
{
    //start Neopixel
    strip.begin();
    strip.setPixelColor(0,strip.Color(0,0,200));
    strip.show();

    //start talking with computer
    Serial.begin(9600);

    //start talking with GPS
    Serial1.begin(GPSBaud);

    Serial.println("Serial 1 started");

    /* Initialise the sensor */
    if(!lsm.begin())
    {
    /* There was a problem detecting the LSM9DS0 ... check your connections */
    Serial.print(F("Ooops, no LSM9DS0 detected ... Check your wiring or I2C ADDR!"));
    while(1);
    }
    Serial.println(F("Found LSM9DS0 9DOF"));

    /* Display some basic information on this sensor */
    displaySensorDetails();

    /* Setup the sensor gain and integration time */
    configureSensor();

    //calibrate sensor
    calibrateSensor();

}


void loop(void)
{
    strip.setPixelColor(0,strip.Color(0,0,200));
    // calibrateSensor();
    // testSensor();
    //!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload
//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload//!!!!!!!!!!!!!!!!!!!!!!testing upload
    getLocation();
    readSensor();
    pixel();
    if(uploadCount==31){
        uploadData();
        uploadCount = 0;
    }

    uploadCount++;
    smartDelay(500);
   /*
  	unsigned long now = millis();
  	if ((now - timeSinceLastRead)>=READ_SENSOR_INTERVAL) {
  	    timeSinceLastRead = now;

  	 readSensor();

  	}
	if ((now - lastTime) >= UPLOAD_INTERVAL) {
		lastTime = now;
		uploadData();
	}
    */

}


/*Setting intervals*/
//inverval for uploading data is 16s
#define UPLOAD_INTERVAL 15000
//inverval for recording good driving data is 5s -->to be decided
#define RECORD_DATA_INTERVAL 5000
//receive info from the sensor every 0.5s. if the info is good, don't do anything, if it is bad, record it
#define READ_SENSOR_INTERVAL 500

#define X_THRESHOLD 0.2
#define Y_THRESHOLD 0.2
#define Z_THRESHOLD 0.3

unsigned long timeSinceLastUpload = 0;
unsigned long timeSinceLastRecord = 0;
unsigned long timeSinceLastRead = 0;
unsigned long lastTime=0;

//original x y z acceleration average by calibration
float XORIGINAL = 0;
float YORIGINAL = 0;
float ZORIGINAL = 0;

String upload = "";
bool firstTime=true;



void calibrateSensor(){
    float xSum = 0;
    float ySum = 0;
    float zSum = 0;

    for(int i=0;i<10;i++){
        sensors_event_t accel, mag, gyro, temp;
        lsm.getEvent(&accel, &mag, &gyro, &temp);
        xSum = xSum + accel.acceleration.x;
        ySum = ySum + accel.acceleration.y;
        zSum = zSum + accel.acceleration.z;
        delay(100);
    }
    XORIGINAL = xSum/10;
    YORIGINAL = ySum/10;
    ZORIGINAL = zSum/10;
    Serial.printlnf("Calibrating Sensor");
    Serial.println(String(XORIGINAL)+" "+String(YORIGINAL)+" "+String(ZORIGINAL));
    Serial.println();

}


//neopixel----------------------------------------------------------------
void pixel(){
    if(trigger){
       if(pixelCount<=3){
           color = strip.Color(200,0,0);
           pixelCount++;
       }else{
           pixelCount = 0;
           trigger = false; //turn off the trigger
           color = strip.Color(0,250,0);
            strip.setPixelColor(0,color);
       }
    }else{
        color = strip.Color(0,250,0);
    }
     strip.setPixelColor(0,color);
     strip.show();
}
//neopixel ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

long count = 0;
void readSensor(void){
    count++;
    /* Get a new sensor event */
    String result;
  sensors_event_t accel, mag, gyro, temp;
  lsm.getEvent(&accel, &mag, &gyro, &temp);

    float x = accel.acceleration.x;
    float y = accel.acceleration.y;
    float z = accel.acceleration.z;
    bool thresholdCheck = x<XORIGINAL-X_THRESHOLD||x>XORIGINAL+X_THRESHOLD||y<YORIGINAL-Y_THRESHOLD||y>YORIGINAL+Y_THRESHOLD||z<ZORIGINAL-Z_THRESHOLD||z>ZORIGINAL+Z_THRESHOLD;
    if(thresholdCheck){
        //change pixel color
        trigger = true;
        //print to serial
        Serial.println("Threshold crossed");
        Serial.println("x: "+String(x)+"y: "+String(y)+"z: "+String(z));
        //get location

        //add to upload only add when there is a gps signal
        if(!globalLocation.equals("unavailable")){
            //add to location and motion data to upload
            //record only those that exceed threshold
            String sx = String(x);
            String sy = String(y);
            String sz = String(z);
            if(!(x<XORIGINAL-X_THRESHOLD||x>XORIGINAL+X_THRESHOLD)){
               sx = "0";
            }else if(!(y<YORIGINAL-Y_THRESHOLD||y>YORIGINAL+Y_THRESHOLD)){
                sy = "0";
            }else if(!(z<ZORIGINAL-Z_THRESHOLD||z>ZORIGINAL+Z_THRESHOLD)){
               sz = "0";
            }
            result = sx+","+sy+","+sz+","+globalLocation+";";
            Serial.println("result: "+result);
            upload = upload + result;

        }
    }
//   bool intervalCheck = (count%(RECORD_DATA_INTERVAL/500)==0);
//   if(thresholdCheck||intervalCheck){
//     String x = String(accel.acceleration.x);
//     String y = String(accel.acceleration.y);
//     String z = String(accel.acceleration.z);
//     result = String("{\"x\":")+x+String(",\"y\":")+y+String(",\"z\":")+z+String("}");
//     upload = upload+result+String(",");
//     //Serial.print(result);
//     if(intervalCheck){
//         Serial.println("record interval------------------------\n\n");
//     }
//   }else{

//   }

}


//only upload latitude and longitude, don't upload x y z.
//use comma separated, don't use json.
//only 255 characters are allowed in each field.
///upload 255 characters at a time, use a large string and remove 255 and upload those characters at a time
//only send data

void uploadData(void){
     if(!globalLocation.equals("unavailable")){
             upload = upload + "0,0,0," + globalLocation+";";
    }

    if(upload.length()>1&&upload.length()<255){
         Serial.println("upload\n"+upload);
         Particle.publish("acceleration", upload, PRIVATE);
         upload = "";

    }else if(upload.length()>=255){
        String uploadThisTime = upload.substring(0,255);
        upload = upload.remove(0,254);
        Serial.println("upload\n"+uploadThisTime);
        Particle.publish("acceleration", uploadThisTime, PRIVATE);
    }
}


//test sensor
 void testSensor(void){

         String result;
  sensors_event_t accel, mag, gyro, temp;
  lsm.getEvent(&accel, &mag, &gyro, &temp);

    //Serial.print("reading but not showing\n");


    String x = String(accel.acceleration.x);
    String y = String(accel.acceleration.y);
    String z = String(accel.acceleration.z);
    result = String("{\"x\":")+x+String(",\"y\":")+y+String(",\"z\":")+z+String("}");

    Serial.print(result);

 }
