/**
 * Sarspec Linux usb device library
 * Project Name:
 * Design Name:
 * Linux Device Driver
 * working  with kernel 5.10
 *
 * Copyright  2024-present IPFN-Instituto Superior Tecnico, Portugal
 * Creation Date  2024-04-20
 */

#include <iostream>
#include <sstream>
#include <cmath>

#include "sarspec-device.h"


#ifdef DEBUG
#endif

namespace sarspec_usb {
    const int DA_LEVELS = 65536;
    const int P = 3648;  //Sony 2048    Toshiba 3648
    const int L = 3694;  //Sony 2087    Toshiba 3694

    SarspecResDevice::SarspecResDevice() : ftdi_dev(NULL)
    {
        if ((ftdi_dev = ftdi_new()) == 0)
        {
            std::cerr << "ftdi_new() failed" << std::endl;
            //std::cerr << 
            //<< std::endl;
        }
        ftdi_set_latency_timer(ftdi_dev, 1);
        ftdi_dev -> usb_read_timeout = 300000;
        ftdi_dev -> usb_write_timeout = 300000;
    }

    SarspecResDevice::~SarspecResDevice()
    {
        //disconnect();
    }

    bool SarspecResDevice::connect(int vendor, int product)
    {
        int status;
        if ((status = ftdi_usb_open(ftdi_dev, vendor, product)) < 0)
        {
            //std::cerr << 
            //<< std::endl;
            std::cerr << "unable to open ftdi device: " << status 
                << ftdi_get_error_string(ftdi_dev) << std::endl;
            ftdi_free(ftdi_dev);
            return false;
        }

        unsigned char bufDark[] = {0x06, 0x00, 0x00};
        unsigned char bufGain[] = {0x08, 0x00, 0x00};

        ftdi_tcioflush(ftdi_dev);

        bufDark[1] = 9;
        bufDark[2] = 0;
        status = ftdi_write_data(ftdi_dev, bufDark, 3);
        if (status <= 0) return false;

        bufGain[1] = 0x12;
        bufGain[2] = 0;
        status = ftdi_write_data(ftdi_dev, bufGain, 3);
        if (status <= 0) return false;

        getCurrentEEPROMDarkGain();
        gain0 = darkGain[1];

        double d = setDark(darkGain[0]);
        double g = setGain(darkGain[1]);
        if (d == 0 || g == 0) return false;

        return true;
    }
    int SarspecResDevice::disconnect()
    {
        int status;
        if ((status = ftdi_usb_close(ftdi_dev)) < 0)
        {
            std::cerr << "unable to close ftdi device: " << status 
                << ftdi_get_error_string(ftdi_dev) << std::endl;
            //fprintf(stderr, "unable to close ftdi device: %d (%s)\n", status, ftdi_get_error_string(ftdi_dev));
            ftdi_free(ftdi_dev);
        }

        ftdi_free(ftdi_dev);

        return status;
    }
    int SarspecResDevice::setLed(bool led)
    {
        int status;
        unsigned char bufLED[] = {0x18, 0x00};

        if (led) {
            bufLED[1] = 0x01;
        }

        status = ftdi_write_data(ftdi_dev, bufLED, 2);
        if (status < 0)
        {
            std::cerr << "write failed for: " << status
                << ftdi_get_error_string(ftdi_dev) << std::endl;
            //fprintf(stderr,"write failed for 0x%x, error %d (%s)\n",bufLED[0],status, ftdi_get_error_string(ftdi_dev));

            return status;
        }
        else
        {
            printf("ftdi LED write succeeded: %d\n",status);
        }

        return status;
    }

    std::vector<double> SarspecResDevice::getXData(double coeffs[4])
    {
        xData.clear();
        for (int i = 0; i < P; i++)
        {
            xData.push_back(coeffs[0] + coeffs[1] * i + coeffs[2] * pow(double(i), 2) + coeffs[3] * pow(double(i), 3));
        }
        return xData;
    }

    std::vector<double> SarspecResDevice::getYData(bool extTrigger, int delay) //delay em ms/20000 ??
    {
        int status;
        unsigned char bufStartScan[] = { 0x02, 0x00 };
        unsigned char bufStartScanExt[] = { 0x10, 0, 0, 0, 1 };
        unsigned char incomingBuf[8192];
        uint32_t bytesRead;
        yData.clear();

        timespec startTime, endTime;

        if (extTrigger) {

            tExtDelay = delay/20000;

            if (tExtDelay > 0) {

                uint32_t temp = offsetTimeout + calcTimeOut + tExtDelay;

                bufStartScanExt[1] = (unsigned char)(tExtDelay >> 24);
                bufStartScanExt[2] = (unsigned char)(tExtDelay >> 16);
                bufStartScanExt[3] = (unsigned char)(tExtDelay >> 8);
                bufStartScanExt[4] = (unsigned char)(tExtDelay);
            }

            status = ftdi_write_data(ftdi_dev, bufStartScanExt, 5);
            tc = ftdi_read_data_submit(ftdi_dev, incomingBuf, L << 1);
            ftdi_transfer_data_done(tc);

            if (tc->size > 0) {
                bytesRead = tc->size;
            }
            else {
                return std::vector<double> {0};
            }

            ftdi_tcioflush(ftdi_dev);
        }
        else {

            status = ftdi_write_data(ftdi_dev, bufStartScan, 1);
            tc = ftdi_read_data_submit(ftdi_dev, incomingBuf, L << 1);
            ftdi_transfer_data_done(tc);

            if (tc->size > 0) {
                bytesRead = tc->size;
            }
            else {
                return std::vector<double> {0};
            }

            ftdi_tcioflush(ftdi_dev);
        }

        uint16_t temp[7388];

        for (int i = 0; i < int(bytesRead >> 1); i++) {

            temp[i] = (uint16_t)(incomingBuf[i << 1] | (incomingBuf[(i << 1) + 1] << 8));

            if (i >= 32 && i <= 3679) {
                yData.push_back(double(temp[i]));
            }
        }

        return yData;
    }

