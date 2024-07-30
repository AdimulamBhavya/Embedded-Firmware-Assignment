#include<stdio.h>
#include<stdint.h>

// declaring the global variables
volatile uint8_t g_ReadDIpinSts = 0x00; // Real-time status of digital input pins
volatile uint8_t g_AppDIpinSts = 0x00;  // Application digital input status

// creating an array to keep track of consecutive consistent reads
uint8_t pinConsistentCounter[8] = {0};

int ISR_DIsampling(){
    // temporary variable to store the new status of g_AppDIpinSts
    uint8_t newAppDIpinSts = g_AppDIpinSts;

    // creating loop through each digital input pin (assuming 8 digital inputs)
    for (int i = 0; i < 8; i++) {
        //extract the status of the current pin
        uint8_t pinMask = 1 << i;

        // checking if the current pin status is consistent
        if ((g_ReadDIpinSts & pinMask) == (g_AppDIpinSts & pinMask)) {
            // increment the counter for consistent reads
            if (pinConsistentCounter[i] < 10) {
                pinConsistentCounter[i]++;
            }
        } else {
            // reset the counter if the pin status is not consistent
            pinConsistentCounter[i] = 0;
        }

        // update the application status if the counter reaches 10
        if (pinConsistentCounter[i] >= 10) {
            if (g_ReadDIpinSts & pinMask) {
                newAppDIpinSts |= pinMask;
            } else {
                newAppDIpinSts &= ~pinMask;
            }
        }
    }

    // now update the global application status variable
    g_AppDIpinSts = newAppDIpinSts;

    return 0;
}
