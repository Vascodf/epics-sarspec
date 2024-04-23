/*
 * specAsynPortDriver.cpp
 *
 * Asyn driver that inherits from the asynPortDriver class to demonstrate its use.
 * It simulates a digital scope looking at a 1kHz 1000-point noisy sine wave.  Controls are
 * provided for time/division, volts/division, volt offset, trigger delay, noise amplitude, update time,
 * and run/stop.
 * Readbacks are provides for the waveform data, min, max and mean values.
 *
 * Author: Mark Rivers
 *
 * Created Feb. 5, 2009
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <iocsh.h>

#include "specAsynPortDriver.h"
#include <epicsExport.h>

#define FREQUENCY 1000       /* Frequency in Hz */
#define AMPLITUDE 1.0        /* Plus and minus peaks of sin wave */
#define NUM_DIVISIONS 10     /* Number of scope divisions in X and Y */
#define MIN_UPDATE_TIME 0.02 /* Minimum update time, to prevent CPU saturation */

#define MAX_ENUM_STRING_SIZE 20

static const char *driverName="specAsynPortDriver";
void acquireTask(void *drvPvt);


/** Constructor for the specAsynPortDriver class.
  * Calls constructor for the asynPortDriver base class.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] maxPoints The maximum  number of points in the volt and time arrays */
specAsynPortDriver::specAsynPortDriver(const char *portName, int maxPoints) //MAX POINTS == 3648
   : asynPortDriver(portName,
                    1, /* maxAddr */
                    asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask | asynDrvUserMask, /* Interface mask */
                    asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask,  /* Interrupt mask */
                    0, /* asynFlags.  This driver does not block and it is not multi-device, so flag is 0 */
                    1, /* Autoconnect */
                    0, /* Default priority */
                    0) /* Default stack size*/
{
    asynStatus status;

    const char *functionName = "specAsynPortDriver";
    //int integrationTime = 3;
    //bool temp = false;

    /* Make sure maxPoints is positive */
    if (maxPoints < 1) maxPoints = 3648;

    if(!specDev.connect(0x0403, 0x6010)) {
                        printf("Failed to connect device\n");
                    //return EXIT_FAILURE;
    }
//    temp = ChangeIntegrationTime(integrationTime);

    /* Allocate the YData array */
    pYData_ = (epicsFloat64 *)calloc(3648, sizeof(epicsFloat64));

    /* Allocate the wavelength array */
    pXData_ = (epicsFloat64 *)calloc(3648, sizeof(epicsFloat64));
    
    eventId_ = epicsEventCreate(epicsEventEmpty);
    createParam(P_RunString,                asynParamInt32,         &P_Run);
    createParam(P_MaxPointsString,          asynParamInt32,         &P_MaxPoints);

    createParam(P_TriggerDelayString,       asynParamInt32,         &P_TriggerDelay);

    createParam(P_UpdateTimeString,         asynParamFloat64,       &P_UpdateTime);
    
    
    createParam(P_YDataString,              asynParamFloat64Array,  &P_YData);
    createParam(P_XDataString,              asynParamFloat64Array,  &P_XData);

    /* Set the initial values of some parameters */
    setIntegerParam(P_MaxPoints,         3648);
    setIntegerParam(P_Run,               0);
    setDoubleParam (P_UpdateTime,        0.5);
    /*
    
    Minhas coisas
    
    */

    //createParam(P_RunString,              asynParamInt32,         &P_Run); repetido
    createParam(P_LedString,                asynParamInt32,           &P_Led);
    createParam(P_GainString,               asynParamInt32,           &P_Gain);
    createParam(P_IntTimeString,            asynParamInt32,           &P_IntTime);
    createParam(P_ExtTriggerString,         asynParamInt32,           &P_ExtTrigger);
    createParam(P_Coeff0String,             asynParamFloat64,         &P_Coeff0);
    createParam(P_Coeff1String,             asynParamFloat64,         &P_Coeff1);
    createParam(P_Coeff2String,             asynParamFloat64,         &P_Coeff2);
    createParam(P_Coeff3String,             asynParamFloat64,         &P_Coeff3);

    //setIntegerParam(P_Run, 0); repetido
    setIntegerParam(P_Led, 0);
    setIntegerParam(P_Gain, 1);
    setIntegerParam(P_IntTime, 10);
    setIntegerParam(P_ExtTrigger, 0);
    setIntegerParam(P_TriggerDelay, 0);
    setDoubleParam(P_Coeff0, 0.0);    
    setDoubleParam(P_Coeff1, 1.0);
    setDoubleParam(P_Coeff2, 0.0);
    setDoubleParam(P_Coeff3, 0.0);

    /* Create the thread that computes the waveforms in the background */
    status = (asynStatus)(epicsThreadCreate("specAsynPortDriverTask",
                          epicsThreadPriorityMedium,
                          epicsThreadGetStackSize(epicsThreadStackMedium),
                          (EPICSTHREADFUNC)::acquireTask,
                          this) == NULL);
    if (status) {
        printf("%s:%s: epicsThreadCreate failure\n", driverName, functionName);
        return;
    }
}



