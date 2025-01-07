#include <LiquidCrystal.h>

#include <SoftwareSerial.h>

#include <string.h> 

LiquidCrystal lcd(4, 5, 6, 9, 7, 8);

SoftwareSerial Serial1(2,3);

void setup()
{
Serial.begin(4800);
Serial1.begin(4800);
delay(300);
lcd.begin(20,4);             // limit czasu 10s
lcd.setCursor(0,0);
lcd.print("    Yeasu 757GX2"); 
delay(1000);
lcd.setCursor(0,1);
lcd.print("  External Display");
delay(1000);
lcd.setCursor(0,2);
lcd.print("     Controller");
delay(1000);
lcd.setCursor(0,3);
lcd.print("   v.1.0 by SP5TOF");
delay(5000);
lcd.clear();

}

void loop()
{
  String clari;
  byte freq_in[4] ;
unsigned long freq_out ;

lcd.setCursor(14,4);
lcd.print("S ");
lcd.print(getSmetr757());
Serial1.flush(); 
byte command[5] = {0x00,0x00,0x00,0x00,0x00};
command[4] = 0x10;

for (byte i=0; i<5; i++) {
Serial1.write(command[i]);
}
delay(5);

byte status[75];
long timeout = millis();
long elapsed = 0;

while (Serial.available() < 1 && elapsed < 200) {
  delay(10);
		elapsed = millis() - timeout;
   	;}


	for (byte j = 0; j < 75; j++) {
		status[j] = Serial1.read();  

   	}
   
// analyze and display status flag

byte status_flags ; // status flag bits
//status_flags=status[0];
int flags[8];
String flag;
 
 CrackByte( status[0], flags );
 
//device lock pointer
if (flags[0]==0) 
{
  lcd.setCursor(17,2);
  lcd.print("   ");
} 
if (flags[0]==1)
{
   lcd.setCursor(17,2);
  lcd.print(" LK");
}

//splitting
if (flags[1]==1) 
{
  
  lcd.setCursor(7,3);
  lcd.print( "Split");
  lcd.setCursor(1,2);
  if(flags[3]==1) //if active vfo B
  {
String frequency_vfoa;  


freq_in[0] = status[13];
freq_in[1] = status[12];
freq_in[2] = status[11];
freq_in[3] = status[10];

freq_out = from_bcd_be(freq_in, 8);


frequency_vfoa =  "VFOA:" + frequency_conv(freq_out); 
frequency_vfoa.replace("Mhz","");
lcd.setCursor(1,2);
lcd.print(frequency_vfoa);  
lcd.print(band_mode(status[14]));
  }
if (flags[3]==0) //if activa vfo a
{
  
String frequency_vfob;

freq_in[0] = status[18];
freq_in[1] = status[17];
freq_in[2] = status[16];
freq_in[3] = status[15];

freq_out = from_bcd_be(freq_in, 8);

frequency_vfob =  "VFOB:" + frequency_conv(freq_out); 
frequency_vfob.replace("Mhz", "");
lcd.setCursor(1,2);
lcd.print(frequency_vfob);
lcd.print(band_mode(status[19]));  
  
}
  
}

if (flags[1]==0)  flag = flag  +  " Smplx ";

//clarifier freq display

	
if (flags[2]==0) 
{
  lcd.setCursor(17,1);
  lcd.print("   ");
  lcd.setCursor(0,1);
clari = "                ";
lcd.print(clari);

 lcd.setCursor(14,0);
  lcd.print("  ");
  
  lcd.setCursor(14,1);
  lcd.print("  ");
}
if (flags[2]==1)  
{
  lcd.setCursor(17,1);
  lcd.print("CLR");
  
  lcd.setCursor(14,0);
  lcd.print("rx");
  
  lcd.setCursor(14,1);
  lcd.print("tx");


freq_in[0] = status[23];
freq_in[1] = status[22];
freq_in[2] = status[21];
freq_in[3] = status[20];

freq_out = from_bcd_be(freq_in, 8);

lcd.setCursor(0,1);
clari = frequency_conv(freq_out);
lcd.print(clari);
}
if (flags[3]==0)  
{
  lcd.setCursor(0,4);
  lcd.print("VFO A");
}
if (flags[3]==1)
{
  lcd.setCursor(0,4);
  lcd.print("VFO B");
}
if (flags[4]==0)  
{
  if (flags[1]==0)
  {
  lcd.setCursor(1,2);
  lcd.print("                 ");
  
  lcd.setCursor(7,3);
lcd.print("      ");
  }
}
if (flags[4]==1)
{
  lcd.setCursor(1,2);
  lcd.print("MEMORY OPERATE  ");
  
  byte selected_memory_channel ;
String channel;
selected_memory_channel=status[4] ;

if (selected_memory_channel==0x00) channel = channel + " CH:0"; 
if (selected_memory_channel==0x01) channel = channel + " CH:1"; 
if (selected_memory_channel==0x02) channel = channel + " CH:2"; 
if (selected_memory_channel==0x03) channel = channel + " CH:3"; 
if (selected_memory_channel==0x04) channel = channel + " CH:4"; 
if (selected_memory_channel==0x05) channel = channel + " CH:5"; 
if (selected_memory_channel==0x06) channel = channel + " CH:6"; 
if (selected_memory_channel==0x07) channel = channel + " CH:7"; 
if (selected_memory_channel==0x08) channel = channel + " CH:8"; 
if (selected_memory_channel==0x09) channel = channel + " CH:9"; 

lcd.setCursor(7,3);
lcd.print(channel);
  
  
  
  
}
if (flags[5]==0)  flag = flag  +  " RX ";
if (flags[5]==1)  flag = flag  +  " TX ";
if (flags[6]==0)  flag = flag  +  " Gen Steps ";
if (flags[6]==1)  flag = flag  +  " Ham Band Steps ";   


Serial.println(" ");
Serial.println(flag);

String band;
byte band_data; 

band_data=status[3];

if (band_data==0x01) band = band + " Band below 2.5 Mhz "; 
if (band_data==0x02) band = band + " Band 2.5 - 4.0 "; 
if (band_data==0x03) band = band + " Band 4.0 - 7.5 "; 
if (band_data==0x04) band = band + " Band 7.5 - 10.5 "; 
if (band_data==0x05) band = band + " Band 10.5 - 14.5 "; 
if (band_data==0x06) band = band + " Band 14.5 - 18.5 "; 
if (band_data==0x07) band = band + " Band 18.5 - 21.5 "; 
if (band_data==0x08) band = band + " Band 21.5 - 25.0 "; 
if (band_data==0x09) band = band + " Band 25.0 - 30.0 "; 

Serial.println(band);


// extract selected memory channel


freq_in[0] = status[8];
freq_in[1] = status[7];
freq_in[2] = status[6];
freq_in[3] = status[5];

freq_out = from_bcd_be(freq_in, 8);

lcd.setCursor(0,0);
lcd.print(frequency_conv(freq_out)) ;



// extract band mode of operating frequency

lcd.setCursor(16,0);
lcd.print(band_mode(status[9]));

// extract VFO A frequency and Mode

String frequency_vfoa;

freq_in[0] = status[13];
freq_in[1] = status[12];
freq_in[2] = status[11];
freq_in[3] = status[10];

freq_out = from_bcd_be(freq_in, 8);

frequency_vfoa =  " VFO A " + frequency_conv(freq_out); 

Serial.print(frequency_vfoa);
Serial.print(band_mode(status[14]));
Serial.println(" ");

// extract VFO B frequency Mode

freq_in[0] = status[18];
freq_in[1] = status[17];
freq_in[2] = status[16];
freq_in[3] = status[15];

freq_out = from_bcd_be(freq_in, 8);

String frequency_vfob;
frequency_vfob =  " VFO B " + frequency_conv(freq_out); 

Serial.print(frequency_vfob);
Serial.print(band_mode(status[19]));
Serial.println(" ");
// extract Clarifier Frequency and Mode

Serial.print(clari);
Serial.print(band_mode(status[24]));
Serial.println(" ");

delay(500);


}


