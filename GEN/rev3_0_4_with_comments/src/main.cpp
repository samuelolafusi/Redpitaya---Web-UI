// Include standard C libraries for limits, math, I/O, process control, and system info
#include <limits.h>    // For integer limits like INT_MAX
#include <math.h>      // For math functions
#include <stdio.h>     // For printing messages
#include <stdlib.h>    // For utilities like exit
#include <unistd.h>    // For POSIX system calls
#include <sys/types.h> // For system data types
#include <sys/sysinfo.h> // For system info functions
#include <vector>      // For using std::vector

// Include Red Pitaya-specific header file
#include "main.h"

// Notes:
// - This code uses the Red Pitaya ecosystem for OS version 2.05-37.
// - Check RedPitaya-master\Examples for examples.
// - See RedPitaya-master\rp-api\api\include and ..\src on GitHub for details.

// Define signal constants
#define SIGNAL_SIZE_DEFAULT 1024  // Number of samples in the signal buffer
#define SIGNAL_UPDATE_INTERVAL 1  // Update signals every 1 millisecond

// Define a signal buffer for voltage with 1024 samples, starting at 0.0
CFloatSignal VOLTAGE("VOLTAGE", SIGNAL_SIZE_DEFAULT, 0.0f);

// Create a vector to hold 1024 float values for signal data
std::vector<float> g_data(SIGNAL_SIZE_DEFAULT);

// Define parameters for Channel 1 (names match JavaScript)
CIntParameter FREQUENCY("FREQUENCY", CBaseParameter::RW, 0, 0, 1, 50000000);      // Frequency: 1 Hz to 50 MHz, starts at 0
CFloatParameter AMPLITUDE("AMPLITUDE", CBaseParameter::RW, 0.0, 0, 0.1, 1);        // Amplitude: 0.1 V to 1 V, starts at 0
CIntParameter WAVEFORM("WAVEFORM", CBaseParameter::RW, 0, 0, 0, 9);                 // Waveform: 0-9 types, starts at 0 (sine)
CIntParameter PHASE("PHASE", CBaseParameter::RW, 0, 0, 1, 360);                     // Phase: 1-360 degrees, starts at 0
CFloatParameter OFFSET("OFFSET", CBaseParameter::RW, 0, 0, 0.1, 2);                 // Offset: 0.1 V to 2 V, starts at 0
CIntParameter CHANNEL_STATE_1("CHANNEL_STATE_1", CBaseParameter::RW, 0, 0, 0, 1);  // Channel 1 on/off: 0 = off, 1 = on, starts off
CIntParameter GAIN("GAIN", CBaseParameter::RW, 1, 0, 1, 1);                         // Gain: fixed at 1

// Define parameters for Channel 2 (names match JavaScript with "1" suffix)
CIntParameter FREQUENCY1("FREQUENCY1", CBaseParameter::RW, 0, 0, 1, 50000000);     // Frequency: 1 Hz to 50 MHz, starts at 0
CFloatParameter AMPLITUDE1("AMPLITUDE1", CBaseParameter::RW, 0.0, 0, 0.1, 1);      // Amplitude: 0.1 V to 1 V, starts at 0
CIntParameter WAVEFORM1("WAVEFORM1", CBaseParameter::RW, 0, 0, 0, 9);              // Waveform: 0-9 types, starts at 0 (sine)
CIntParameter PHASE1("PHASE1", CBaseParameter::RW, 0, 0, 1, 360);                   // Phase: 1-360 degrees, starts at 0
CFloatParameter OFFSET1("OFFSET1", CBaseParameter::RW, 0, 0, 0.1, 2);              // Offset: 0.1 V to 2 V, starts at 0
CIntParameter CHANNEL_STATE_2("CHANNEL_STATE_2", CBaseParameter::RW, 0, 0, 0, 1); // Channel 2 on/off: 0 = off, 1 = on, starts off
CIntParameter GAIN1("GAIN1", CBaseParameter::RW, 1, 0, 1, 1);                      // Gain: fixed at 1

// Function to set up the signal generator for both channels
void set_generator_config()
{
    // Configure Channel 1
    rp_GenFreq(RP_CH_1, FREQUENCY.Value());   // Set frequency from JavaScript value
    rp_GenOffset(RP_CH_1, OFFSET.Value());    // Set offset from JavaScript value
    rp_GenAmp(RP_CH_1, AMPLITUDE.Value());    // Set amplitude from JavaScript value
    rp_GenPhase(RP_CH_1, PHASE.Value());      // Set phase from JavaScript value

    // Choose waveform for Channel 1 based on value from JavaScript
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
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_ARBITRARY); // 8 = Arbitrary wave (noise)
    }

    // Configure Channel 2
    rp_GenFreq(RP_CH_2, FREQUENCY1.Value());  // Set frequency
    rp_GenOffset(RP_CH_2, OFFSET1.Value());   // Set offset
    rp_GenAmp(RP_CH_2, AMPLITUDE1.Value());   // Set amplitude
    rp_GenPhase(RP_CH_2, PHASE1.Value());     // Set phase

    // Choose waveform for Channel 2
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
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_ARBITRARY); // 8 = Arbitrary wave (noise)
    }
}