    bool SarspecResDevice::setIntegrationTime(int intTime)
    {
        int status;

        if(intTime < 3 || intTime > 214500){return false;}

        uint32_t delay = (uint32_t)((double)intTime * 20000.0);

        if (delay_old < delay) {
            calcTimeOut = delay / 20000;
        }
        else {
            calcTimeOut = delay_old / 20000;
        }

        if (delay_old == 0) {

            ftdi_dev -> usb_read_timeout = 300000;
            ftdi_dev -> usb_write_timeout = 300000;
        }
        else {

            uint32_t temp = offsetTimeout + calcTimeOut + tExtDelay;
            ftdi_dev -> usb_read_timeout = temp;
            ftdi_dev -> usb_write_timeout = temp;    
        }

        unsigned char bufDelay[] = {0x16, 0, 0, 0, 0};

        bufDelay[1] = (unsigned char)(delay >> 24);
        bufDelay[2] = (unsigned char)(delay >> 16);
        bufDelay[3] = (unsigned char)(delay >> 8);
        bufDelay[4] = (unsigned char)(delay);

        status = ftdi_write_data(ftdi_dev, bufDelay, 5);
        delay_old = delay;

        if (status > 0) {return true;}

        return false;
    }

    bool SarspecResDevice::setTimeout(int timeOut)
    {
        int status;
        if (timeOut < 100) {return false;}

        offsetTimeout = timeOut;

        uint32_t temp = offsetTimeout + calcTimeOut + tExtDelay;
        ftdi_dev -> usb_read_timeout = temp;
        ftdi_dev -> usb_write_timeout = temp;

        if (status == 0) {return true;}
        return false;
    }

    char* SarspecResDevice::readEEPROMPage(int page) 
    {
        int status;
        static char ret[64];
        unsigned char bufEEPROM_RCMD[] = { 0x0E, 0x03, 0x00 };
        unsigned char bufEEPROM_R[64];
        unsigned char pageaddr;

        //0<=Page<256
        if (page > 255)
        {
            return ret;
        }
        pageaddr = (unsigned char)page;
        bufEEPROM_RCMD[1] = pageaddr;

        status = ftdi_write_data(ftdi_dev, bufEEPROM_RCMD, 2);
        tc = ftdi_read_data_submit(ftdi_dev, bufEEPROM_R, 64);
        ftdi_transfer_data_done(tc);

        for (int k = 0; k < 64; k++)
        {
            if((char)bufEEPROM_R[k] != (char)0)
            {
                ret[k] = (char)bufEEPROM_R[k];
            }
        }
        ftdi_tcioflush(ftdi_dev);

        return ret;
    }

    double SarspecResDevice::setDark(int dark)
    {
        int status;
        uint16_t DARK = dark;
        unsigned char bufDark[] = {0x06, 0x00, 0x00};

        bufDark[1] = (unsigned char)(DARK >> 8);
        bufDark[2] = (unsigned char)(DARK);

        status = ftdi_write_data(ftdi_dev, bufDark, 3);

        if (status > 0) {
            return double(DARK) / (1000.0 * DA_LEVELS / 4096);
        }

        return 0;
    }

    double SarspecResDevice::setGain(int gain)
    {
        int status;
        uint16_t GAIN = gain;

        GAIN = (uint16_t)(GAIN << 2);

        unsigned char bufGain[] = {0x08, 0x00, 0x00};

        bufGain[1] = (unsigned char)(GAIN >> 8);
        bufGain[2] = (unsigned char)(GAIN);

        status = ftdi_write_data(ftdi_dev, bufGain, 3);

        if (status > 0) {
            return 6160.0 / (GAIN / 4.0);
        }

        return 0;
    }

    double* SarspecResDevice::getCurrentEEPROMDarkGain()
    {
        char *tempstr = readEEPROMPage(0);

        //char to string
        std::string initstr(tempstr);

        //split string by #
        std::istringstream ss(initstr);
        std::string token;
        for(int i=0;i<6;i++) 
        {
            getline(ss, token, '#');
            if(i==4)
            {
                darkGain[0]= atof(token.c_str());
            }
            if(i==5)
            {
                darkGain[1]= atof(token.c_str());
            }
        }

        return darkGain;	
    }

    bool SarspecResDevice::setDeviceGain(int gain)
    {
        if (gain < 1 || gain > 500) {
            return false;
        }

        double gain0inV;

        gain0inV = 6160.0/gain0;
        gain--;

        double temp = std::round(6160.0/(gain + gain0inV));

        int tmp = setGain(int(temp));

        if(tmp==0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

} // namespace sarspec_usb
// vim: syntax=cpp ts=4 sw=4 sts=4 sr et