void CrackByte( byte b, int variable[8] ) // splitting byte into the 8-bits
{
 byte i;
 
 for ( i=0; i < 8; ++i )
 {
   variable[i] = b & 1;
   b = b >> 1;
 }
}

unsigned long from_bcd_be (const byte bcd_data[], unsigned bcd_len) //converting the incoming bytes into the BCD frequency
{
	int i;
	long f = 0;

	for (i=0; i < bcd_len/2; i++) {
		f *= 10;
		f += bcd_data[i]>>4;
		f *= 10;
		f += bcd_data[i] & 0x0f;
	}
	if (bcd_len&1) {
		f *= 10;
		f += bcd_data[bcd_len/2]>>4;
	}
	return f;
}

String frequency_conv (unsigned long frequency_in) //converting the BCD frequency for LCD purpose
{
  String frequency_temp, frequency_temp2;
  frequency_temp+= frequency_in;
  frequency_temp2+=frequency_in;
  
if(frequency_temp.length()==6)
{
  frequency_temp.remove(1,5);
  frequency_temp2.remove(0,1);
    return  frequency_temp + "." + frequency_temp2 + " Mhz  " ;
  }
else if(frequency_temp.length()==7)
  {
  frequency_temp.remove(2,5);
  frequency_temp2.remove(0,2);
    return  frequency_temp + "." + frequency_temp2 + " Mhz " ;
  }

else
{

 return "Loading/Error  " ;
// return "FRE:" + frequency_temp + "." + frequency_temp2 + " Mhz" ;
}

}

String band_mode(byte band_in)
{
  String mode;
  
if (band_in==0x00) mode = mode + " LSB "; 
if (band_in==0x01) mode = mode +  " USB "; 
if (band_in==0x02) mode = mode +  " CWW "; 
if (band_in==0x03) mode = mode +  " CWN "; 
if (band_in==0x04) mode = mode +  " AM "; 
if (band_in==0x05) mode = mode +  " FM "; 

return mode;
}

String getSmetr757() {
byte rigGetSmetr[5] = {0x00,0x00,0x00,0x00,0x00};
	rigGetSmetr[4] = 0x10; // command byte
	rigGetSmetr[3] = 0x01 ; //parameter1 = 01 (Meter)
	byte Smetr ;
	long timeout = millis();
	long elapsed = 0;

	Serial1.flush();	// clear the RX buffer which helps prevent
				// any crap data from making it through

        for (byte i=0; i<5; i++) {
		Serial1.write(rigGetSmetr[i]);
	}
	
	while (Serial.available() < 1 && elapsed < 100) {
		elapsed = millis() - timeout;
    	;}
	
	Smetr=Serial1.read();


	Serial1.flush();	// clear the RX buffer which helps prevent
				// any crap data from making it through
        String smeter = String(Smetr, DEC) + "  ";
	return smeter;
}

