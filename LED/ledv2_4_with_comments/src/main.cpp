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

// Define parameters for controlling the state of 4 LEDs
// Each has a name (e.g., "LED1_STATE"), is read/write (RW), defaults to false, and version 0
CBooleanParameter led1State("LED1_STATE", CBaseParameter::RW, false, 0);
CBooleanParameter led2State("LED2_STATE", CBaseParameter::RW, false, 0);
CBooleanParameter led3State("LED3_STATE", CBaseParameter::RW, false, 0);
CBooleanParameter led4State("LED4_STATE", CBaseParameter::RW, false, 0);

// Function to return a description of the application
const char *rp_app_desc(void)
{
    return (const char *)"Red Pitaya LED control.\n"; // Simple description of the app
}

// Function to initialize the application
int rp_app_init(void)
{
    fprintf(stderr, "Loading LED control\n"); // Print a message to indicate loading

    // Initialize the Red Pitaya API
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

    rpApp_Release(); // Release resources used by the Red Pitaya API

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

// Function to handle new parameter updates
void OnNewParams(void) 
{
    // Update all LED state parameters with any new values
    led1State.Update();
    led2State.Update();
    led3State.Update();
    led4State.Update();

    // Set the state for each LED based on its parameter value
    // If true, set to RP_HIGH (on); if false, set to RP_LOW (off)
    rp_DpinSetState(RP_LED1, led1State.Value() ? RP_HIGH : RP_LOW);
    rp_DpinSetState(RP_LED2, led2State.Value() ? RP_HIGH : RP_LOW);
    rp_DpinSetState(RP_LED3, led3State.Value() ? RP_HIGH : RP_LOW);
    rp_DpinSetState(RP_LED4, led4State.Value() ? RP_HIGH : RP_LOW);
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