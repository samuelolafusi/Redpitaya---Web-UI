// Include standard C libraries for limits, math, I/O, process control, and system info
#include <limits.h>    // For integer limits like INT_MAX
#include <math.h>      // For math functions like sinf
#include <stdio.h>     // For printing messages
#include <stdlib.h>    // For utilities like malloc and exit
#include <unistd.h>    // For POSIX system calls
#include <sys/types.h> // For system data types
#include <sys/sysinfo.h> // For system info functions
#include <vector>      // For using std::vector

#include "main.h"      // Include Red Pitaya-specific header

// lots of stuffs here are stated in the redpitaya ecosystem files compatible with my OS version .....
// ...... Version - RedPitaya_OS_2.05-37
// example codes in Redpitaya_Ecosystem\RedPitaya-master\Examples also gave insights
// check redpitaya github for the ecosystem version for RedPitaya_OS_2.05-37
// see files Redpitaya_Ecosystem\RedPitaya-master\rp-api\api\include and also ..\src

//Signal size
#define SIGNAL_SIZE_DEFAULT      1024 // Number of samples in signal buffer
#define SIGNAL_UPDATE_INTERVAL 10     // 10ms updates - How often signals update
#define SAMPLE_RATE 1000.0f           // 1 kHz for plotting (adjustable) - Samples per second

//Signal
CFloatSignal VOLTAGE_CH1("VOLTAGE_CH1", SIGNAL_SIZE_DEFAULT, 0.0f); // Signal buffer for Channel 1
CFloatSignal VOLTAGE_CH2("VOLTAGE_CH2", SIGNAL_SIZE_DEFAULT, 0.0f); // Signal buffer for Channel 2
std::vector<float> g_data_ch1(SIGNAL_SIZE_DEFAULT); // Vector to hold Channel 1 data
std::vector<float> g_data_ch2(SIGNAL_SIZE_DEFAULT); // Vector to hold Channel 2 data

// paramter_type parametername_in_c("parameter_name_in_js", CBaseParameter::RW, initial_value, 0, min_value, max_value);
// initial value, min value, max value must correspond to html and Redpitaya device capabilities
// note, max output voltage of baord is -1V/+1V, total (offset + amplitude) will be clipped if above the limit.

// Parameters for channel 1
CIntParameter FREQUENCY("FREQUENCY", CBaseParameter::RW, 0, 0, 0, 50000000);      // Frequency: 0-50 MHz, starts at 0
CFloatParameter AMPLITUDE("AMPLITUDE", CBaseParameter::RW, 0.0, 0, 0.0, 1.0);      // Amplitude: 0-1 V, starts at 0
CIntParameter WAVEFORM("WAVEFORM", CBaseParameter::RW, 0, 0, 0, 5);                 // Waveform: 0-5 types, starts at 0
CIntParameter PHASE("PHASE", CBaseParameter::RW, 0, 0, 0, 360);                     // Phase: 0-360 degrees, starts at 0
CFloatParameter OFFSET("OFFSET", CBaseParameter::RW, 0.0, 0, -1, 1);                // Offset: -1 to 1 V, starts at 0
CIntParameter CHANNEL_STATE_1("CHANNEL_STATE_1", CBaseParameter::RW, 0, 0, 0, 1);  // Channel 1 on/off: 0=off, 1=on, starts off

CIntParameter GAIN("GAIN", CBaseParameter::RW, 0, 0, 0, 5);                         // Gain: 0-5, starts at 0

// Parameters for channel 2
CIntParameter FREQUENCY1("FREQUENCY1", CBaseParameter::RW, 0, 0, 0, 50000000);     // Frequency: 0-50 MHz, starts at 0
CFloatParameter AMPLITUDE1("AMPLITUDE1", CBaseParameter::RW, 0.0, 0, 0.0, 1.0);    // Amplitude: 0-1 V, starts at 0
CIntParameter WAVEFORM1("WAVEFORM1", CBaseParameter::RW, 0, 0, 0, 5);              // Waveform: 0-5 types, starts at 0
CIntParameter PHASE1("PHASE1", CBaseParameter::RW, 0, 0, 0, 360);                   // Phase: 0-360 degrees, starts at 0
CFloatParameter OFFSET1("OFFSET1", CBaseParameter::RW, 0.0, 0, -1, 1);             // Offset: -1 to 1 V, starts at 0
CIntParameter CHANNEL_STATE_2("CHANNEL_STATE_2", CBaseParameter::RW, 0, 0, 0, 1); // Channel 2 on/off: 0=off, 1=on, starts off

