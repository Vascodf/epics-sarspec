#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include "math.h"
#include "stdint.h"
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "ftd2xx.h"
#include <math.h> 

using namespace std;


int PSONY = 2048;
int PTOSHIBA = 3648;
double wavelength[3648];
BYTE CCDResult[] = { 0x00 };
BYTE EEPROM_W[64];
BYTE EEPROM_WCMD[] = { 0x0C, 0x02, 0x00 };
BYTE EEPROM_RCMD[] = { 0x0E, 0x03, 0x00 };
BYTE EEPROM_R[64];
BYTE EEPROMPageAddress = 0;
FT_STATUS usbStatus;
FT_HANDLE fthandle;
DWORD Bytestransfered = 0;
FT_HANDLE ftHandleTemp;
DWORD numDevices = 0;
DWORD Flags;
DWORD ID;
DWORD Type;
DWORD LocId;
BYTE ACK[] = { 0 };
BYTE DarkCommand[] = { 0x06, 0x00, 0x00 };
BYTE GainCommand[] = { 0x08, 0x00, 0x00 };
BYTE DelayCommand[] = { 0x16, 0, 0, 0, 0 };
BYTE CCDCommand[] = { 0x0A };
BYTE IncomingData[8192];
BYTE StartScan[] = { 0x02, 0x00 };
BYTE StartScanExt[] = { 0x10, 0, 0, 0, 1 };
BYTE StopScanExt[] = { 0x12 };
BYTE TrigEnableCommand[] = { 0x1A, 1 };
BYTE TrigDisableCommand[] = { 0x1A, 0};
BYTE LEDCommand[] = { 0x18, 0x00 };
char SerialNumber[16];
char Description[64];
int DA_LEVELS = 65536;
int DAC_DARK1 = 56000;
int DAC_DARK2 = 48000;
int MAXLEVEL = 58982; // 65536*90%
int LSONY = 2087;
int LTOSHIBA = 3694;
double GanhoInicial;
uint16_t Samples[4096];
uint16_t DARK = 0;
uint16_t GAIN = 0;
uint32_t Delay = 0;
uint32_t DelayAnterior = 0;
uint32_t TriggerInDelay = 0;
uint32_t calcTimeOut = 0;
uint32_t offsetTimeout = 100; //100ms


//SET DARK
double SetDark(int Dark)
{
	double resposta=0;
	DARK = Dark;
	DarkCommand[1] = (BYTE)(DARK >> 8);
	DarkCommand[2] = (BYTE)(DARK);
	usbStatus = FT_Write(fthandle, DarkCommand, 3, &Bytestransfered);
	if (usbStatus == FT_OK)
	{
		resposta = (double)DARK / (1000.0 * DA_LEVELS / 4096);
	}
	else
	{
		resposta=0;
	}

	return resposta;
}


//SET GAIN
double SetGain(int Gain)
{
	double resposta=0;
	GAIN = Gain;
	GAIN = (uint16_t)(GAIN << 2);
	GainCommand[1] = (BYTE)(GAIN >> 8);
	GainCommand[2] = (BYTE)(GAIN);
	usbStatus = FT_Write(fthandle, GainCommand, 3, &Bytestransfered);
	if (usbStatus == FT_OK)
	{
		//cout<<"Gain Command OK!"<<endl;
		resposta = 6160.0 / (GAIN / 4.0);
	}
	else
	{
		return 0;
	}

	return resposta;
}


//READ EEPROM BY PAGE
char* ReadEEPROMPage(int Page)
{

	char static resposta[64];

	//0<=Page<256
	if (Page > 255)
	{
		return resposta;
	}

	EEPROMPageAddress = Page;
	EEPROM_RCMD[1] = EEPROMPageAddress;
	usbStatus = FT_Write(fthandle, EEPROM_RCMD, 2, &Bytestransfered);
	usbStatus = FT_Read(fthandle, EEPROM_R, 64, &Bytestransfered);

	for (int k = 0; k < 64; k++)
	{	
		if((char)EEPROM_R[k]!=(char)0)
		{
			resposta[k] = (char)EEPROM_R[k];		
		}	
	}
	
	return resposta;
}

