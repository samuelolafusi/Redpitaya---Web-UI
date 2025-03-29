// Include standard C libraries for limits, math, I/O, process control, and system info
#include <limits.h>    // For integer limits (e.g., INT_MAX)
#include <math.h>      // For mathematical functions
#include <stdio.h>     // For input/output functions like printf
#include <stdlib.h>    // For general utilities like exit
#include <unistd.h>    // For POSIX operating system API (e.g., sleep)
#include <sys/types.h> // For data types used in system calls
#include <sys/sysinfo.h> // For system information functions
#include <vector>      // For std::vector container

// Include custom header file (assumed to contain Red Pitaya-specific definitions)
#include "main.h"

// Define constants for signal handling
#define SIGNAL_SIZE_DEFAULT 1024  // Default number of samples in the signal
#define SIGNAL_UPDATE_INTERVAL 1  // Signal update interval in milliseconds

// Define a signal for voltage output with 1024 samples, initialized to 0.0
CFloatSignal VOLTAGE("VOLTAGE", SIGNAL_SIZE_DEFAULT, 0.0f);

// Create a vector to store signal data (1024 floats)
std::vector<float> g_data(SIGNAL_SIZE_DEFAULT);

// Define parameters for Channel 1 (names match JavaScript)
CIntParameter FREQUENCY("FREQUENCY", CBaseParameter::RW, 0, 0, 1, 50000000);      // Frequency: 1-50 MHz, default 0
CFloatParameter AMPLITUDE("AMPLITUDE", CBaseParameter::RW, 0.0, 0, 0.1, 1);        // Amplitude: 0.1-1 V, default 0
CIntParameter WAVEFORM("WAVEFORM", CBaseParameter::RW, 0, 0, 0, 9);                 // Waveform: 0-9 types, default 0
CIntParameter GAIN("GAIN", CBaseParameter::RW, 1, 0, 1, 1);                         // Gain: fixed at 1
CIntParameter CHANNEL_STATE_1("CHANNEL_STATE_1", CBaseParameter::RW, 0, 0, 0, 1);  // Channel 1 state: 0 (off) or 1 (on), default 0

// Define parameters for Channel 2 (names match JavaScript with "1" suffix)
CIntParameter FREQUENCY1("FREQUENCY1", CBaseParameter::RW, 0, 0, 1, 50000000);     // Frequency: 1-50 MHz, default 0
CFloatParameter AMPLITUDE1("AMPLITUDE1", CBaseParameter::RW, 0.0, 0, 0.1, 1);      // Amplitude: 0.1-1 V, default 0
CIntParameter WAVEFORM1("WAVEFORM1", CBaseParameter::RW, 0, 0, 0, 9);              // Waveform: 0-9 types, default 0
CIntParameter GAIN1("GAIN1", CBaseParameter::RW, 1, 0, 1, 1);                      // Gain: fixed at 1
CIntParameter CHANNEL_STATE_2("CHANNEL_STATE_2", CBaseParameter::RW, 0, 0, 0, 1); // Channel 2 state: 0 (off) or 1 (on), default 0

// Function to configure the Red Pitaya signal generator for both channels
void set_generator_config()
{
    // Configure Channel 1
    rp_GenFreq(RP_CH_1, FREQUENCY.Value());   // Set frequency for Channel 1
    rp_GenOffset(RP_CH_1, 0.5);               // Set fixed offset of 0.5V
    rp_GenAmp(RP_CH_1, AMPLITUDE.Value());    // Set amplitude for Channel 1

    // Set waveform type for Channel 1 based on WAVEFORM value
    if (WAVEFORM.Value() == 0) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);     // 0 = Sine wave
    } else if (WAVEFORM.Value() == 1) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SQUARE);   // 1 = Square wave
    } else if (WAVEFORM.Value() == 2) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_TRIANGLE); // 2 = Triangle wave
    } else if (WAVEFORM.Value() == 3) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_RAMP_UP);  // 3 = Ramp up wave
    } else if (WAVEFORM.Value() == 4) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_RAMP_DOWN); // 4 = Ramp down wave
    } else if (WAVEFORM.Value() == 5) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_DC);       // 5 = DC positive
    } else if (WAVEFORM.Value() == 6) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_DC_NEG);   // 6 = DC negative
    } else if (WAVEFORM.Value() == 7) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_PWM);      // 7 = PWM wave
    } else if (WAVEFORM.Value() == 8) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_ARBITRARY); // 8 = Arbitrary wave
    }

    // Configure Channel 2
    rp_GenFreq(RP_CH_2, FREQUENCY1.Value());  // Set frequency for Channel 2
    rp_GenOffset(RP_CH_2, 0.5);               // Set fixed offset of 0.5V
    rp_GenAmp(RP_CH_2, AMPLITUDE1.Value());   // Set amplitude for Channel 2

    // Set waveform type for Channel 2 based on WAVEFORM1 value
    if (WAVEFORM1.Value() == 0) {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SINE);     // 0 = Sine wave
    } else if (WAVEFORM1.Value() == 1) {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SQUARE);   // 1 = Square wave
    } else if (WAVEFORM1.Value() == 2) {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_TRIANGLE); // 2 = Triangle wave
    } else if (WAVEFORM1.Value() == 3) {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_RAMP_UP);  // 3 = Ramp up wave
    } else if (WAVEFORM1.Value() == 4) {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_RAMP_DOWN); // 4 = Ramp down wave
    } else if (WAVEFORM1.Value() == 5) {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_DC);       // 5 = DC positive
    } else if (WAVEFORM1.Value() == 6) {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_DC_NEG);   // 6 = DC negative
    } else if (WAVEFORM1.Value() == 7) {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_PWM);      // 7 = PWM wave
    } else if (WAVEFORM1.Value() == 8) {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_ARBITRARY); // 8 = Arbitrary wave
    }
}

