/**
 * Sarspec Linux usb device library test program
 * Project Name:
 * Design Name:
 *
 * Copyright  2024-present IPFN-Instituto Superior Tecnico, Portugal
 * Creation Date  2024-04-20
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>

#include "sarspec-device.h"

using namespace sarspec_usb;

namespace sarspec_usb_test {
    int runTest(int argc, char* argv[]) {
        int rc;
        sarspec_usb::SarspecResDevice teste;
        if(!teste.connect(0x0403, 0x6010)) {
            std::cerr << "Failed to connect device: " << std::endl;
            return EXIT_FAILURE;
        }

        std::vector<double> x;
        std::vector<std::vector<double>> y;

        double coeffs[4] = {0, 1, 0, 0};

        bool a = teste.setDeviceGain(1);
        bool b = teste.setIntegrationTime(10);
        int aqNr = 5;

        x = teste.getXData(coeffs);

        y = teste.getYDataSequence(true, aqNr, 50);

        std::ofstream Data("data.txt");
            
        for (int j = 0; j < aqNr; j++) {

            for (int k = 0; k < 3649; k++) {
                Data << y[j][k] << " ";
                //std::cout << y[j][k] << std::endl;
            }

            Data << "\n";
        }
    
        Data.close();

        //y = teste.getYDataSequence(true, 6, 30);
        //for (int i = 0; i<256; i++) {
        //
        //    printf(teste.ReadEEPROMPage(i));
        //    printf("\n");
        //} Ver se ele guarda no EEPROM

        
        teste.disconnect();

        return EXIT_SUCCESS;
    }
} // namespace sarspec_usb_test

int main(int argc, char* argv[])
{
    return sarspec_usb_test::runTest(argc, argv);
}

// vim: syntax=cpp ts=4 sw=4 sts=4 sr et
