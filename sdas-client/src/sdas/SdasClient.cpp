/*
 * SdasClient.cpp
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
#include <cstring>
#include <map>
#include "XmlRpc.h"

#include "SdasClient.h"

using namespace std;
using namespace XmlRpc;
using namespace org::sdas::core::client;

// Constructors...

SdasClient::SdasClient(string hostname, int port, string cred)
{
	string credentials[2] = {"", ""};

	if (cred != "") {
		
		size_t p = cred.find(",");
		credentials[0] = cred.substr(0, p);
		credentials[1] = cred.substr(p+1, cred.length() - 1);
	}

	this->client = new XmlRpcClient(hostname.c_str(), port);
    
    this->sessionId = this->authenticate(credentials);
}

// Destructor
SdasClient::~SdasClient(void)
{
	this->client->close();
	delete this->client;
}

// ****************************************************************************
// *                           Conversion Functions                           *
// ****************************************************************************

XmlRpcValue SdasClient::createXmlRpcValueFromDate(Date date)
{
	XmlRpcValue rpcValue;

	rpcValue["year"] = date.getYear();
	rpcValue["month"] = date.getMonth();
	rpcValue["day"] = date.getDay();

	return rpcValue;
}

XmlRpcValue SdasClient::createXmlRpcValueFromTime(Time time)
{
	XmlRpcValue rpcValue;

	rpcValue["hours"] = time.getHours();
	rpcValue["minutes"] = time.getMinutes();
	rpcValue["seconds"] = time.getSeconds();
	rpcValue["millis"] = time.getMillis();
	rpcValue["micros"] = time.getMicros();
	rpcValue["nanos"] = time.getNanos();
	rpcValue["picos"] = time.getPicos();

	return rpcValue;
}

XmlRpcValue SdasClient::createXmlRpcValueFromTimeStamp(TimeStamp tstamp)
{
	XmlRpcValue rpcValue;

	rpcValue["date"] = this->createXmlRpcValueFromDate(tstamp.getDate());
	rpcValue["time"] = this->createXmlRpcValueFromTime(tstamp.getTime());

	return rpcValue;
}

XmlRpcValue SdasClient::createXmlRpcValueFromHEvent(HEvent hevent)
{
	XmlRpcValue rpcValue;

	rpcValue["evtNumber"] = hevent.getEventNumber();
	rpcValue["uniqueID"] = hevent.getUniqueID();
	rpcValue["tstamp"] = this->createXmlRpcValueFromTimeStamp(hevent.getTimeStamp());

	return rpcValue;
}

XmlRpcValue SdasClient::createXmlRpcValueFromExtraInfo(map<string,string> extraInfo)
{
	XmlRpcValue rpcValue;

	for (const auto& x : extraInfo)
        rpcValue[x.first] = x.second;
    if (extraInfo.size() == 0)
        rpcValue[""] = "";

	return rpcValue;
}

XmlRpcValue SdasClient::createXmlRpcValueFromErrorInfo(ErrorInfo errori)
{
	XmlRpcValue rpcValue;

	rpcValue["errorCode"] = errori.getCode();
	rpcValue["errorText"] = errori.getText();

	return rpcValue;
}

XmlRpcValue SdasClient::createXmlRpcValueFromData(Data data)
{
	XmlRpcValue rpcValue;

	rpcValue["parameterUniqueID"] = data.getParameterUniqueID();
	rpcValue["tstart"] = this->createXmlRpcValueFromTimeStamp(data.getTimeStart());
	rpcValue["tend"] = this->createXmlRpcValueFromTimeStamp(data.getTimeEnd());
	rpcValue["mime_type"] = data.getMimeType();
	vector<HEvent> evt = data.getEvents();
	int eventSize = evt.size();
	rpcValue["events"].setSize(eventSize);
	for(int i = 0 ; i < eventSize ; i++)
		rpcValue["events"][i] = this->createXmlRpcValueFromHEvent(evt[i]);
    rpcValue["raw_data"] = XmlRpcValue(data.getRawData().data(), data.getRawData().size());
    rpcValue["mime_type"] = data.getMimeType();
    rpcValue["data_type"] = data.getDataType();
    rpcValue["extra_info"] = createXmlRpcValueFromExtraInfo(data.getExtraInfo());
    rpcValue["units"] = data.getUnits();
    rpcValue["transfer_function"] = data.getTransferFunction();
    rpcValue["error_info"] = this->createXmlRpcValueFromErrorInfo(data.getErrorInfo());

	return rpcValue;
}

Date SdasClient::createDateStructureFromXmlRpcValue(XmlRpcValue rpcValue)
{
	Date date(rpcValue["year"], rpcValue["month"], rpcValue["day"]);

	return date;
}

Time SdasClient::createTimeStructureFromXmlRpcValue(XmlRpcValue rpcValue)
{
	Time time((char)(int) rpcValue["hours"],
				(char)(int) rpcValue["minutes"],
				(char)(int) rpcValue["seconds"],
				(short)(int) rpcValue["millis"],
				(short)(int) rpcValue["micros"],
				(short)(int) rpcValue["nanos"],
				(short)(int) rpcValue["picos"]);

	return time;
}

TimeStamp SdasClient::createTimeStampStructureFromXmlRpcValue(XmlRpcValue rpcValue)
{
	TimeStamp tstamp(this->createDateStructureFromXmlRpcValue(rpcValue["date"]),
						this->createTimeStructureFromXmlRpcValue(rpcValue["time"]));

	return tstamp;
}

DescriptorUID SdasClient::createDescriptorUidStructureFromXmlRpcValue(XmlRpcValue rpcValue)
{
	DescriptorUID descriptorUID(rpcValue["name"], rpcValue["description"], rpcValue["locale"], rpcValue["uniqueID"]);

	return descriptorUID;
}

HEvent SdasClient::createHEventStructureFromXmlRpcValue(XmlRpcValue rpcValue)
{
	HEvent hevent(rpcValue["uniqueID"],
					rpcValue["evtNumber"],
					this->createTimeStampStructureFromXmlRpcValue(rpcValue["tstamp"]));

	return hevent;
}

ErrorInfo SdasClient::createErrorInfoStructureFromXmlRpcValue(XmlRpcValue rpcValue)
{
    ErrorInfo errorInfo(rpcValue["code"], rpcValue["text"]);
    
    return errorInfo;
}

//
// Creates a Parameter object from a XmlRpcValue object
// TODO: acrescentar a extra_info
//
Parameter SdasClient::createParameterStructureFromXmlRpcValue(XmlRpcValue rpcValue)
{
	DescriptorUID descriptorUID = this->createDescriptorUidStructureFromXmlRpcValue(rpcValue["descriptorUID"]);
	map<string, string> extraInfo; // Ignorar este campo por agora
	Parameter parameter(descriptorUID, rpcValue["mime_type"], extraInfo);

	return parameter;
}

Data SdasClient::createDataStructureFromXmlRpcValue(XmlRpcValue rpcValue)
{
	string parameterUniqueID = rpcValue["parameterUniqueID"];
	TimeStamp tstart = this->createTimeStampStructureFromXmlRpcValue(rpcValue["tstart"]);
	TimeStamp tend = this->createTimeStampStructureFromXmlRpcValue(rpcValue["tend"]);
	string mime_type = rpcValue["mime_type"];
	vector<HEvent> events;
	vector<char> raw_data = rpcValue["raw_data"];

	for(int i = 0 ; i < rpcValue["events"].size() ; i++)
		events.push_back(this->createHEventStructureFromXmlRpcValue(rpcValue["events"][i]));

	Data data(parameterUniqueID, tstart, tend, mime_type, events, raw_data);

	return data;
}

// ****************************************************************************
// *                               Event Search                               *
// ****************************************************************************

//
//
//
vector<DescriptorUID> SdasClient::searchDeclaredEventsByUniqueID(string uniqueID)
{
	vector<DescriptorUID> eventList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	args.setSize(2);
	args[0] = this->sessionId;
	args[1] = uniqueID;
	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchDeclaredEventsByUniqueID", args, result))
	{
		for(int i = 0 ; i < result.size() ; i++)
		{
			DescriptorUID descriptor = this->createDescriptorUidStructureFromXmlRpcValue(result[i]);
			eventList.push_back(descriptor);
		}
	}

	return eventList;
}

//
//
//
vector<DescriptorUID> SdasClient::searchDeclaredEventsByName(string name)
{
	vector<DescriptorUID> eventList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;
    
    //
	// Setup the arguments
	//
	args.setSize(2);
	args[0] = this->sessionId;
	args[1] = name;

	//
	// Execute the command
	//
	printf("okayyy...\n");

	if(this->client->execute("SDASServer.searchDeclaredEventsByName", args, result))
	{
		printf("okayyy...\n");
		for(int i = 0 ; i < result.size() ; i++)
		{
			DescriptorUID descriptor = this->createDescriptorUidStructureFromXmlRpcValue(result[i]);
			eventList.push_back(descriptor);
		}
	}
	printf("skip if?\n");
	return eventList;
}

//
//
//
vector<DescriptorUID> SdasClient::searchDeclaredEventsByName(string name, string locale)
{
	vector<DescriptorUID> eventList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(3);
    args[0] = this->sessionId;
	args[1] = name;
	args[2] = locale;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchDeclaredEventsByName", args, result))
	{
		for(int i = 0 ; i < result.size() ; i++)
		{
			DescriptorUID descriptor = this->createDescriptorUidStructureFromXmlRpcValue(result[i]);
			eventList.push_back(descriptor);
		}
	}

	return eventList;
}

//
//
//
vector<DescriptorUID> SdasClient::searchDeclaredEventsByDescription(string description)
{
	vector<DescriptorUID> eventList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	args.setSize(2);
	args[0] = this->sessionId;
	args[1] = description;
	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchDeclaredEventsByDescription", args, result))
	{
		for(int i = 0 ; i < result.size() ; i++)
		{
			DescriptorUID descriptor = this->createDescriptorUidStructureFromXmlRpcValue(result[i]);
			eventList.push_back(descriptor);
		}
	}

	return eventList;
}

//
//
//
vector<DescriptorUID> SdasClient::searchDeclaredEventsByDescription(string description, string locale)
{
	vector<DescriptorUID> eventList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(3);
	args[0] = this->sessionId;
	args[1] = description;
	args[2] = locale;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchDeclaredEventsByDescription", args, result))
	{
		for(int i = 0 ; i < result.size() ; i++)
		{
			DescriptorUID descriptor = this->createDescriptorUidStructureFromXmlRpcValue(result[i]);
			eventList.push_back(descriptor);
		}
	}

	return eventList;
}

//
//
//
vector<HEvent> SdasClient::searchEventsByEventNumber(int number)
{
	vector<HEvent> eventList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;
    
    //
	// Setup the arguments
	//
    args.setSize(2);
	args[0] = this->sessionId;
	args[1] = number;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchEventsByEventNumber", args, result))
	{
		for(int i = 0 ; i < result.size() ; i++)
		{
			HEvent hevent = this->createHEventStructureFromXmlRpcValue(result[i]);
			eventList.push_back(hevent);
		}
	}

	return eventList;
}

//
//
//
vector<HEvent> SdasClient::searchEventsByEventTimeWindow(TimeStamp from, TimeStamp to)
{
	vector<HEvent> eventList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
    args.setSize(3);
	args[0] = this->sessionId;
	args[1] = this->createXmlRpcValueFromTimeStamp(from);
	args[2] = this->createXmlRpcValueFromTimeStamp(to);

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchEventsByEventTimeWindow", args, result))
	{
		for(int i = 0 ; i < result.size() ; i++)
		{
			HEvent hevent = this->createHEventStructureFromXmlRpcValue(result[i]);
			eventList.push_back(hevent);
		}
	}

	return eventList;
}

//
//
//
int SdasClient::searchMaxEventNumber(string eventUniqueID)
{
	int maxEvent = 0;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;
    
    //
	// Setup the arguments
	//
    args.setSize(2);
	args[0] = this->sessionId;
	args[1] = eventUniqueID;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchMaxEventNumber", args, result))
		maxEvent = result;

	return maxEvent;
}

//
//
//
int SdasClient::searchMinEventNumber(string eventUniqueID)
{
	int maxEvent = 0;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;
    
    //
	// Setup the arguments
	//
    args.setSize(2);
	args[0] = this->sessionId;
	args[1] = eventUniqueID;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchMinEventNumber", args, result))
		maxEvent = result;

	return maxEvent;
}

// ****************************************************************************
// *                             Parameter Search                             *
// ****************************************************************************

//
//
//
vector<Parameter> SdasClient::searchParametersByUniqueID(string uniqueID)
{
	vector<Parameter> parameterList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	args.setSize(2);
	args[0] = this->sessionId;
	args[1] = uniqueID;
	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchParametersByUniqueID", args, result))
		for(int i = 0 ; i < result.size() ; i++)
			parameterList.push_back(this->createParameterStructureFromXmlRpcValue(result[i]));

	return parameterList;
}

//
//
//
vector<Parameter> SdasClient::searchParametersByName(string name)
{
	vector<Parameter> parameterList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	args.setSize(2);
	args[0] = this->sessionId;
	args[1] = name;
	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchParametersByName", args, result))
		for(int i = 0 ; i < result.size() ; i++)
			parameterList.push_back(this->createParameterStructureFromXmlRpcValue(result[i]));

	return parameterList;
}

//
//
//
vector<Parameter> SdasClient::searchParametersByName(string name, string locale)
{
	vector<Parameter> parameterList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(3);
	args[0] = this->sessionId;
	args[1] = name;
	args[2] = locale;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchParametersByName", args, result))
		for(int i = 0 ; i < result.size() ; i++)
			parameterList.push_back(this->createParameterStructureFromXmlRpcValue(result[i]));

	return parameterList;
}

//
//
//
vector<Parameter> SdasClient::searchParametersByDescription(string description)
{
	vector<Parameter> parameterList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	args.setSize(2);
	args[0] = this->sessionId;
	args[1] = description;
	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchParametersByDescription", args, result))
		for(int i = 0 ; i < result.size() ; i++)
			parameterList.push_back(this->createParameterStructureFromXmlRpcValue(result[i]));

	return parameterList;
}

//
//
//
vector<Parameter> SdasClient::searchParametersByDescription(string description, string locale)
{
	vector<Parameter> parameterList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(3);
	args[0] = this->sessionId;
	args[1] = description;
	args[2] = locale;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchParametersByDescription", args, result))
		for(int i = 0 ; i < result.size() ; i++)
			parameterList.push_back(this->createParameterStructureFromXmlRpcValue(result[i]));

	return parameterList;
}

bool SdasClient::parameterExists(string uniqueID, string eventUniqueID, int eventNumber)
{
    bool exists = false;
    //
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(4);
	args[0] = this->sessionId;
	args[1] = uniqueID;
	args[2] = eventUniqueID;
    args[3] = eventNumber;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.parameterExists", args, result))
		exists = result;
    
    return exists;
}

// ****************************************************************************
// *                                Data Search                               *
// ****************************************************************************

//
//
//
vector<string> SdasClient::searchDataByEvent(HEvent hevent)	
{
	vector<string> parameterUniqueIdList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;
    
    //
	// Setup the arguments
	//
    args.setSize(2);
	args[0] = this->sessionId;
	args[1] = this->createXmlRpcValueFromHEvent(hevent);

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchDataByEvent", args, result))
		for(int i = 0 ; i < result.size() ; i++)
			parameterUniqueIdList.push_back(result[i]);

	return parameterUniqueIdList;
}

//
//
//
vector<string> SdasClient::searchDataByEvent(string eventUniqueID, int eventNumber)
{
	vector<string> parameterUniqueIdList;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(3);
    args[0] = this->sessionId;
	args[1] = eventUniqueID;
	args[2] = eventNumber;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.searchDataByEvent", args, result))
		for(int i = 0 ; i < result.size() ; i++)
			parameterUniqueIdList.push_back(result[i]);

	return parameterUniqueIdList;
}

// ****************************************************************************
// *                              Data Retrieving                             *
// ****************************************************************************

//
//
//
vector<Data> SdasClient::getData(string parameterUniqueID, string eventUniqueID, int eventNumber)
{
	vector<Data> retrievedData;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(4);
	args[0] = this->sessionId;
	args[1] = parameterUniqueID;
	args[2] = eventUniqueID;
	args[3] = eventNumber;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.getData", args, result))
		for (int i = 0; i < result.size(); i++)
			retrievedData.push_back(createDataStructureFromXmlRpcValue(result[i]));

	return retrievedData;
}

//
//
//
vector<Data> SdasClient::getData(string parameterUniqueID, HEvent hevent)
{

	vector<Data> retrievedData;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//

	args.setSize(3);
	args[0] = this->sessionId;
	args[1] = parameterUniqueID;
	args[2] = this->createXmlRpcValueFromHEvent(hevent);

	//
	// Execute the command
	//

	if(this->client->execute("SDASServer.getData", args, result)) { //da throw a exception quando o rpc value e passado
		for (int i = 0; i < result.size(); i++)
			retrievedData.push_back(createDataStructureFromXmlRpcValue(result[i]));
	}

	return retrievedData;
}

//
//
//
vector<vector<Data> > SdasClient::getMultipleData(vector<string> parameterUniqueID, vector<string> eventUniqueID, vector<int> eventNumber)
{
	vector<vector<Data> > retrievedData;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(4);
	args[0] = this->sessionId;
	args[1].setSize(parameterUniqueID.size());
	for(unsigned int i = 0 ; i < parameterUniqueID.size() ; i++)
		args[1][i] = parameterUniqueID[i];
	args[2].setSize(eventUniqueID.size());
	for(unsigned int i = 0 ; i < eventUniqueID.size() ; i++)
		args[2][i] = eventUniqueID[i];
	args[3].setSize(eventNumber.size());
	for(unsigned int i = 0 ; i < eventNumber.size() ; i++)
		args[3][i] = eventNumber[i];

	//
	// Execute the command
	//
	vector<Data> temp;
	if(this->client->execute("SDASServer.getMultipleData", args, result)) {
		for(int i = 0 ; i < result.size() ; i++) {
				temp.clear();
				for(int j = 0; j < result[0].size(); j++)
					temp.push_back(createDataStructureFromXmlRpcValue(result[i][j]));
			
			retrievedData.push_back(temp);
		}
	}
	return retrievedData;
}

//
//
//
vector<vector<Data> > SdasClient::getMultipleData(vector<string> parameterUniqueID, vector<HEvent> hevent)
{
	vector<vector<Data> > retrievedData;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(3);
	args[0] = this->sessionId;
	args[1].setSize(parameterUniqueID.size());
	for(unsigned int i = 0 ; i < parameterUniqueID.size() ; i++)
		args[1][i] = parameterUniqueID[i];
	args[2].setSize(hevent.size());
	for(unsigned int i = 0 ; i < hevent.size() ; i++)
		args[2][i] = this->createXmlRpcValueFromHEvent(hevent[i]);

	//
	// Execute the command
	//
	vector<Data> temp;
	if(this->client->execute("SDASServer.getMultipleData", args, result)) {
		temp.clear();
		for(int i = 0 ; i < result.size() ; i++) {
				for(int j = 0; j < result[0].size(); j++)
					temp.push_back(createDataStructureFromXmlRpcValue(result[i][j]));
			
			retrievedData.push_back(temp);
		}
	}
	return retrievedData;
}

//
//
//
vector<vector<Data> > SdasClient::getMultipleData(vector<string> parameterUniqueID, HEvent hevent)
{
	vector<vector<Data> > retrievedData;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(3);
	args[0] = this->sessionId;
	args[1].setSize(parameterUniqueID.size());
	for(unsigned int i = 0 ; i < parameterUniqueID.size() ; i++)
		args[1][i] = parameterUniqueID[i];
	args[2] = this->createXmlRpcValueFromHEvent(hevent);

	//
	// Execute the command
	//
	vector<Data> temp;
	if(this->client->execute("SDASServer.getMultipleData", args, result)) {
		temp.clear();
		for(int i = 0 ; i < result.size() ; i++) {
				for(int j = 0; j < result[0].size(); j++)
					temp.push_back(createDataStructureFromXmlRpcValue(result[i][j]));
			
			retrievedData.push_back(temp);
		}
	}
	return retrievedData;;
}

//
//
//
vector<vector<Data> > SdasClient::getMultipleData(vector<string> parameterUniqueID, string eventUniqueID, int eventNumber)
{
	vector<vector<Data> > retrievedData;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(4);
	args[0] = this->sessionId;
	args[1].setSize(parameterUniqueID.size());
	for(unsigned int i = 0 ; i < parameterUniqueID.size() ; i++)
		args[1][i] = parameterUniqueID[i];
	args[2] = eventUniqueID;
	args[3] = eventNumber;

	//
	// Execute the command
	//
	vector<Data> temp;
	if(this->client->execute("SDASServer.getMultipleData", args, result)) {
		temp.clear();
		for(int i = 0 ; i < result.size() ; i++) {
				for(int j = 0; j < result[0].size(); j++)
					temp.push_back(createDataStructureFromXmlRpcValue(result[i][j]));
			
			retrievedData.push_back(temp);
		}
	}
	return retrievedData;
}

//
//
//
vector<vector<Data> > SdasClient::getMultipleData(string parameterUniqueID, vector<string> eventUniqueID, vector<int> eventNumber)
{
	vector<vector<Data> > retrievedData;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(4);
	args[0] = this->sessionId;
	args[1] = parameterUniqueID;
	args[2].setSize(eventUniqueID.size());
	for(unsigned int i = 0 ; i < eventUniqueID.size() ; i++)
		args[2][i] = eventUniqueID[i];
	args[3].setSize(eventNumber.size());
	for(unsigned int i = 0 ; i < eventNumber.size() ; i++)
		args[3][i] = eventNumber[i];

	//
	// Execute the command
	//
	vector<Data> temp;
	if(this->client->execute("SDASServer.getMultipleData", args, result)) {
		temp.clear();
		for(int i = 0 ; i < result.size() ; i++) {
				for(int j = 0; j < result[0].size(); j++)
					temp.push_back(createDataStructureFromXmlRpcValue(result[i][j]));
			
			retrievedData.push_back(temp);
		}
	}
	return retrievedData;
}

//
//
//
vector<vector<Data> > SdasClient::getMultipleData(string parameterUniqueID, string eventUniqueID, vector<int> eventNumber)
{
	vector<vector<Data> > retrievedData;

	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(4);
	args[0] = this->sessionId;
	args[1] = parameterUniqueID;
	args[2] = eventUniqueID;
	args[3].setSize(eventNumber.size());
	for(unsigned int i = 0 ; i < eventNumber.size() ; i++)
		args[3][i] = eventNumber[i];

	//
	// Execute the command
	//
	vector<Data> temp;
	if(this->client->execute("SDASServer.getMultipleData", args, result)) {
		temp.clear();
		for(int i = 0 ; i < result.size() ; i++) {
				for(int j = 0; j < result[0].size(); j++)
					temp.push_back(createDataStructureFromXmlRpcValue(result[i][j]));
			
			retrievedData.push_back(temp);
		}
	}
	return retrievedData;
}

// ****************************************************************************
// *                                 User Data                                *
// ****************************************************************************

//
//
//
bool SdasClient::createNewDataStructure(string structure_name, vector<string> parameters)
{
	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(3);
	args[0] = this->sessionId;
	args[1] = structure_name;
	args[2].setSize(parameters.size());
	for(unsigned int i = 0 ; i < parameters.size() ; i++)
		args[2][i] = parameters[i];

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.createNewDataStructure", args, result))
		return result;

	return false;
}

//
//
//
bool SdasClient::removeDataStructure(string structure_name)
{
	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(2);
	args[0] = this->sessionId;
	args[1] = structure_name;

	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.createNewDataStructure", args, result))
		return result;

	return false;
}

//
//
//
bool SdasClient::putData(Data data)
{
	//
	// Parameters and results
	//
	XmlRpcValue args;
	XmlRpcValue result;

	//
	// Setup the arguments
	//
	args.setSize(2);
	args[0] = this->sessionId;
	args[1] = this->createXmlRpcValueFromData(data);

    cout << args[1] << endl;
	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.putData", args, result))
    {
        cerr << result << endl;
		return true;
    }

	return false;
}

//
//
//
string SdasClient::authenticate(string credentials[2])
{
    string sessionId = "";
	//
	// Parameters and results
	//
    XmlRpcValue args;
	XmlRpcValue result;
    
    //
	// Setup the arguments
	//
	args["username"] = credentials[0];
    args["password"] = credentials[1];
    
	//
	// Execute the command
	//
	if(this->client->execute("SDASServer.authenticate", args, result))
        sessionId = string(result);
    
	return sessionId;
}

