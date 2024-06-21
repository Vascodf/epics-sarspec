/*
 * Data.h
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

#ifndef __DATA_H__
#define __DATA_H__

#include <string>
#include <vector>
#include <map>

#include "HEvent.h"
#include "ErrorInfo.h"

using namespace std;
using namespace org::sdas::core::common;
using namespace org::sdas::core::time;

namespace org
{
	namespace sdas
	{
		namespace core
		{
			namespace common
			{

				class Data
				{
					private:
						string parameterUniqueID;
						TimeStamp tstart;
						TimeStamp tend;
						string mime_type;
                        string data_type;
						vector<HEvent> events;
						vector<char> raw_data;
                        map<string, string> extra_info;
                        string units;
                        string transfer_function;
                        ErrorInfo error_info;

					public:
						Data(void);
						Data(string uniqueID, TimeStamp tStart, TimeStamp tEnd, string mimeType, vector<HEvent> evt, vector<char> data);

					public:
						~Data(void);

					public:
						string getParameterUniqueID();
						TimeStamp getTimeStart();
						TimeStamp getTimeEnd();
						string getMimeType();
						vector<HEvent> getEvents();
						vector<char> getRawData();
						vector<short> getInt16Data();
						vector<int> getInt32Data();
						vector<float> getFloat32Data();
						vector<double> getFloat64Data();
                        string getDataType();
                        map<string, string> getExtraInfo();
                        string getUnits();
                        string getTransferFunction();
                        ErrorInfo getErrorInfo();
						void setParameterUniqueID(string uniqueID);
						void setTimeStart(TimeStamp tStart);
						void setTimeEnd(TimeStamp tEnd);
						void setMimeType(string mimeType);
						void setEvents(vector<HEvent> evt);
						void setRawData(vector<char> data);
                        void setExtraInfo(map<string, string> extraInfo);
                        void setErrorInfo(ErrorInfo errorInfo);
				};

			} // common
		} // core
	} // sdas
} // org

#endif