//GET INFO
const char* GetDeviceInfo()
{
	char *info = ReadEEPROMPage(118);
	
	//char to string
	string str(info);
	int pos = str.find("#");
	
	//Sub string
	string Substr = str.substr (0,pos);
	const char * resposta = Substr.c_str();
	return resposta;
}

//GET INIT DARK GAIN
double* GetCurrentEEPROMDarkGain()
{
	char *tempstr = ReadEEPROMPage(0);
	string dark,gain;
	double static resposta[2];
	
	//char to string
	string initstr(tempstr);
	
	//split string by #
	istringstream ss(initstr);
	string token;
	for(int i=0;i<6;i++) 
	{
		getline(ss, token, '#');
		if(i==4)
		{
			resposta[0]= atof(token.c_str());
		}
		if(i==5)
		{
			resposta[1]= atof(token.c_str());
		}
	}
	
	return resposta;	
}

//SET DEVICE GAIN BY USER
bool SetDeviceGain(int Gain)
{
	
	if(Gain<1||Gain>500)
	{
		return false;
	}
	
    double ganhoInicialemV;
    
    ganhoInicialemV=6160.0/GanhoInicial;
    Gain--;
    
    double temp = round(6160.0/(Gain + ganhoInicialemV)); 
	
	int tmp = SetGain((int)temp);
	
	if(tmp==0)
	{
		return false;
	}
	else
	{
		return true;
	}
}


//CONNECT DEVICE
int* Connect()
{
	//informacao de estado {(desligado/ligado)(0/1) , (sony/toshiba/nenhum)(0/1/2/99)}
	static int infoEstado[2];
	int i=0;
	
	//numero dispositivos
	usbStatus = FT_CreateDeviceInfoList(&numDevices);

	//se FT_OK
	if (usbStatus == FT_OK)
	{
		//adquire informacao do dispositivo
		usbStatus = FT_GetDeviceInfoDetail(i, &Flags, &Type, &ID, &LocId, SerialNumber, Description, &ftHandleTemp);
	}

	if (usbStatus == FT_OK)
	{
		//abre dispositivo pelo numero de serie
		usbStatus = FT_OpenEx(PVOID(SerialNumber), FT_OPEN_BY_SERIAL_NUMBER, &fthandle);
	}

	//Abertura OK ou nao OK				
	if (usbStatus == FT_OK)
	{ 
		infoEstado[0] = 1;//ligado
	}
	else
	{
		infoEstado[0] = 0;//desligado
	}


	//faz a purga
	usbStatus = FT_Purge(fthandle, FT_PURGE_RX | FT_PURGE_TX);
	if (usbStatus == FT_OK){ /*cout << "Purge = OK"<<endl;*/}

	//carrega timeouts
	usbStatus = FT_SetTimeouts(fthandle, 300000, 300000);
	if (usbStatus == FT_OK){ /*cout << "timeouts = OK"<<endl;*/}

	//write ACK
	usbStatus = FT_Write(fthandle, ACK, sizeof(ACK), &Bytestransfered);
	if (usbStatus == FT_OK){ /*cout << "FT_Write ACK = OK"<<endl;*/}

	//read ACK
	usbStatus = FT_Read(fthandle, ACK, sizeof(ACK), &Bytestransfered);
	if (usbStatus == FT_OK){ /*cout << "FT_Read ACK = OK"<<endl;*/}
			
	//ACK
	if ((ACK[0] & 0xF0) != 0xF0)
	{
		//Error close
		FT_Close(fthandle);	
		infoEstado[1] = 0;
	}

	//ACK OK
	else{


		if (ACK[0] == 0xF0)
		{
			DA_LEVELS = 65536;
			DAC_DARK1 = 56000;
			DAC_DARK2 = 48000;
			MAXLEVEL = 58982;
		}

		if (ACK[0] == 0xFF)
		{
			DA_LEVELS = 4096;
			DAC_DARK1 = 3500;
			MAXLEVEL = 3686;
		}

		ACK[0] = 0;
		DarkCommand[1] = 9;
		DarkCommand[2] = 0;
		usbStatus = FT_Write(fthandle, DarkCommand, 3, &Bytestransfered);
		GainCommand[1] = 0x12;
		GainCommand[2] = 0;
		usbStatus = FT_Write(fthandle, GainCommand, 3, &Bytestransfered);
		usbStatus = FT_Write(fthandle, CCDCommand, 1, &Bytestransfered);
		usbStatus = FT_Read(fthandle, CCDResult, 1, &Bytestransfered);

		if (CCDResult[0] == 0x01)
		{
			infoEstado[1] = 1;
		}

		if (CCDResult[0] == 0x00)
		{
			infoEstado[1] = 0;
		}

		if (CCDResult[0] != 0x01 && CCDResult[0] != 0x00)
		{
			infoEstado[1] = 99;
		}

		//Load dark gain
		double *darkGain = GetCurrentEEPROMDarkGain();
		GanhoInicial=darkGain[1];
		SetDark(darkGain[0]);
		SetGain(darkGain[1]);
	}

	return infoEstado;
		
}


