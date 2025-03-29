#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include "main.h"

// Parameters
CBooleanParameter led1State("LED1_STATE", CBaseParameter::RW, false, 0);
CBooleanParameter led2State("LED2_STATE", CBaseParameter::RW, false, 0);
CBooleanParameter led3State("LED3_STATE", CBaseParameter::RW, false, 0);
CBooleanParameter led4State("LED4_STATE", CBaseParameter::RW, false, 0);

const char *rp_app_desc(void)
{
    return (const char *)"Red Pitaya LED control.\n";
}

int rp_app_init(void)
{
    fprintf(stderr, "Loading LED control\n");

    // Initialization of API
    if (rpApp_Init() != RP_OK) 
    {
        fprintf(stderr, "Red Pitaya API init failed!\n");
        return EXIT_FAILURE;
    }
    else fprintf(stderr, "Red Pitaya API init success!\n");

    return 0;
}

int rp_app_exit(void)
{
    fprintf(stderr, "Unloading LED control\n");

    rpApp_Release();

    return 0;
}

int rp_set_params(rp_app_params_t *p, int len)
{
    return 0;
}

int rp_get_params(rp_app_params_t **p)
{
    return 0;
}

int rp_get_signals(float ***s, int *sig_num, int *sig_len)
{
    return 0;
}

void UpdateSignals(void){}

void UpdateParams(void){}

void OnNewParams(void) 
{
    led1State.Update();
    led2State.Update();
    led3State.Update();
    led4State.Update();

    // Set the state for each LED based on its parameter
    rp_DpinSetState(RP_LED1, led1State.Value() ? RP_HIGH : RP_LOW);
    rp_DpinSetState(RP_LED2, led2State.Value() ? RP_HIGH : RP_LOW);
    rp_DpinSetState(RP_LED3, led3State.Value() ? RP_HIGH : RP_LOW);
    rp_DpinSetState(RP_LED4, led4State.Value() ? RP_HIGH : RP_LOW);
}

void OnNewSignals(void){}

void PostUpdateSignals(void){}