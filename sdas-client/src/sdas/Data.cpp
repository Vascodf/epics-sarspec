/*
 * Data.cpp
 *
 * Created on March 1, 2006, 00:00 AM
 *
 * <p>Title: SDAS</p>
 *
 * <p>Description: Shared Data Access System</p>
 *
 * <p>Copyright: (C) Copyright 2005-2006, by Centro de Fusao Nuclear
 *
 * Project Info:  	http://baco.cfn.ist.utl.pt/sdas
 *                      http://www.cfn.ist.utl.pt
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * [Java is a trademark or registered trademark of Sun Microsystems, Inc.
 * in the United States and other countries.]
 * </p>
 *
 * <p>Company: CFN - EURATOM/IST- http://www.cfn.ist.utl.pt</p>
 *
 * Author: Daniel Filipe Valcarcel
 * Version: 1.0.0
 *
 */

#include <iostream>
#include "Data.h"

// Constructors
Data::Data(void)
{
    this->setParameterUniqueID("");
	this->setTimeStart(TimeStamp());
	this->setTimeEnd(TimeStamp());
	this->setMimeType("");
	this->setEvents(vector<HEvent>());
	this->setRawData(vector<char>());
    this->data_type = "";
    this->extra_info = map<string, string>();
    this->units = "";
    this->transfer_function = "x";
}

Data::Data(string uniqueID, TimeStamp tStart, TimeStamp tEnd, string mimeType, vector<HEvent> evt, vector<char> data)
{
	this->setParameterUniqueID(uniqueID);
	this->setTimeStart(tStart);
	this->setTimeEnd(tEnd);
	this->setMimeType(mimeType);
	this->setEvents(evt);
	this->setRawData(data);
    this->data_type = "";
    this->extra_info = map<string, string>();
    this->units = "";
    this->transfer_function = "x";
}

// Destructor
Data::~Data(void)
{
}

string Data::getParameterUniqueID()
{
	return this->parameterUniqueID;
}

TimeStamp Data::getTimeStart()
{
	return this->tstart;
}

TimeStamp Data::getTimeEnd()
{
	return this->tend;
}

string Data::getMimeType()
{
	return this->mime_type;
}

vector<HEvent> Data::getEvents()
{
	return this->events;
}

vector<char> Data::getRawData()
{
	return this->raw_data;
}

vector<short> Data::getInt16Data()
{
	bool isInt16 = false;
	bool isLittleEndian = false;
	vector<short> int16Data;

	if(this->mime_type.find("short") != -1)
		isInt16 = true;
	if(this->mime_type.find("_le") != -1)
		isLittleEndian = true;

	if(isInt16)
	{
		unsigned int num1 = 0;
		unsigned int num0 = 0;
		unsigned short numInt16 = 0;

		for(int i = 0 ; i < this->raw_data.size() ; i += 2)
		{
			if(isLittleEndian)
			{
				num1 = (((unsigned short) this->raw_data[i + 1]) << 8) & 0xFF00;
				num0 = ((unsigned short) this->raw_data[i]) & 0x00FF;
			}
			else
			{
				num1 = (((unsigned short) this->raw_data[i]) << 8) & 0xFF00;
				num0 = ((unsigned short) this->raw_data[i + 1]) & 0x00FF;
			}
			numInt16 = num1 | num0;
			int16Data.push_back(numInt16);
		}
	}

	return int16Data;
}

vector<int> Data::getInt32Data()
{
	bool isInt32 = false;
	bool isLittleEndian = false;
	vector<int> int32Data;

	if(this->mime_type.find("int") != -1)
		isInt32 = true;
	if(this->mime_type.find("_le") != -1)
		isLittleEndian = true;

	if(isInt32)
	{
		unsigned int num3 = 0;
		unsigned int num2 = 0;
		unsigned int num1 = 0;
		unsigned int num0 = 0;
		unsigned int numInt = 0;
		float numInt32 = 0;

		for(int i = 0 ; i < this->raw_data.size() ; i += 4)
		{
			if(isLittleEndian)
			{
				num3 = (((unsigned int) this->raw_data[i + 3]) << 24) & 0xFF000000;
				num2 = (((unsigned int) this->raw_data[i + 2]) << 16) & 0x00FF0000;
				num1 = (((unsigned int) this->raw_data[i + 1]) << 8) & 0x0000FF00;
				num0 = ((unsigned int) this->raw_data[i]) & 0x000000FF;
			}
			else
			{
				num3 = (((unsigned int) this->raw_data[i]) << 24) & 0xFF000000;
				num2 = (((unsigned int) this->raw_data[i + 1]) << 16) & 0x00FF0000;
				num1 = (((unsigned int) this->raw_data[i + 2]) << 8) & 0x0000FF00;
				num0 = ((unsigned int) this->raw_data[i + 3]) & 0x000000FF;
			}
			numInt32 = num3 | num2 | num1 | num0;
			int32Data.push_back(numInt32);
		}
	}

	return int32Data;
}

