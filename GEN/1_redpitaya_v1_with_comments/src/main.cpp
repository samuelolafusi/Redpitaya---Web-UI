// Include standard C libraries for limits, math, I/O, process control, and system info
#include <limits.h>    // For integer limits (e.g., INT_MAX)
#include <math.h>      // For mathematical functions (sinf, fmodf, etc.)
#include <stdio.h>     // For input/output functions like printf
#include <stdlib.h>    // For general utilities like exit and malloc
#include <unistd.h>    // For POSIX operating system API (e.g., sleep)
#include <sys/types.h> // For data types used in system calls
#include <sys/sysinfo.h> // For system information functions
#include <vector>      // For std::vector container

// Include custom header file (assumed to contain Red Pitaya-specific definitions)
#include "main.h"

// Define constants for signal handling
#define SIGNAL_SIZE_DEFAULT 1024      // Default number of samples in the signal (buffer size)
#define SIGNAL_UPDATE_INTERVAL 10     // Update interval in milliseconds (10ms)
#define SAMPLE_RATE 1000.0f           // Sample rate in Hz (1 kHz for plotting)

// Define a signal for voltage output with 1024 samples, initialized to 0.0
CFloatSignal VOLTAGE("VOLTAGE", SIGNAL_SIZE_DEFAULT, 0.0f);

// Create a vector to store signal data (1024 floats)
std::vector<float> g_data(SIGNAL_SIZE_DEFAULT);

// Define parameters with names matching JavaScript, read/write access, initial values, and ranges
CIntParameter FREQUENCY("FREQUENCY", CBaseParameter::RW, 1, 0, 1, 20);      // Frequency: 1-20 Hz, default 1
CFloatParameter AMPLITUDE("AMPLITUDE", CBaseParameter::RW, 0.5, 0, 0, 0.5); // Amplitude: 0-0.5 V, default 0.5
CIntParameter WAVEFORM("WAVEFORM", CBaseParameter::RW, 0, 0, 0, 2);         // Waveform: 0-2 (sine, sawtooth, square), default 0
CIntParameter GAIN("GAIN", CBaseParameter::RW, 1, 0, 1, 5);                 // Gain: 1-5, default 1

// Function to configure the Red Pitaya signal generator
void set_generator_config()
{
    // Set the frequency for channel 1
    rp_GenFreq(RP_CH_1, FREQUENCY.Value());

    // Set a fixed offset of 0.5V for channel 1
    rp_GenOffset(RP_CH_1, 0.5);

    // Set the amplitude for channel 1
    rp_GenAmp(RP_CH_1, AMPLITUDE.Value());

    // Set the waveform type for channel 1 based on the WAVEFORM parameter
    if (WAVEFORM.Value() == 0) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);    // 0 = Sine wave
    }
    else if (WAVEFORM.Value() == 1) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_RAMP_UP); // 1 = Sawtooth (ramp-up) wave
    }
    else if (WAVEFORM.Value() == 2) {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SQUARE);  // 2 = Square wave
    }
}

// Function to return a description of the application
const char *rp_app_desc(void)
{
    return (const char *)"WebUI Controlled Function Gen.\n"; // Description of the app
}

// Function to initialize the application
int rp_app_init(void)
{
    fprintf(stderr, "Loading generator application\n"); // Print a loading message

    // Initialize the Red Pitaya API
    if (rpApp_Init() != RP_OK) { // Check if initialization fails
        fprintf(stderr, "Red Pitaya API init failed!\n"); // Print error message
        return EXIT_FAILURE; // Exit with failure code
    }
    else {
        fprintf(stderr, "Red Pitaya API init success!\n"); // Print success message
    }

    // Set the signal update interval (10ms)
    CDataManager::GetInstance()->SetSignalInterval(SIGNAL_UPDATE_INTERVAL);

    // Initialize the generator with default settings
    set_generator_config();
    rp_GenOutEnable(RP_CH_1);      // Enable output on channel 1
    rp_GenResetTrigger(RP_CH_1);   // Reset the trigger for channel 1

    return 0; // Return 0 to indicate success
}

// Function to clean up and exit the application
int rp_app_exit(void)
{
    fprintf(stderr, "Unloading generator application\n"); // Print an unloading message

    // Disable the generator output on channel 1
    rp_GenOutDisable(RP_CH_1);

    // Release Red Pitaya API resources
    rpApp_Release();

    return 0; // Return 0 to indicate successful exit
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

// Function to provide signal data to the Red Pitaya framework
int rp_get_signals(float ***s, int *sig_num, int *sig_len) {
    *sig_num = 1;                  // Number of signals (1 signal: VOLTAGE)
    *sig_len = SIGNAL_SIZE_DEFAULT; // Length of the signal (1024 samples)
    *s = (float **)malloc(*sig_num * sizeof(float *)); // Allocate memory for signal pointers
    if (*s == NULL) return -1;     // Return error if allocation fails
    (*s)[0] = (float *)malloc(*sig_len * sizeof(float)); // Allocate memory for signal data
    if ((*s)[0] == NULL) {         // If allocation fails
        free(*s);                  // Free the pointer array
        return -1;                 // Return error
    }
    for (int i = 0; i < *sig_len; i++) { // Copy signal data to the output buffer
        (*s)[0][i] = VOLTAGE[i];
    }
    return 0; // Return 0 to indicate success
}

// Function to update signal data with simulated waveform
void UpdateSignals(void) {
    static float phase = 0.0f;     // Persistent phase for continuous waveform generation
    float freq = FREQUENCY.Value(); // Current frequency value
    float amp = AMPLITUDE.Value() * GAIN.Value(); // Amplitude scaled by gain
    int wave = WAVEFORM.Value();   // Current waveform type
    float offset = 0.5;            // Fixed offset (matches generator config)
    float dt = SIGNAL_UPDATE_INTERVAL / 1000.0f; // Time step (10ms in seconds)
    float samples_per_update = SAMPLE_RATE * dt; // Number of samples per update (e.g., 10 at 1 kHz)

    // Generate new samples for the update interval
    for (int i = 0; i < (int)samples_per_update; i++) {
        float t = phase / freq;    // Time based on phase and frequency
        float val;                 // Calculated value for the sample
        if (wave == 0) {           // Sine wave
            val = offset + amp * sinf(2 * M_PI * freq * t);
        } else if (wave == 1) {    // Sawtooth wave
            float period = fmodf(freq * t, 1.0); // Fractional part of cycle
            val = offset + amp * (2 * period - 1); // Linear ramp from -amp to +amp
        } else {                   // Square wave
            val = offset + amp * (sinf(2 * M_PI * freq * t) > 0 ? 1 : -1); // +amp or -amp
        }
        g_data.erase(g_data.begin()); // Remove the oldest sample
        g_data.push_back(val);        // Add the new sample
        phase += freq * dt / samples_per_update; // Increment phase for next sample
    }

    // Update the VOLTAGE signal with the current buffer
    for (int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) {
        VOLTAGE[i] = g_data[i];
    }
}

// Empty function to update parameters (placeholder)
void UpdateParams(void)
{
    // Nothing to do here yet
}

// Function to handle new parameter updates from the user
void OnNewParams(void)
{
    // Update all parameters with new values from the user
    GAIN.Update();
    FREQUENCY.Update();
    AMPLITUDE.Update();
    WAVEFORM.Update();

    // Apply the updated generator configuration
    set_generator_config();
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