//DISCONNECT DEVICE
bool Disconnect()
{
	bool desligado = false;

	usbStatus = FT_Close(fthandle);

	if (usbStatus == FT_OK)
	{
		desligado = true;
	}
	else
	{
		desligado=false;
	}

	return desligado;
}


//LED ON_OFF
bool LED(bool estado)
{
	bool resposta = false;

	if (estado)
	{
		LEDCommand[1] = 0x01;
	}
	else
	{
		LEDCommand[1] = 0x00;
	}

	usbStatus = FT_Write(fthandle, LEDCommand, 2, &Bytestransfered);
	
	if (usbStatus == FT_OK && estado)
	{
		resposta = true;
	}
	
	if (usbStatus == FT_OK && estado==false)
	{
		resposta = true;
	}
	
	if (usbStatus != FT_OK)
	{
		resposta = false;
	}

	return resposta;
}




//TEMPO INTEGRACAO
bool ChangeIntegrationTime(int IntegrationTime)
{

	if (CCDResult[0] == 0x01) //Toshiba
	{
		if(IntegrationTime<3 || IntegrationTime>214500 ){return false;}
		Delay = (IntegrationTime)*20000.0; //minimo 3
	}
	else //Sony
	{
		if(IntegrationTime<2 || IntegrationTime>214500){return false;}
		Delay = (IntegrationTime)*20000.0; //minimo 1.7
	}

	

	//Usa sempre o delay maior
	if (DelayAnterior < Delay)
	{
		calcTimeOut = (Delay / 20000);
	}
	else
	{
		calcTimeOut = (DelayAnterior / 20000);
	}

	
	//Condicao inicial quando nao se sabe o TI que estava de inicio nao se pode definir o timeout
	if (DelayAnterior == 0)
	{
		usbStatus = FT_SetTimeouts(fthandle, 300000, 300000);
		DelayCommand[1] = (BYTE)(Delay >> 24);
		DelayCommand[2] = (BYTE)(Delay >> 16);
		DelayCommand[3] = (BYTE)(Delay >> 8);
		DelayCommand[4] = (BYTE)(Delay);
		usbStatus = FT_Write(fthandle, DelayCommand, 5, &Bytestransfered); 
	}
	
	//Funcionamento normal em que existe um valor de delay anterior para comprar e se definir um timeout
	if (DelayAnterior != 0)
	{
		uint32_t temp = offsetTimeout + calcTimeOut + TriggerInDelay;
		usbStatus = FT_SetTimeouts(fthandle, temp, temp);
		DelayCommand[1] = (BYTE)(Delay >> 24);
		DelayCommand[2] = (BYTE)(Delay >> 16);
		DelayCommand[3] = (BYTE)(Delay >> 8);
		DelayCommand[4] = (BYTE)(Delay);
		usbStatus = FT_Write(fthandle, DelayCommand, 5, &Bytestransfered);

	}

	//Recolhe delay e define como anterior
	DelayAnterior = Delay;	

	if (usbStatus == FT_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
	
}



//RESTORE EEPROM
bool EEPROMRestore()
{
	
	bool resposta=false;
	
	char *tempstr = ReadEEPROMPage(128);
	string data[6];
	
	//char to string
	string initstr(tempstr);
	
	//split string by #
	istringstream ss(initstr);
	string token;
	for(int i=0;i<6;i++) 
	{
		getline(ss, token, '#');
		data[i]=token;
	}
	
	//String 
	string STR= data[0]+"#"+data[1]+"#"+data[2]+"#"+data[3]+"#"+data[4]+"#"+data[5]+"#";
	
	//String to chart array
	int size = STR.size();
	char charArray[size];
	strncpy(charArray, STR.c_str(), size);

	//creat byte array
	BYTE textoBytes[64];
	std::fill_n(textoBytes, 64, 0);

	for (int k = 0; k < 64; k++)
	{
		if (k < size)
		{
		  textoBytes[k] = (BYTE)charArray[k];
		}
		EEPROM_W[k] = textoBytes[k];
	}

	//Write to EEPROM
	EEPROMPageAddress = 0;
	EEPROM_WCMD[1] = EEPROMPageAddress;
	usbStatus = FT_Write(fthandle, EEPROM_WCMD, 2, &Bytestransfered);
	usbStatus = FT_Write(fthandle, EEPROM_W, 64, &Bytestransfered);

	if (usbStatus == FT_OK)
	{
		resposta = true;
	}
	else 
	{
		resposta = false;
	}

	return resposta;
	
}


//WRITE EEPROM COEFF
bool WriteEEPROMCoeff(double A, double B, double C, double D)
{
    double *darkGain=GetCurrentEEPROMDarkGain();
    
	bool resposta = false;
	int Page=0;
	ostringstream strs;
	
	//Double to string
	strs << A;
	string strA = strs.str();
	strs.str("");
	strs.clear();
	
	strs << B;
	string strB = strs.str();
	strs.str("");
	strs.clear();
	
	strs << C;
	string strC = strs.str();
	strs.str("");
	strs.clear();
	
	strs << D;
	string strD = strs.str();
	strs.str("");
	strs.clear();
	
	strs << darkGain[0];
	string dark = strs.str();
	strs.str("");
	strs.clear();
	
	strs << darkGain[1];
	string gain = strs.str();
	strs.str("");
	strs.clear();
	
	//String
	string STR= strA+"#"+strB+"#"+strC+"#"+strD+"#"+dark+"#"+gain+"#";
	
	//String to chart array
	int size = STR.size();
	char charArray[size];
	strncpy(charArray, STR.c_str(), size);


	//creat byte array
	BYTE textoBytes[64];
	std::fill_n(textoBytes, 64, 0);
	
	for (int k = 0; k < 64; k++)
	{
		if (k < size)
		{
		  textoBytes[k] = (BYTE)charArray[k];
		}
		EEPROM_W[k] = textoBytes[k];
	}

	//Write to EEPROM
	EEPROMPageAddress = Page;
	EEPROM_WCMD[1] = EEPROMPageAddress;
	usbStatus = FT_Write(fthandle, EEPROM_WCMD, 2, &Bytestransfered);
	usbStatus = FT_Write(fthandle, EEPROM_W, 64, &Bytestransfered);

	if (usbStatus == FT_OK)
	{
		resposta = true;
	}
	else
	{
		resposta = false;
	}

	return resposta;

}





//READ EEPROM COEFF
double* ReadEEPROMCoeff()
{
	double static resposta[4];
	double position[5];
	char temp[64];
	int Page=0;

	EEPROMPageAddress = Page;
	EEPROM_RCMD[1] = EEPROMPageAddress;
	usbStatus = FT_Write(fthandle, EEPROM_RCMD, 2, &Bytestransfered);
	usbStatus = FT_Read(fthandle, EEPROM_R, 64, &Bytestransfered);
	
	for (int k = 0; k < 64; k++)
	{	
		if((char)EEPROM_R[k]!=(char)0)
		{
			temp[k] = (char)EEPROM_R[k];		
		}
	}
	
	//char to string
	string str(temp);
	
	//split string by #
	istringstream ss(str);
	string token;
	for(int i=0;i<4;i++) 
	{
		getline(ss, token, '#');
		resposta[i]=atof(token.c_str());
	}
	
	return resposta;
}


//TRIGGER OUT ON/OFF
bool TriggerOutOnOff(int On_Off)
{
	string str;
	
	if (On_Off == 0)
	{
		usbStatus = FT_Write(fthandle, TrigDisableCommand, 2, &Bytestransfered);	
	}
	if (On_Off == 1)
	{
		usbStatus = FT_Write(fthandle, TrigEnableCommand, 2, &Bytestransfered);
	}
		
	if (usbStatus == FT_OK)
	{
		return true;
	}
	else
	{
		return false;
	}

}


//YDATA
double* YData(bool TriggerIN, int Delay )
{
	
	double static m_yData[3648];

	if (TriggerIN)
	{
		//Recolhe valor do delay
		TriggerInDelay=Delay/20000;

		if (TriggerInDelay > 0)
		{
			uint32_t temp = offsetTimeout + calcTimeOut + TriggerInDelay;
			usbStatus = FT_SetTimeouts(fthandle, temp, temp);
			StartScanExt[1] = (BYTE)(TriggerInDelay >> 24);
			StartScanExt[2] = (BYTE)(TriggerInDelay >> 16);
			StartScanExt[3] = (BYTE)(TriggerInDelay >> 8);
			StartScanExt[4] = (BYTE)(TriggerInDelay);
		}

		usbStatus = FT_Write(fthandle, StartScanExt, 5, &Bytestransfered);
		
		if (CCDResult[0] == 0){ usbStatus = FT_Read(fthandle, IncomingData, LSONY << 1, &Bytestransfered); }
		if (CCDResult[0] == 1){ usbStatus = FT_Read(fthandle, IncomingData, LTOSHIBA << 1, &Bytestransfered); }
		usbStatus = FT_Purge(fthandle, FT_PURGE_RX | FT_PURGE_TX);
	}

	if (TriggerIN == false)
	{
		usbStatus = FT_Write(fthandle, StartScan, 1, &Bytestransfered);

		if (CCDResult[0] == 0){ usbStatus = FT_Read(fthandle, IncomingData, LSONY << 1, &Bytestransfered); }
		if (CCDResult[0] == 1){ usbStatus = FT_Read(fthandle, IncomingData, LTOSHIBA << 1, &Bytestransfered); }

		usbStatus = FT_Purge(fthandle, FT_PURGE_RX | FT_PURGE_TX);
	}

	for (int i = 0; i < ((int)Bytestransfered >> 1); i++)
	{
		Samples[i] = (unsigned short)(IncomingData[i << 1] + (IncomingData[(i << 1) + 1] << 8));

		switch (CCDResult[0])
		{

		case  1: //TOSHIBA
			if (i >= 32 && i <= 3679){ m_yData[i - 32] = (double)Samples[i]; }
			break;

		case 0: //SONY
			if (i >= 35 && i <= 2082) { m_yData[i - 35] = (double)Samples[i];}
			break;
		}
	}

	//Desliga USB
	if (usbStatus != FT_OK)
	{
		return 0;
	}

	return m_yData;	
}


//CALC X DATA
double* XData(double interc, double coef1, double coef2, double coef3)
{

	if (CCDResult[0] == 0) //SONY
	{
		double static m_xData[2048];

		for (int i = 0; i < PSONY; i++)
		{
			wavelength[i] = (interc)+(coef1)* i + (coef2)* pow((double)i, 2) + (coef3)* pow((double)i, 3);
			m_xData[i] = wavelength[i];
		}
		
		return m_xData;
	}


	if (CCDResult[0] == 1) //TOSHIBA
	{
		double static m_xData[3648];

		for (int i = 0; i < PTOSHIBA; i++)
		{
			wavelength[i] = (interc)+(coef1)* i + (coef2)* pow((double)i, 2) + (coef3)* pow((double)i, 3);
			m_xData[i] = wavelength[i];

		}

		return m_xData;
	}
	

}


//SET TIMEOUT INDIVIDUAL
bool SetTimeOut(int UserTimeOut)
{
    //Lock 100ms
	if(UserTimeOut<100)
	{
		return false;
	}
	
	//Define offset
	offsetTimeout=UserTimeOut;
	
    //Define timeout
	uint32_t temp = offsetTimeout + calcTimeOut + TriggerInDelay;
	usbStatus = FT_SetTimeouts(fthandle, temp, temp);
	
	if (usbStatus == FT_OK) 
	{ 
		return true;
	}
	else
	{
		return false;
	}	
}


void LibTest()
{
  cout<<"Hello from Sarspec Library"<<endl;
}