vector<float> Data::getFloat32Data()
{
	bool isFloat32 = false;
	bool isLittleEndian = false;
	vector<float> float32Data;

	if(this->mime_type.find("float") != -1)
		isFloat32 = true;
	if(this->mime_type.find("_le") != -1)
		isLittleEndian = true;

	if(isFloat32)
	{
		unsigned int num3 = 0;
		unsigned int num2 = 0;
		unsigned int num1 = 0;
		unsigned int num0 = 0;
		unsigned int numInt = 0;
		float numFloat32 = 0;

		for(int i = 0 ; i < this->raw_data.size() /*- (this->raw_data.size() % 4)*/ ; i += 4)
		{
			if(isLittleEndian)
			{
				num3 = (((unsigned int) this->raw_data[i + 3]) << 24) & 0xFF000000;
				num2 = (((unsigned int) this->raw_data[i + 2]) << 16) & 0x00FF0000;
				num1 = (((unsigned int) this->raw_data[i + 1]) << 8) & 0x0000FF00;
				num0 = ((unsigned int) this->raw_data[i]) & 0x000000FF;
			}
			else
			{
				num3 = (((unsigned int) this->raw_data[i]) << 24) & 0xFF000000;
				num2 = (((unsigned int) this->raw_data[i + 1]) << 16) & 0x00FF0000;
				num1 = (((unsigned int) this->raw_data[i + 2]) << 8) & 0x0000FF00;
				num0 = ((unsigned int) this->raw_data[i + 3]) & 0x000000FF;
			}
			numInt = num3 | num2 | num1 | num0;
			numFloat32 = *((float *) &numInt);
			float32Data.push_back(numFloat32);
		}
	}

	return float32Data;
}

vector<double> Data::getFloat64Data()
{
	bool isFloat64 = false;
	bool isLittleEndian = false;
	vector<double> float64Data;

	if(this->mime_type.find("double") != -1)
		isFloat64 = true;
	if(this->mime_type.find("_le") != -1)
		isLittleEndian = true;

	if(isFloat64)
	{
		unsigned int num7 = 0;
		unsigned int num6 = 0;
		unsigned int num5 = 0;
		unsigned int num4 = 0;
		unsigned int num3 = 0;
		unsigned int num2 = 0;
		unsigned int num1 = 0;
		unsigned int num0 = 0;
		unsigned int numInt[2] = { 0, 0 };
		double numFloat64 = 0;

		for(int i = 0 ; i < this->raw_data.size() /*- (this->raw_data.size() % 4)*/ ; i += 8)
		{
			if(isLittleEndian)
			{
				num7 = (((unsigned int) this->raw_data[i + 3]) << 24) & 0xFF000000;
				num6 = (((unsigned int) this->raw_data[i + 2]) << 16) & 0x00FF0000;
				num5 = (((unsigned int) this->raw_data[i + 1]) << 8) & 0x0000FF00;
				num4 = ((unsigned int) this->raw_data[i]) & 0x000000FF;
				num3 = (((unsigned int) this->raw_data[i + 3]) << 24) & 0xFF000000;
				num2 = (((unsigned int) this->raw_data[i + 2]) << 16) & 0x00FF0000;
				num1 = (((unsigned int) this->raw_data[i + 1]) << 8) & 0x0000FF00;
				num0 = ((unsigned int) this->raw_data[i]) & 0x000000FF;
			}
			else
			{
				num7 = (((unsigned int) this->raw_data[i]) << 24) & 0xFF000000;
				num6 = (((unsigned int) this->raw_data[i + 1]) << 16) & 0x00FF0000;
				num5 = (((unsigned int) this->raw_data[i + 2]) << 8) & 0x0000FF00;
				num4 = ((unsigned int) this->raw_data[i + 3]) & 0x000000FF;
				num3 = (((unsigned int) this->raw_data[i]) << 24) & 0xFF000000;
				num2 = (((unsigned int) this->raw_data[i + 1]) << 16) & 0x00FF0000;
				num1 = (((unsigned int) this->raw_data[i + 2]) << 8) & 0x0000FF00;
				num0 = ((unsigned int) this->raw_data[i + 3]) & 0x000000FF;
			}
			numInt[1] = num7 | num6 | num5 | num4;
			numInt[0] = num3 | num2 | num1 | num0;
			numFloat64 = *((double *) numInt);
			float64Data.push_back(numFloat64);
		}
	}

	return float64Data;
}

string Data::getDataType()
{
    return this->data_type;
}

map<string, string> Data::getExtraInfo()
{
    return this->extra_info;
}

string Data::getUnits()
{
    return this->units;
}

string Data::getTransferFunction()
{
    return this->transfer_function;
}

ErrorInfo Data::getErrorInfo(){
    return this->error_info;
}

void Data::setParameterUniqueID(string uniqueID)
{
	this->parameterUniqueID = uniqueID;
}

void Data::setTimeStart(TimeStamp tStart)
{
	this->tstart = tStart;
}

void Data::setTimeEnd(TimeStamp tEnd)
{
	this->tend = tEnd;
}

void Data::setMimeType(string mimeType)
{
	this->mime_type = mimeType;
}

void Data::setEvents(vector<HEvent> evt)
{
	this->events = evt;
}

void Data::setExtraInfo(map<string, string> extraInfo)
{
    this->extra_info = extraInfo;
}

void Data::setErrorInfo(ErrorInfo errorInfo)
{
    this->error_info = errorInfo;
}

void Data::setRawData(vector<char> data)
{
	this->raw_data = data;
}