void acquireTask(void *drvPvt)
{
    specAsynPortDriver *pPvt = (specAsynPortDriver *)drvPvt;

    pPvt->acquireTask();
}

/** Simulation task that runs as a separate thread.  When the P_Run parameter is set to 1
  * to rub the simulation it computes a 1 kHz sine wave with 1V amplitude and user-controllable
  * noise, and displays it on
  * a simulated scope.  It computes waveforms for the X (time) and Y (volt) axes, and computes
  * statistics about the waveform. */
void specAsynPortDriver::acquireTask(void)
{
    /* This thread computes the waveform and does callbacks with it */

    epicsInt32 run, maxPoints, ext, delay;
    epicsFloat64 updateTime;
    
    getIntegerParam(P_MaxPoints, &maxPoints);
    getIntegerParam(P_ExtTrigger, &ext);
    getIntegerParam(P_TriggerDelay, &delay);
    getIntegerParam(P_Run, &run);
    getDoubleParam(P_UpdateTime, &updateTime);
    
    lock();
    /* Loop forever */
    while (1) {

        // Release the lock while we wait for a command to start or wait for updateTime
        unlock();
        if (run) epicsEventWaitWithTimeout(eventId_, updateTime);
        else     (void) epicsEventWait(eventId_);
        // Take the lock again
        lock();
        /* run could have changed while we were waiting */
        getIntegerParam(P_Run, &run);
        if (!run) continue;
        
        std::vector<double> temp = specDev.getYData(ext, delay);
        for (int i=0; i<3648; i++) {pYData_[i] = temp[i]; /*printf("%u   %g\n", i, temp[i]);*/}

        setIntegerParam(P_Run, 0);

        updateTimeStamp();
        callParamCallbacks();
        doCallbacksFloat64Array(pYData_, 3648, P_YData, 0);
    }
}

/** Called when asyn clients call pasynInt32->write().
  * This function sends a signal to the simTask thread if the value of P_Run has changed.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus specAsynPortDriver::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;
    const char *paramName;
    const char* functionName = "writeInt32";

    /* Set the parameter in the parameter library. */
    status = (asynStatus) setIntegerParam(function, value);

    /* Fetch the parameter string name for possible use in debugging */
    getParamName(function, &paramName);

    if (function == P_Run) {
        /* If run was set then wake up the simulation task */
        if (value) epicsEventSignal(eventId_);
    }
    else if (function == P_Led) {
        setLed();
    }
    else if (function == P_Gain) {
        setGain();
    }
    else if (function == P_IntTime) {
        setIntTime();
    }
    else {
        /* All other parameters just get set in parameter list, no need to
         * act on them here */
    }

    /* Do callbacks so higher layers see any changes */
    status = (asynStatus) callParamCallbacks();

    if (status)
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, name=%s, value=%d",
                  driverName, functionName, status, function, paramName, value);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, name=%s, value=%d\n",
              driverName, functionName, function, paramName, value);
    return status;
}