// Function to describe the app
const char *rp_app_desc(void)
{
    return (const char *)"WebUI Controlled Function Gen.\n"; // Short description
}

// Function to start the app
int rp_app_init(void)
{
    fprintf(stderr, "Loading generator application\n"); // Print loading message

    // Start the Red Pitaya API
    if (rpApp_Init() != RP_OK) { // If it fails
        fprintf(stderr, "Red Pitaya API init failed!\n"); // Show error
        return EXIT_FAILURE; // Exit with error
    } else {
        fprintf(stderr, "Red Pitaya API init success!\n"); // Show success
    }

    // Set how often signals update (1ms)
    CDataManager::GetInstance()->SetSignalInterval(SIGNAL_UPDATE_INTERVAL);

    // Set up the generator with current settings
    set_generator_config();

    // Turn Channel 1 on or off based on toggle state
    if (CHANNEL_STATE_1.Value() == 1) {
        rp_GenOutEnable(RP_CH_1);    // Turn on output
        rp_GenResetTrigger(RP_CH_1); // Reset trigger
    } else {
        rp_GenOutDisable(RP_CH_1);   // Turn off output
    }

    // Turn Channel 2 on or off based on toggle state
    if (CHANNEL_STATE_2.Value() == 1) {
        rp_GenOutEnable(RP_CH_2);
        rp_GenResetTrigger(RP_CH_2);
    } else {
        rp_GenOutDisable(RP_CH_2);
    }

    return 0; // Success
}

// Function to stop the app
int rp_app_exit(void)
{
    fprintf(stderr, "Unloading generator application\n"); // Print unloading message

    rp_GenOutDisable(RP_CH_1); // Turn off Channel 1
    rp_GenOutDisable(RP_CH_2); // Turn off Channel 2

    rpApp_Release(); // Free up Red Pitaya resources

    return 0; // Success
}

// Placeholder to set parameters (not used yet)
int rp_set_params(rp_app_params_t *p, int len)
{
    return 0; // Return success (empty function)
}

// Placeholder to get parameters (not used yet)
int rp_get_params(rp_app_params_t **p)
{
    return 0; // Return success (empty function)
}

// Placeholder to get signals (not used yet)
int rp_get_signals(float ***s, int *sig_num, int *sig_len)
{
    return 0; // Return success (empty function)
}

// Function to update signal data
void UpdateSignals(void)
{
    float val;      // Store analog input value
    uint32_t raw;   // Store raw analog input value

    // Only update if at least one channel is on
    if (CHANNEL_STATE_1.Value() == 1 || CHANNEL_STATE_2.Value() == 1) {
        rp_AIpinGetValue(0, &val, &raw); // Read voltage from analog pin 0

        // Use gain from the active channel
        float gain_factor = 1.0; // Default gain
        if (CHANNEL_STATE_1.Value() == 1) {
            gain_factor = GAIN.Value(); // Channel 1 gain
        } else if (CHANNEL_STATE_2.Value() == 1) {
            gain_factor = GAIN1.Value(); // Channel 2 gain
        }

        g_data.erase(g_data.begin()); // Remove oldest value
        g_data.push_back(val * gain_factor); // Add new value with gain

        // Copy data to the VOLTAGE signal
        for (int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) {
            VOLTAGE[i] = g_data[i];
        }
    }
    // If no channels are on, do nothing (signal stays as is)
}

// Empty function for parameter updates (placeholder)
void UpdateParams(void)
{
    // Nothing here yet
}

// Function to update settings when user changes them
void OnNewParams(void)
{
    // Update Channel 1 settings from JavaScript
    FREQUENCY.Update();
    AMPLITUDE.Update();
    PHASE.Update();
    WAVEFORM.Update();
    CHANNEL_STATE_1.Update();

    // Update Channel 2 settings from JavaScript
    FREQUENCY1.Update();
    AMPLITUDE1.Update();
    PHASE1.Update();
    WAVEFORM1.Update();
    CHANNEL_STATE_2.Update();

    // Apply the new settings to the generator
    set_generator_config();

    // Turn channels on or off based on toggle state
    if (CHANNEL_STATE_1.Value() == 1) {
        rp_GenOutEnable(RP_CH_1);
        rp_GenResetTrigger(RP_CH_1);
    } else {
        rp_GenOutDisable(RP_CH_1);
    }

    if (CHANNEL_STATE_2.Value() == 1) {
        rp_GenOutEnable(RP_CH_2);
        rp_GenResetTrigger(RP_CH_2);
    } else {
        rp_GenOutDisable(RP_CH_2);
    }
}

// Empty function for new signals (placeholder)
void OnNewSignals(void)
{
    // Nothing here yet
}

// Empty function for post-signal updates (placeholder)
void PostUpdateSignals(void)
{
    // Nothing here yet
}