#include <SoftwareSerial.h>
#include<EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <Keypad.h>

SoftwareSerial SIM800(8,7); // RX, TX
const byte buflen = 13;
struct customer_data {
  
  char customer_number[buflen];
 
};
customer_data data[15]; //data[0].customer_name; for testing keeping only 5
// depending your fingerprint sensor you can set it to 20 or 200

customer_data tempData[15];


int value = analogRead(A1);
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 6, 9, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {11, 12, 2}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );



LiquidCrystal_I2C lcd(0x27,16,2);//SCL-A5,SDA-A4
//SoftwareSerial mySerial(0, 1);//Rx,TX=D0,D1 

uint8_t id;


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial);
//#define enroll 14
//#define del 15
#define up 16
#define down 17
//#define match 20

 
#define sw1 3
#define sw2 4
//#define sw3 13
//#define resultsw 2
//#define indVote 13
//#define indFinger 8
#define buzzer 14
#define records 25
int vote1,vote2,vote3;
 
int flag;
 
void setup() 
{
    lcd.init();                      // initialize the lcd 
    lcd.init();
  // Print a message to the LCD.
    lcd.backlight();
    delay(1000);
     SIM800.begin(9600);   // setting the baud rate of GSM module
     Serial.begin(9600);
    pinMode(up, INPUT_PULLUP); 
    pinMode(down, INPUT_PULLUP); 
    //pinMode(del, INPUT_PULLUP);
    //pinMode(match, INPUT_PULLUP);
    pinMode(sw1, INPUT_PULLUP); 
    pinMode(sw2, INPUT_PULLUP);
  //  pinMode(sw3, INPUT_PULLUP);
    //pinMode(resultsw, INPUT_PULLUP);
    pinMode(buzzer, OUTPUT);
   //  pinMode(indVote, OUTPUT);
   // pinMode(indFinger, OUTPUT);
 
lcd.begin(16,2);
  if (digitalRead(up) == 0)
  {
       for(int i=0;i<records;i++)
       EEPROM.write(i+50,0xff);
       for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0xff);
   }    
       
       
       EEPROM.write(3,0);
       EEPROM.write(2,0);
       
       lcd.clear();
       lcd.print(F("System Reset"));
       delay(1000);
  }
    
    lcd.clear();
    lcd.print(F("Voting Machine"));
    lcd.setCursor(0,1);
    lcd.print(F("by Finger Print"));
    delay(2000);
    lcd.clear();
    lcd.print(F("DSCE-ECE"));
    lcd.setCursor(0,1);
    lcd.print(F("Mini Project"));
    delay(2000);
 
  if(EEPROM.read(3) == 0xff)
   EEPROM.write(3,0);
 
  
 
  if(EEPROM.read(2) == 0xff)
   EEPROM.write(2,0);
 
   
      
    //finger.begin(57600);
    Serial.begin(57600);
    lcd.clear();
    lcd.print(F("Finding Module"));
    lcd.setCursor(0,1);
    delay(1000);
    if (finger.verifyPassword()) 
    {
      //Serial.println(F("Found fingerprint sensor!"));
      lcd.clear();
      lcd.print(F("Found Module "));
      delay(1000);
    } 
    else 
    {
    //Serial.println(F("Did not find fingerprint sensor :));
    lcd.clear();
    lcd.print(F("module not Found"));
    lcd.setCursor(0,1);
    lcd.print(F("Check Connections"));
    while (1);
    }
 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Cn1"));
  lcd.setCursor(4,0);
  lcd.print(F("Cn2"));  
  
 
  lcd.setCursor(0,1);
  vote1=EEPROM.read(2);
  lcd.print(vote1);
  lcd.setCursor(6,1);
  vote2=EEPROM.read(3);
  lcd.print(vote2);
  lcd.setCursor(12,1);
  
  delay(2000);
}
 
