#include <RL_RFID.h>
#include <SPI.h>
#include <Ethernet.h>
#include <RL_HumidityTemp.h>
#include <RL_LCD.h>
#include <RL_Switch.h>


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// Set the static IP address
IPAddress ip(192, 168, 0, 111);

// Initialize the library instance:
EthernetClient client;

char server[16] = "192.168.0.1";
char url[2000];

HTSensor ht; // Humidity & Temp sensor

RFID rfid; // make new object

LCD lcd(0x27,16,2); //LCD display

Switch pushSwitch;  //Push switch

char devID[30] = "vA5A66B9D4D55143"; //device id


// last time you connected to the server, in milliseconds
unsigned long lastConnectionTime = 0;           

// delay between updates, in milliseconds
const unsigned long postingInterval = 15000UL;

// Variable to check active state
int isActive = 1;

// Variable to count the number of digits entered from switch board
int count = 0;

// Int array to store digits pressed
int digits[3];

void setup() {

  // Ethernet.init(pin) to configure the CS pin
  Ethernet.init(32);  

  // start serial port:
  Serial.begin(57600);

  // Initiate humidity temp sensor
  ht.begin();
  
  // Light mode pin set
  pinMode(A0, INPUT);
  rfid.PCD_Init(); // Init MFRC522
  
  // Init LCD display
  lcd.init();
  
  // Init Push switch
  pushSwitch.begin();

  Ethernet.begin(mac, ip);
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  // give the Ethernet shield a second to initialize:
  delay(1000);
}


void loop() {

  // if there's an incoming data from the net connection,
  // send it out through serial port.  This is for debugging
  // purposes only:

	if(client.find("\r\n\r\n"))  {
		while (client.available()) {
			char c = client.read();
			//Serial.print("Char : ");
			Serial.print(c);
		  		  
			if(c == 'a') {  // If card is verified then update LCD display and set System state as SUSPENDED
				Serial.println(" Card verified");
				isActive = 0; 
				lcd.clear();
				lcd.setCursor(1,0);
				lcd.print("Authentication ");
				lcd.setCursor(2,1);
				lcd.print("Successful!");

        httpRequest(2, 0); //send notification of verified card
			}else if(c == '1'){ // Confirmation of Light and temp added and updating System state as ACTIVE
				Serial.println(" Added Light and Temp");
				isActive = 1;
			} else if(c == 'r') {
				Serial.println(" RFID card incorrect");
			} else if(c == '0') {
       Serial.println(" Error in updating values");
      }
		}
	}

  // if fifteen seconds have passed since your last connection and system is in ACTIVE state,
  // then connect again and send data:
	if (millis() - lastConnectionTime > postingInterval && isActive == 1) {
  
		Serial.print("\n===============================\nMy IP address: ");
		Serial.println(Ethernet.localIP());
		httpRequest(0, 0);
  
	}

// Listen for RFID cards if system is in ACTIVE state
	if(isActive == 1){
    
		// Look for new cards
		if(!rfid.isNewCardPresent()) { return; }
		
		// Select one of the cards
		if(!rfid.readCardSerial()) { return; }
		
		httpRequest(1, 0);
		
	}
// Read input from Switch module when system is in SUSPENDED state
	if(isActive == 0){
		//Read key pushed by user
		int pushKey = pushSwitch.getPushKey();
		if(pushKey >= 0){
			Serial.print("Pushed Key : ");
			Serial.println(pushKey);
			// Assigning the pushed key to corresponding digit number in int array
			digits[count] = pushKey;
			// Increamenting the step counter of int array
			count++;
			Serial.print("Count: ");
			Serial.println(count);
		  
		    // When counter int array counter reach 3 then make number from int array
			if(count==3){
				Serial.print("No is: ");
				int samplingNo = (digits[0] * 100) + (digits[1] *10) + digits[2];
				Serial.println(samplingNo);
				
				//Reset the count variable;
				count=0;
				
				//Check if sampling number is greated than 50
				if(samplingNo >= 50){
					httpRequest(0, samplingNo);
				}else{
					Serial.println("Sampling number should be between 50 to 999");
				}
			}else{
			  delay(2000);
			}
		}
	}
}



// this method makes an HTTP connection to the server:
//variable mode works in mode 0 to post Light and temp data and in mode 1 to post RFID card data
void httpRequest(int mode, int samplingNo) {
	// close any connection before sending a new request.
  
	client.stop();

	// if there's a successful connection:
	if (client.connect(server, 80)) {
    
		memset(url, '\0', 2000); 

//Mode to send Light and Temp data
		if(mode == 0){
		
		//Read temp data
			float t = ht.readTemperature();
		// Read light value
			uint16_t lightLevel = 1024 - analogRead(A0);
			
		// Check if sampling number have been sent by switch module
			if(samplingNo>0){
				// Variable to calculate total light value
				long lightsTotal = 0;
			  
				Serial.print("Taking ");
				Serial.print(samplingNo);
				Serial.println(" samples in 15 second");
				for(int i = 0; i<samplingNo; i++){
					uint16_t lightLevel = 1024 - analogRead(A0);
					lightsTotal = lightsTotal+lightLevel;
					//Delay mechanism to take exact number of samples in 15 second
					delay(15000/samplingNo);
				}
				//update light value variable
				lightLevel = (uint16_t) (lightsTotal/samplingNo);
				Serial.println(lightLevel);
			}
		// Posting light and temp data to server
			sprintf(url, "GET /alacritas_direct.php?acc=nnarwal&call=KIT717/A1/recordTempLight.php&t=%d.%02d&l=%d HTTP/1.1", (int) t, ((int) (t * 100) % 100), lightLevel);
			
			Serial.println(url);
      Serial.println("connecting...");

		// Mode to send RFID card data
		}else if(mode == 1){
			//Psoting RFID card details
			sprintf(url, "GET /alacritas_direct.php?acc=nnarwal&call=KIT717/A1/checkRFID.php&rfid=%x-%x-%x-%x HTTP/1.1", rfid.uid.uidByte[0], rfid.uid.uidByte[1], rfid.uid.uidByte[2], rfid.uid.uidByte[3]);
			
			Serial.println(url);
      Serial.println("connecting...");

		}else if(mode == 2) { // send pushingBox message
      sprintf(url, "GET /pbox_direct.php?devid=%s HTTP/1.1", devID);
      Serial.println(url);
      Serial.println("Sending notification...");
    }
		delay (10);


		// send the HTTP GET request:
		client.println(url);
		client.println("Host: 192.168.0.1");
		client.println("User-Agent: arduino-ethernet");
		client.println("Connection: close");
		client.println();


		// note the time that the connection was made:
		lastConnectionTime = millis();

	} else {
		// if you couldn't make a connection:
		Serial.println("connection failed");
	}
}