// Generator config
void set_generator_config() // Set up the signal generator for both channels
{
    // These formats/keyword/structure depending on what you want to create, are specified in RedPitaya-master\rp-api\api\include and also ..\src ......
    // ........ in the ecosystem for RedPitaya_OS_2.05-37 on Github
    // note the names of the local parameters for both channels in JS as this is the same name for parameters here in CPP

    rp_GenFreq(RP_CH_1, FREQUENCY.Value());   // Set Channel 1 frequency
    rp_GenOffset(RP_CH_1, OFFSET.Value());    // Set Channel 1 offset
    rp_GenAmp(RP_CH_1, AMPLITUDE.Value());    // Set Channel 1 amplitude
    rp_GenPhase(RP_CH_1, PHASE.Value());      // Set Channel 1 phase

    //Set waveform
    // WAVEFORM value 0 represents SINE WAVE from html, value number must match stated waveform type
    switch (WAVEFORM.Value()) { // Choose Channel 1 waveform based on value
        case 0: rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE); break;      // 0 = Sine wave
        case 1: rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SQUARE); break;    // 1 = Square wave
        case 2: rp_GenWaveform(RP_CH_1, RP_WAVEFORM_TRIANGLE); break;  // 2 = Triangle wave
        case 3: rp_GenWaveform(RP_CH_1, RP_WAVEFORM_RAMP_UP); break;   // 3 = Ramp up
        case 4: rp_GenWaveform(RP_CH_1, RP_WAVEFORM_RAMP_DOWN); break; // 4 = Ramp down
    }

    rp_GenFreq(RP_CH_2, FREQUENCY1.Value());  // Set Channel 2 frequency
    rp_GenOffset(RP_CH_2, OFFSET1.Value());   // Set Channel 2 offset
    rp_GenAmp(RP_CH_2, AMPLITUDE1.Value());   // Set Channel 2 amplitude
    rp_GenPhase(RP_CH_2, PHASE1.Value());     // Set Channel 2 phase
    switch (WAVEFORM1.Value()) { // Choose Channel 2 waveform
        case 0: rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SINE); break;
        case 1: rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SQUARE); break;
        case 2: rp_GenWaveform(RP_CH_2, RP_WAVEFORM_TRIANGLE); break;
        case 3: rp_GenWaveform(RP_CH_2, RP_WAVEFORM_RAMP_UP); break;
        case 4: rp_GenWaveform(RP_CH_2, RP_WAVEFORM_RAMP_DOWN); break;
    }
}

// APP description
const char *rp_app_desc(void) // Describe the application
{
    return (const char *)"WebUI Controlled Function Gen.\n"; // Simple description
}

//APP initialization
int rp_app_init(void) // Start the application
{
    fprintf(stderr, "Loading generator application\n"); // Print loading message

    // Initialization of API
    if (rpApp_Init() != RP_OK) // Try to start Red Pitaya API
    {
        fprintf(stderr, "Red Pitaya API init failed!\n"); // Print error if failed
        return EXIT_FAILURE; // Exit with error code
    }
    else fprintf(stderr, "Red Pitaya API init success!\n"); // Print success message

    for (int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) { // Fill initial signal buffers
        g_data_ch1[i] = OFFSET.Value();  // Set Channel 1 to initial offset
        g_data_ch2[i] = OFFSET1.Value(); // Set Channel 2 to initial offset
        VOLTAGE_CH1[i] = g_data_ch1[i];  // Copy to Channel 1 signal
        VOLTAGE_CH2[i] = g_data_ch2[i];  // Copy to Channel 2 signal
    }

    //Set signal update interval
    CDataManager::GetInstance()->SetSignalInterval(SIGNAL_UPDATE_INTERVAL); // Update every 10ms

    // Init generator
    // Here, the toggle state from JS determines the activation of channels
    set_generator_config(); // Apply initial settings

    if (CHANNEL_STATE_1.Value() == 1) { // If Channel 1 is on
        rp_GenOutEnable(RP_CH_1);    // Turn on output
        rp_GenResetTrigger(RP_CH_1); // Reset trigger
    } else {
        rp_GenOutDisable(RP_CH_1);   // Turn off output
    }

    if (CHANNEL_STATE_2.Value() == 1) { // If Channel 2 is on
        rp_GenOutEnable(RP_CH_2);
        rp_GenResetTrigger(RP_CH_2);
    } else {
        rp_GenOutDisable(RP_CH_2);
    }

    return 0; // Return success
}