void loop() 
{
 int value = analogRead(A1);

 lcd.setCursor(0,0);
 lcd.print(F("Press Match Key "));
 lcd.setCursor(0,1);
 lcd.print(F("to start system"));
 if (value > 480 && value < 650)
 {
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW); 
   lcd.clear();
    
  delay(1000);
  
  
  for(int i=0;i<4;i++)
  {
    lcd.clear();
    lcd.print(F("Place Finger"));
    delay(500);
    int result=getFingerprintIDez();
    if(result>=0)
    {
      flag=0;
      
        for(int i=0;i<records;i++)
        {
          if(result == EEPROM.read(i+50))
          {
             lcd.clear();
              lcd.print(F("Authorised Voter"));
              lcd.setCursor(0,1);
              lcd.print(F("Please Wait...."));
              delay(1000);  
              //Enter OTP for further voting
              
              otpsend(result);
              EEPROM.write(i+50, 0xff);
              flag=1;
              return;
          }
        }
         if(flag == 0)
        {
        lcd.clear();
        lcd.print(F("Already Voted"));
        lcd.setCursor(0,1);
        
        lcd.print("");
        digitalWrite(buzzer, HIGH);
        delay(5000);
        digitalWrite(buzzer, LOW);
        return;
        }
        
        
        }
  }
    lcd.clear();
   
  }
 



 

 
 

 checkKeys();
 delay(1000);
  if (value > 650 && value < 750)
        {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("INC");
              lcd.setCursor(6,0);
              lcd.print("BJP");
              
              for(int i=0;i<2;i++)
              {
                lcd.setCursor(i*6,1);
                lcd.print(EEPROM.read(i));
              }
              delay(2000);
           int vote=vote1+vote2;
           if(vote)
           {
            if(vote1 > vote2)
            {
             lcd.clear();
             lcd.print("INC Wins");
             delay(2000);
             lcd.clear();
            }
            else if(vote2 > vote1 )
            {
             lcd.clear();
             lcd.print("BJP Wins");
             delay(2000);
             lcd.clear();
            }
            
           
           else
           {
             lcd.clear();
             lcd.print("   Tie Up Or   ");
             lcd.setCursor(0,1);
             lcd.print("   No Result   ");
             delay(1000);
             lcd.clear();
           }
           
          }      
           else 
           {
             lcd.clear();
             lcd.print("No Voting....");
             delay(2000);
             lcd.clear();
           }
           vote1=0;vote2=0;vote=0;
           lcd.clear();
           
         return;
         }
}

 
void checkKeys()
{
 int value = analogRead(A1); 
   if (value >800  && value < 1000)
   {
    lcd.clear();
    lcd.print(F("Please Wait"));
    delay(1000);    
    Enroll();
   }
 
   else if (value > 750 && value < 800)
   {
    lcd.clear();
    lcd.print(F("Please Wait"));
    delay(1000);
    delet();
   }  
}
 
void Enroll()
{
  int value = analogRead(A1);
   int count=0;
   lcd.clear();
   lcd.print(F("Enter Finger ID:"));
   
   while(1)
   {
    int value = analogRead(A1);
    lcd.setCursor(0,1);
     lcd.print(count);
     if(digitalRead(up) == 0)
     {
       count++;
       if(count>25)
       count=0;
       delay(500);
     }
 
     else if(digitalRead(down) == 0)
     {
       count--;
       if(count<0)
       count=25;
       delay(500);
     }
    else if (value > 750 && value < 800)
     {
          id=count;
          getFingerprintEnroll();  //Ask voter to enter registered mobile number
          
          for(int i=0;i<records;i++)
          {
            if(EEPROM.read(i+50) == 0xff)
            {
              EEPROM.write(i+50, id);
              break;
            }
          }
         
          
          return;
     }
 
      else if (value > 800 && value < 1000)
     {        
          return;
     }
 }
}
 
void delet()
{
  int value = analogRead(A1);
   int count=0;
   lcd.clear();
   lcd.print(F("Enter Finger ID"));
   
   while(1)
   {
    int value = analogRead(A1);
    lcd.setCursor(0,1);
     lcd.print(count);
     if(digitalRead(up) == 0)
     {
       count++;
       if(count>25)
       count=0;
       delay(500);
     }
 
     else if(digitalRead(down) == 0)
     {
       count--;
       if(count<0)
       count=25;
       delay(500);
     }
     else if (value > 750 && value < 800)
     {
          id=count;
          deleteFingerprint(id);
          for(int i=0;i<records;i++)
          {
            if(EEPROM.read(i+50) == id)
            {
              EEPROM.write(i+50, 0xff);
              break;
            }
          }
          return;
     }
       else if (value > 800 && value < 1000)
     {        
          return;
     }
 }
}
 