// Function to return a description of the application
const char *rp_app_desc(void)
{
    return (const char *)"WebUI Controlled Function Gen.\n"; // Simple app description
}

// Function to initialize the application
int rp_app_init(void)
{
    fprintf(stderr, "Loading generator application\n"); // Print loading message

    // Initialize the Red Pitaya API
    if (rpApp_Init() != RP_OK) { // Check if initialization fails
        fprintf(stderr, "Red Pitaya API init failed!\n"); // Print error message
        return EXIT_FAILURE; // Exit with failure code
    } else {
        fprintf(stderr, "Red Pitaya API init success!\n"); // Print success message
    }

    // Set the signal update interval (1ms)
    CDataManager::GetInstance()->SetSignalInterval(SIGNAL_UPDATE_INTERVAL);

    // Apply initial generator settings
    set_generator_config();

    // Enable or disable Channel 1 based on its state
    if (CHANNEL_STATE_1.Value() == 1) {
        rp_GenOutEnable(RP_CH_1);    // Turn on Channel 1 output
        rp_GenResetTrigger(RP_CH_1); // Reset trigger for Channel 1
    } else {
        rp_GenOutDisable(RP_CH_1);   // Turn off Channel 1 output
    }

    // Enable or disable Channel 2 based on its state
    if (CHANNEL_STATE_2.Value() == 1) {
        rp_GenOutEnable(RP_CH_2);    // Turn on Channel 2 output
        rp_GenResetTrigger(RP_CH_2); // Reset trigger for Channel 2
    } else {
        rp_GenOutDisable(RP_CH_2);   // Turn off Channel 2 output
    }

    return 0; // Return 0 to indicate success
}

// Function to clean up and exit the application
int rp_app_exit(void)
{
    fprintf(stderr, "Unloading generator application\n"); // Print unloading message

    // Disable both channels
    rp_GenOutDisable(RP_CH_1); // Turn off Channel 1
    rp_GenOutDisable(RP_CH_2); // Turn off Channel 2

    rpApp_Release(); // Release Red Pitaya API resources

    return 0; // Return 0 to indicate success
}

// Placeholder function to set parameters (not implemented)
int rp_set_params(rp_app_params_t *p, int len)
{
    return 0; // Return 0 (success), no implementation yet
}

// Placeholder function to get parameters (not implemented)
int rp_get_params(rp_app_params_t **p)
{
    return 0; // Return 0 (success), no implementation yet
}

// Placeholder function to get signals (not implemented)
int rp_get_signals(float ***s, int *sig_num, int *sig_len)
{
    return 0; // Return 0 (success), no implementation yet
}

// Function to update signal data
void UpdateSignals(void)
{
    float val;      // Variable for analog input value
    uint32_t raw;   // Variable for raw analog input value

    // Check if either channel is enabled
    if (CHANNEL_STATE_1.Value() == 1 || CHANNEL_STATE_2.Value() == 1) {
        rp_AIpinGetValue(0, &val, &raw); // Read voltage from analog input pin 0

        // Apply gain based on which channel is active
        float gain_factor = 1.0; // Default gain factor
        if (CHANNEL_STATE_1.Value() == 1) {
            gain_factor = GAIN.Value(); // Use Channel 1 gain if active
        } else if (CHANNEL_STATE_2.Value() == 1) {
            gain_factor = GAIN1.Value(); // Use Channel 2 gain if active
        }

        g_data.erase(g_data.begin()); // Remove oldest sample
        g_data.push_back(val * gain_factor); // Add new sample with gain

        // Update the VOLTAGE signal with current data
        for (int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) {
            VOLTAGE[i] = g_data[i];
        }
    }
    // If no channel is enabled, no action is taken (signal remains unchanged)
}

// Empty function to update parameters (placeholder)
void UpdateParams(void)
{
    // Nothing to do here yet
}

// Function to handle new parameter updates from the user
void OnNewParams(void)
{
    // Update all parameters with new values from the UI
    GAIN.Update();
    FREQUENCY.Update();
    AMPLITUDE.Update();
    WAVEFORM.Update();
    CHANNEL_STATE_1.Update();

    GAIN1.Update();
    FREQUENCY1.Update();
    AMPLITUDE1.Update();
    WAVEFORM1.Update();
    CHANNEL_STATE_2.Update();

    // Apply updated generator settings
    set_generator_config();

    // Enable or disable channels based on their state
    if (CHANNEL_STATE_1.Value() == 1) {
        rp_GenOutEnable(RP_CH_1);    // Turn on Channel 1
        rp_GenResetTrigger(RP_CH_1); // Reset trigger
    } else {
        rp_GenOutDisable(RP_CH_1);   // Turn off Channel 1
    }

    if (CHANNEL_STATE_2.Value() == 1) {
        rp_GenOutEnable(RP_CH_2);    // Turn on Channel 2
        rp_GenResetTrigger(RP_CH_2); // Reset trigger
    } else {
        rp_GenOutDisable(RP_CH_2);   // Turn off Channel 2
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