int rp_app_exit(void) // Stop the application
{
    fprintf(stderr, "Unloading generator application\n"); // Print unloading message

    rp_GenOutDisable(RP_CH_1); // Turn off Channel 1
    rp_GenOutDisable(RP_CH_2); // Turn off Channel 2

    rpApp_Release(); // Release Red Pitaya resources

    return 0; // Return success
}

int rp_set_params(rp_app_params_t *p, int len) // Placeholder for setting parameters
{
    return 0; // Return success (not implemented)
}

int rp_get_params(rp_app_params_t **p) // Placeholder for getting parameters
{
    return 0; // Return success (not implemented)
}

int rp_get_signals(float ***s, int *sig_num, int *sig_len) { // Send signals to JavaScript
    *sig_num = CHANNEL_STATE_1.Value() + CHANNEL_STATE_2.Value(); // Count active channels
    if (*sig_num == 0) { // If no channels are active
        *sig_num = 1; // Send at least one signal to avoid crash
        *sig_len = SIGNAL_SIZE_DEFAULT; // Set signal length
        *s = (float **)malloc(*sig_num * sizeof(float *)); // Allocate memory
        (*s)[0] = (float *)malloc(*sig_len * sizeof(float)); // Allocate signal data
        for (int i = 0; i < *sig_len; i++) (*s)[0][i] = 0.0f; // Fill with zeros
        return 0; // Return success
    }

    *sig_len = SIGNAL_SIZE_DEFAULT; // Set signal length
    *s = (float **)malloc(*sig_num * sizeof(float *)); // Allocate memory for signals
    int idx = 0; // Index for active signals
    if (CHANNEL_STATE_1.Value()) { // If Channel 1 is active
        (*s)[idx] = (float *)malloc(*sig_len * sizeof(float)); // Allocate memory
        for (int i = 0; i < *sig_len; i++) (*s)[idx][i] = VOLTAGE_CH1[i]; // Copy Channel 1 data
        idx++; // Move to next signal
    }
    if (CHANNEL_STATE_2.Value()) { // If Channel 2 is active
        (*s)[idx] = (float *)malloc(*sig_len * sizeof(float));
        for (int i = 0; i < *sig_len; i++) (*s)[idx][i] = VOLTAGE_CH2[i]; // Copy Channel 2 data
    }
    return 0; // Return success
}