uint8_t getFingerprintEnroll() 
{
  int p = -1;
  lcd.clear();
  lcd.print(F("finger ID:"));
  lcd.print(id);
  lcd.setCursor(0,1);
  lcd.print(F("Place Finger"));
  delay(2000);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      Serial.println(F("Image taken"));
      lcd.clear();
      lcd.print(F("Image taken"));
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(F("No Finger"));
      lcd.clear();
      lcd.print(F("No Finger"));
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(F("Communication error"));
      lcd.clear();
      lcd.print(F("Comm Error"));
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println(F("Imaging error"));
      lcd.clear();
      lcd.print(F("Imaging Error"));
      break;
    default:
      Serial.println(F("Unknown error"));
       lcd.clear();
      lcd.print(F("Unknown Error"));
      break;
    }
  }
 
  // OK success!
 
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Image converted"));
      lcd.clear();
      lcd.print(F("Image converted"));
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println(F("Image too messy"));
       lcd.clear();
       lcd.print(F("Image too messy"));
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(F("Communication error"));
            lcd.clear();
      lcd.print(F("Comm Error"));
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println(F("Could not find fingerprint features"));
            lcd.clear();
      lcd.print(F("Feature Not Found"));
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println(F("Could not find fingerprint features"));
                  lcd.clear();
      lcd.print(F("Feature Not Found"));
      return p;
    default:
      Serial.println(F("Unknown error"));
                  lcd.clear();
      lcd.print(F("Unknown Error"));
      return p;
  }
  
  //Serial.println(F("Remove finger"));
  lcd.clear();
  lcd.print(F("Remove Finger"));
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  //Serial.print(F("ID ")); //Serial.println(id);
  p = -1;
  //Serial.println(F("Place same finger again"));
   lcd.clear();
      lcd.print(F("Place Finger"));
      lcd.setCursor(0,1);
      lcd.print(F("   Again"));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      //Serial.println(F("Image taken"));
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.print(F("."));
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println(F("Communication error"));
      break;
    case FINGERPRINT_IMAGEFAIL:
      //Serial.println(F("Imaging error"));
      break;
    default:
      //Serial.println(F("Unknown error"));
      return;
    }
  }
 
  // OK success!
 
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
    lcd.clear();
    lcd.print(F("Image converted"));
      //Serial.println(F("Image converted"));
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println(F("Image too messy"));
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println(F("Communication error"));
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println(F("Could not find fingerprint features"));
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println(F("Could not find fingerprint features"));
      return p;
    default:
      //Serial.println(F("Unknown error"));
      return p;
  }
  
  // OK converted!
  //Serial.print(F("Creating model for #"));  //Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
     lcd.clear();
    lcd.print(F("Prints matched!"));
    //Serial.println(F("Prints matched!"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println(F("Communication error"));
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    //Serial.println(F("Fingerprints did not match"));
    return p;
  } else {
    //Serial.println(F("Unknown error"));
    return p;
  }   
  
  //Serial.print(F("ID ")); //Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    //Serial.println(F("Stored!"));
    lcd.clear();                      
    lcd.print(F("Stored!"));
    
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println(F("Communication error"));
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    //Serial.println(F("Could not store in that location"));
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    //Serial.println(F("Error writing to flash"));
    return p;
  }
  else {
    //Serial.println(F("Unknown error"));
    return p;
  }  
 // int a=getFingerprintIDez();
  recdata(id);
   
}
 
int getFingerprintIDez()
{
  uint8_t p = finger.getImage();
  
  if (p != FINGERPRINT_OK)  
  return -1;
 
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  
  return -1;
 
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
   lcd.clear();
   lcd.print(F("Finger Not Found"));
   lcd.setCursor(0,1);
   lcd.print(F("Try Later"));
   delay(2000);  
  return -1;
  }
  // found a match!
  //Serial.print(F("Found ID #")); 
  //Serial.print(finger.fingerID); 
  return finger.fingerID; 
}

 
uint8_t deleteFingerprint(uint8_t id) 
{
  uint8_t p = -1;  
  lcd.clear();
  lcd.print(F("Please wait"));
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) 
  {
    //Serial.println(F("Deleted!"));
    lcd.clear();
    lcd.print(F("Figer Deleted"));
    lcd.setCursor(0,1);
    lcd.print(F("Successfully"));
    delay(1000);
  } 
  
  else 
  {
    //Serial.print(F("Something Wrong")); 
    lcd.clear();
    lcd.print(F("Something Wrong"));
    lcd.setCursor(0,1);
    lcd.print(F("Try Again Later"));
    delay(2000);
    return p;
  }   
}
 
