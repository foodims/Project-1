//=====[Libraries]=============================================================
#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration and initialization of public global objects]===============
// Define inputs with pull-down mode so floating pins read as 0
DigitalIn driverSeatSensor(D2, PullDown);
DigitalIn passengerSeatSensor(D3, PullDown);
DigitalIn driverSeatBeltSensor(D4, PullDown);
DigitalIn passengerSeatBeltSensor(D5, PullDown);
DigitalIn ignitionButton(D6, PullDown);

// Define outputs
DigitalOut greenLED(LED1);  
DigitalOut blueLED(LED2);   
DigitalOut alarmBuzzer(D9);

// Serial monitor
UnbufferedSerial uartUsb( USBTX, USBRX, 115200 );

// Helper function to calculate string length
size_t getStringLength(const char* str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

// Print message to serial
void printMessage(const char* message) {
    uartUsb.write( message, getStringLength(message) );
}


//=====[Main function, the program entry point after power on or reset]========
int main()
{
    while (true) {

        bool isDriverSeated = driverSeatSensor;
        bool isPassengerSeated = passengerSeatSensor;
        bool isDriverSeatBeltOn = driverSeatBeltSensor;
        bool isPassengerSeatBeltOn = passengerSeatBeltSensor;

        // Welcome message when the driver sits down
        if ( isDriverSeated ) {
            printMessage("Welcome to enhanced alarm system model 218-W24\n");
        }

        // Green LED logic: both seats occupied and both seat belts fastened
        if ( isDriverSeated && isPassengerSeated && 
             isDriverSeatBeltOn && isPassengerSeatBeltOn ) {

            greenLED = ON;  // Enable ignition
        } else {
            greenLED = OFF; // Disable ignition
        }

        // Ignition button logic
        if ( ignitionButton ) {  // ignitionButton == 1
            if ( greenLED == ON ) {
                // Normal ignition
                greenLED = OFF;
                blueLED  = ON; 
                printMessage("Engine started.\n");
            } else {
                // Ignition inhibited
                alarmBuzzer = ON; 
                printMessage("Ignition inhibited.\n");

                // Display reasons for inhibition
                if ( !isDriverSeated ) {
                    printMessage("Driver seat not occupied.\n");
                }
                if ( !isPassengerSeated ) {
                    printMessage("Passenger seat not occupied.\n");
                }
                if ( !isDriverSeatBeltOn ) {
                    printMessage("Driver seatbelt not fastened.\n");
                }
                if ( !isPassengerSeatBeltOn ) {
                    printMessage("Passenger seatbelt not fastened.\n");
                }

                ThisThread::sleep_for(2000ms);  // allow time to read messages
                alarmBuzzer = OFF;
            }

  
        }

        // Debounce delay
        ThisThread::sleep_for(100ms);
    }
}