void UpdateSignals(void) { // Update signal data for plotting
    static float phase1 = PHASE.Value(); // Keep track of Channel 1 phase
    static float phase2 = PHASE1.Value(); // Keep track of Channel 2 phase
    float dt = SIGNAL_UPDATE_INTERVAL / 1000.0f; // Time step (10ms in seconds)
    int samples_per_update = (int)(SAMPLE_RATE * dt + 0.5f); // Number of samples per update

    if (CHANNEL_STATE_1.Value()) { // If Channel 1 is on
        float freq = FREQUENCY.Value(); // Get frequency
        float amp = AMPLITUDE.Value() * (GAIN.Value() + 1); // Apply gain (+1 to avoid zero)
        int wave = WAVEFORM.Value(); // Get waveform type
        float offset = OFFSET.Value(); // Get offset
        for (int i = 0; i < samples_per_update; i++) { // Generate new samples
            float t = phase1 / (freq > 0 ? freq : 1); // Calculate time, avoid divide by zero
            float val; // Value to add to buffer
            switch (wave) { // Calculate signal value based on waveform
                case 0: val = offset + amp * sinf(2 * M_PI * freq * t); break; // Sine wave
                case 1: val = offset + amp * (sinf(2 * M_PI * freq * t) > 0 ? 1 : -1); break; // Square wave
                case 2: case 3: case 4: { // Triangle, ramp up, ramp down (simplified)
                    float period = fmodf(freq * t, 1.0); // Get position in cycle
                    val = offset + amp * (2 * period - 1); // Linear ramp
                    break;
                }
                default: val = offset; // Default to offset for others
            }
            g_data_ch1.erase(g_data_ch1.begin()); // Remove oldest sample
            g_data_ch1.push_back(val); // Add new sample
            phase1 += freq * dt / samples_per_update; // Update phase
        }
        for (int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) VOLTAGE_CH1[i] = g_data_ch1[i]; // Update signal buffer
    }

    if (CHANNEL_STATE_2.Value()) { // If Channel 2 is on
        float freq = FREQUENCY1.Value();
        float amp = AMPLITUDE1.Value() * (GAIN.Value() + 1);
        int wave = WAVEFORM1.Value();
        float offset = OFFSET1.Value();
        for (int i = 0; i < samples_per_update; i++) {
            float t = phase2 / (freq > 0 ? freq : 1);
            float val;
            switch (wave) {
                case 0: val = offset + amp * sinf(2 * M_PI * freq * t); break;
                case 1: val = offset + amp * (sinf(2 * M_PI * freq * t) > 0 ? 1 : -1); break;
                case 2: case 3: case 4: {
                    float period = fmodf(freq * t, 1.0);
                    val = offset + amp * (2 * period - 1);
                    break;
                }
                default: val = offset;
            }
            g_data_ch2.erase(g_data_ch2.begin());
            g_data_ch2.push_back(val);
            phase2 += freq * dt / samples_per_update;
        }
        for (int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) VOLTAGE_CH2[i] = g_data_ch2[i];
    }
}

void UpdateParams(void) { // Placeholder for parameter updates
}

void OnNewParams(void) // Update parameters when user changes them
{   
    // this part updates the cpp paramters based on new inputs from user
    GAIN.Update(); // Update gain value
    
    FREQUENCY.Update(); // Update Channel 1 frequency
    AMPLITUDE.Update(); // Update Channel 1 amplitude
    PHASE.Update();     // Update Channel 1 phase
    WAVEFORM.Update();  // Update Channel 1 waveform
    OFFSET.Update();    // Update Channel 1 offset
    CHANNEL_STATE_1.Update(); // Update Channel 1 state

    FREQUENCY1.Update(); // Update Channel 2 frequency
    AMPLITUDE1.Update(); // Update Channel 2 amplitude
    PHASE1.Update();     // Update Channel 2 phase
    WAVEFORM1.Update();  // Update Channel 2 waveform
    OFFSET1.Update();    // Update Channel 2 offset
    CHANNEL_STATE_2.Update(); // Update Channel 2 state

    // Set generator config
    set_generator_config(); // Apply new settings to generator

    // Dynamically enable/disable channels based on UI toggle
    if (CHANNEL_STATE_1.Value() == 1) { // If Channel 1 is on
        rp_GenOutEnable(RP_CH_1);
        rp_GenResetTrigger(RP_CH_1);
    } else {
        rp_GenOutDisable(RP_CH_1);
    }

    if (CHANNEL_STATE_2.Value() == 1) { // If Channel 2 is on
        rp_GenOutEnable(RP_CH_2);
        rp_GenResetTrigger(RP_CH_2);
    } else {
        rp_GenOutDisable(RP_CH_2);
    }
}

void OnNewSignals(void) { // Handle new signals (placeholder)
    // VOLTAGE.Update(); // Mark signal as updated - Original comment preserved
}

void PostUpdateSignals(void) { // Placeholder for post-signal updates
}