// Include standard C libraries for limits, math, I/O, process control, and system info
#include <limits.h>    // For integer limits (e.g., INT_MAX)
#include <math.h>      // For mathematical functions
#include <stdio.h>     // For input/output functions like printf
#include <stdlib.h>    // For general utilities like exit
#include <unistd.h>    // For POSIX operating system API (e.g., sleep)
#include <sys/types.h> // For data types used in system calls
#include <sys/sysinfo.h> // For system information functions

// Include custom header file (assumed to contain Red Pitaya-specific definitions)
#include "main.h"

// Note: This code is tailored for Red Pitaya OS version 2.05-37
// Many functions and types are defined in the Red Pitaya ecosystem files
// See RedPitaya_Ecosystem\RedPitaya-master\Examples for example code
// Check RedPitaya_Ecosystem\RedPitaya-master\rp-api\api\src\rp.c, ..\src, ..\include for details
// GitHub link for ecosystem version: https://github.com/RedPitaya/RedPitaya

// Define an array of 8 LED state parameters
// Format: CBooleanParameter("NAME", CBaseParameter::RW, initial_value, version)
// - "NAME" must match the parameter name in JavaScript (e.g., "LED0_STATE")
// - RW means read/write
// - Initial value is false (off), matching HTML’s default state (0/false)
// - Version is 0
CBooleanParameter ledStates[8] = {
    CBooleanParameter("LED0_STATE", CBaseParameter::RW, false, 0), // LED 0 state
    CBooleanParameter("LED1_STATE", CBaseParameter::RW, false, 0), // LED 1 state
    CBooleanParameter("LED2_STATE", CBaseParameter::RW, false, 0), // LED 2 state
    CBooleanParameter("LED3_STATE", CBaseParameter::RW, false, 0), // LED 3 state
    CBooleanParameter("LED4_STATE", CBaseParameter::RW, false, 0), // LED 4 state
    CBooleanParameter("LED5_STATE", CBaseParameter::RW, false, 0), // LED 5 state
    CBooleanParameter("LED6_STATE", CBaseParameter::RW, false, 0), // LED 6 state
    CBooleanParameter("LED7_STATE", CBaseParameter::RW, false, 0)  // LED 7 state
};

// Function to return a description of the application
const char *rp_app_desc(void)
{
    return (const char *)"Custom UI of LED Web Control\n"; // Custom description for this app
}

// Function to initialize the application
int rp_app_init(void)
{
    fprintf(stderr, "Loading LED control\n"); // Print a message to indicate loading

    // Initialize the Red Pitaya API (function provided by Red Pitaya)
    if (rpApp_Init() != RP_OK) // Check if initialization fails
    {
        fprintf(stderr, "Red Pitaya API init failed!\n"); // Print error message
        return EXIT_FAILURE; // Exit with failure code
    }
    else 
    {
        fprintf(stderr, "Red Pitaya API init success!\n"); // Print success message
    }

    return 0; // Return 0 to indicate success
}

// Function to clean up and exit the application
int rp_app_exit(void)
{
    fprintf(stderr, "Unloading LED control\n"); // Print a message to indicate unloading

    rpApp_Release(); // Release resources used by the Red Pitaya API (function provided by Red Pitaya)

    return 0; // Return 0 to indicate successful exit
}

// Function to set parameters (placeholder, does nothing for now)
int rp_set_params(rp_app_params_t *p, int len)
{
    return 0; // Return 0 (success), no implementation yet
}

// Function to get parameters (placeholder, does nothing for now)
int rp_get_params(rp_app_params_t **p)
{
    return 0; // Return 0 (success), no implementation yet
}

// Function to get signals (placeholder, does nothing for now)
int rp_get_signals(float ***s, int *sig_num, int *sig_len)
{
    return 0; // Return 0 (success), no implementation yet
}

// Empty function to update signals (placeholder)
void UpdateSignals(void) 
{
    // Nothing to do here yet
}

// Empty function to update parameters (placeholder)
void UpdateParams(void) 
{
    // Nothing to do here yet
}

// Function to handle new parameter updates from the user
void OnNewParams(void) 
{   
    // Loop through all 8 LEDs to update their states
    for (int i = 0; i < 8; i++) {
        ledStates[i].Update(); // Update the parameter with any new value from the user
        // Set the LED state on the hardware (RP_LED0 + i maps to LED0 through LED7)
        // If the value is true, set to RP_HIGH (on); if false, set to RP_LOW (off)
        rp_DpinSetState((rp_dpin_t)(RP_LED0 + i), ledStates[i].Value() ? RP_HIGH : RP_LOW);
    }
}

// Empty function to handle new signals (placeholder)
void OnNewSignals(void) 
{
    // Nothing to do here yet
}

// Empty function to handle post-signal updates (placeholder)
void PostUpdateSignals(void) 
{
    // Nothing to do here yet
}