/** Called when asyn clients call pasynFloat64->write().
  * This function sends a signal to the simTask thread if the value of P_UpdateTime has changed.
  * For all  parameters it  sets the value in the parameter library and calls any registered callbacks.
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus specAsynPortDriver::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;
    epicsInt32 run;
    const char *paramName;
    const char* functionName = "writeFloat64";

    /* Set the parameter in the parameter library. */
    status = (asynStatus) setDoubleParam(function, value);

    /* Fetch the parameter string name for possible use in debugging */
    getParamName(function, &paramName);

    if (function == P_UpdateTime) {
        /* Make sure the update time is valid. If not change it and put back in parameter library */
        if (value < MIN_UPDATE_TIME) {
            asynPrint(pasynUser, ASYN_TRACE_WARNING,
                "%s:%s: warning, update time too small, changed from %f to %f\n",
                driverName, functionName, value, MIN_UPDATE_TIME);
            value = MIN_UPDATE_TIME;
            setDoubleParam(P_UpdateTime, value);
        }
        /* If the update time has changed and we are running then wake up the simulation task */
        getIntegerParam(P_Run, &run);
        if (run) epicsEventSignal(eventId_);
    }
    else if (function == P_Coeff0 || function == P_Coeff1 || function == P_Coeff2 || function == P_Coeff3) {
        setCoeffs();
    }
    else {
        /* All other parameters just get set in parameter list, no need to
         * act on them here */
    }

    /* Do callbacks so higher layers see any changes */
    status = (asynStatus) callParamCallbacks();

    if (status)
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, name=%s, value=%f",
                  driverName, functionName, status, function, paramName, value);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, name=%s, value=%f\n",
              driverName, functionName, function, paramName, value);
    return status;
}


/** Called when asyn clients call pasynFloat64Array->read().
  * Returns the value of the P_Waveform or P_TimeBase arrays.
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Pointer to the array to read.
  * \param[in] nElements Number of elements to read.
  * \param[out] nIn Number of elements actually read. */
asynStatus specAsynPortDriver::readFloat64Array(asynUser *pasynUser, epicsFloat64 *value,
                                         size_t nElements, size_t *nIn)
{
    int function = pasynUser->reason;
    size_t ncopy;
    asynStatus status = asynSuccess;
    epicsTimeStamp timeStamp;
    const char *functionName = "readFloat64Array";

    getTimeStamp(&timeStamp);
    pasynUser->timestamp = timeStamp;
    ncopy = 3648;
    if (nElements < ncopy) ncopy = nElements;
    if (function == P_XData) {
        memcpy(value, pXData_, ncopy*sizeof(epicsFloat64));
        *nIn = ncopy;
    }
    else if (function == P_YData) {
        memcpy(value, pYData_, ncopy*sizeof(epicsFloat64));
        *nIn = ncopy;
    }
    if (status)
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d",
                  driverName, functionName, status, function);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d\n",
              driverName, functionName, function);
    return status;
}

void specAsynPortDriver::setLed()
{
    epicsInt32 led;

    getIntegerParam(P_Led, &led);
    specDev.setLed(led);
}

void specAsynPortDriver::setGain()
{
    epicsInt32 gain;

    getIntegerParam(P_Gain, &gain);
    specDev.setGain(gain);
}

void specAsynPortDriver::setIntTime()
{
    epicsInt32 intTime;

    getIntegerParam(P_IntTime, &intTime);
    specDev.setGain(intTime);
}

void specAsynPortDriver::setCoeffs()
{
    epicsFloat64 c0, c1, c2, c3;

    getDoubleParam(P_Coeff0, &c0);
    getDoubleParam(P_Coeff1, &c1);
    getDoubleParam(P_Coeff2, &c2);
    getDoubleParam(P_Coeff3, &c3);

    pCoeffs_[0] = c0;
    pCoeffs_[1] = c1;
    pCoeffs_[2] = c2;
    pCoeffs_[3] = c3;

    //printf("0: %g 1: %g 2: %g 3: %g\n", pCoeffs_[0], pCoeffs_[1], pCoeffs_[2], pCoeffs_[3]);
    std::vector<double> temp = specDev.getXData(pCoeffs_);
    for (int i=0; i<3648; i++) {pXData_[i] = temp[i]; /*printf("%u   %g\n", i, temp[i]);*/}
        
    doCallbacksFloat64Array(pXData_, 3648, P_XData, 0);
}


/* Configuration routine.  Called directly, or from the iocsh function below */

extern "C" {

/** EPICS iocsh callable function to call constructor for the specAsynPortDriver class.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] maxPoints The maximum  number of points in the volt and time arrays */
int specAsynPortDriverConfigure(const char *portName, int maxPoints)
{
    new specAsynPortDriver(portName, maxPoints);
    return(asynSuccess);
}


/* EPICS iocsh shell commands */

static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg initArg1 = { "max points",iocshArgInt};
static const iocshArg * const initArgs[] = {&initArg0,
                                            &initArg1};
static const iocshFuncDef initFuncDef = {"specAsynPortDriverConfigure",2,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    specAsynPortDriverConfigure(args[0].sval, args[1].ival);
}

void specAsynPortDriverRegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

epicsExportRegistrar(specAsynPortDriverRegister);

}