void Vote()
{
  lcd.clear();
  lcd.print(F("Please Place"));
  lcd.setCursor(0,1);
  lcd.print(F("Your Vote"));
  
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(1000);
  while(1)
 {
        if(digitalRead(sw1)==0)
        {
          vote1++;
          voteSubmit(1);
          EEPROM.write(2, vote1);
          while(digitalRead(sw1)==0);
          return;
        }
        if(digitalRead(sw2)==0)
        {
          vote2++;
            voteSubmit(2);
            EEPROM.write(3, vote2);
          while(digitalRead(sw2)==0);
          return;
        }
        
        
        
         if(value > 650 && value < 750)
          {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print(F("INC"));
              lcd.setCursor(6,0);
              lcd.print(F("BJP"));
              
              lcd.setCursor(0,1);
              lcd.print(EEPROM.read(2));
              lcd.setCursor(6,1);
              lcd.print(EEPROM.read(3));
              delay(2000);
           int vote=vote1+vote2;
           if(vote)
           {
            if(vote1 > vote2 )
            {
             lcd.clear();
             lcd.print(F("INC Wins"));
             delay(2000);
             lcd.clear();
            }
            else if(vote2 > vote1)
            {
             lcd.clear();
             lcd.print(F("BJP Wins"));
             delay(2000);
             lcd.clear();
            }
            
           
           else
           {
             lcd.clear();
             lcd.print(F("   Tie Up Or   "));
             lcd.setCursor(0,1);
             lcd.print(F("   No Result   "));
             delay(1000);
             lcd.clear();
           }
           
          }      
           else 
           {
             lcd.clear();
             lcd.print(F("No Voting...."));
             delay(1000);
             lcd.clear();
           }
           vote1=0;vote2=0;vote=0;
           lcd.clear();
           
           return;
         }
  }
  // digitalWrite(indVote, LOW);
}
 
void voteSubmit(int cn)
{
  lcd.clear();
  if(cn == 1)
      lcd.print(F("INC"));
  else if(cn == 2)
      lcd.print(F("BJP"));
  
  lcd.setCursor(0,1);
  lcd.print(F("Vote Submitted"));
  digitalWrite(buzzer , HIGH);
  delay(2000);
  digitalWrite(buzzer, LOW);
  //digitalWrite(indVote, LOW);
  return;
}






int getcode() {

  String y = "";
  int a = y.length();
  while (a < 5)
  {
    char customKey = customKeypad.getKey();
    if (customKey) {
      lcd.setCursor(0, 1);
      y = y + customKey;
      lcd.print(y);
      a = y.length();
      
    }
  }
 return y.toInt();;
}

void recdata(int id){
 String stringData;
 lcd.clear();
 lcd.setCursor(0,0);
lcd.print(F("Enter your mobile no"));
 String y = "";
  int a = y.length();
  while (a < 12)
  {
    char customKey = customKeypad.getKey();
    if (customKey) {
      lcd.setCursor(0, 1);
      y = y + customKey;
      lcd.print(y);
      a = y.length();
      
    }
  }
 
    lcd.clear();
   lcd.print(y);
   delay(2000);
   
  y.toCharArray(data[id].customer_number, buflen);
  lcd.clear();
  
  lcd.setCursor(0,1);
  lcd.print(F("Thank You!"));
  delay(1000);
  int addrr = id * buflen;
  EEPROM.put(addrr, data[id]);
  lcd.clear();
  lcd.print(data[id].customer_number);
}

void sendOtpMessage(int genOTP, String number) {
  String msg = "";
  msg = "Your OTP is :" + String(genOTP);
  //Serial.println(msg);

  String cmd = "";
  SIM800.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);     // Delay of 1000 milli seconds or 1 second
  cmd = "AT+CMGS=\"+" + number + "\"";
  SIM800.println(cmd);
  delay(1000);

  SIM800.println(msg);   // The SMS text you want to send
  delay(100);
  SIM800.println((char)26); // ASCII code of CTRL + Z
  delay(2000);
}

void otpsend(int id){
  String stringData;
  long otpvalue, tempotp;
  int addrr = id * buflen;
  EEPROM.get(addrr, tempData[id]);
  
  String CstRegNum = tempData[id].customer_number;
  lcd.clear();
  delay(1000);
  lcd.print(CstRegNum);
  delay(2000);
  if (id < 25)       // for testing we are using only 5 but here also depending on your finger module you can change it
    {
      otpvalue = random(10000,99999);
      
      sendOtpMessage(otpvalue, CstRegNum);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F(" Enter OTP sent"));
      
      //Serial.print(F("Enter OTP: "));
      delay(1000);
    }
    else
    {
      lcd.print(F("No Record found"));
      return;
    }
    tempotp=getcode();
  if (otpvalue == tempotp) {
    Vote();
  }
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Invalid OTP"));
    delay(1000);
    
    }
  }

 
        
