/*
 * Parameter.h
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

#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include <string>
#include <map>

#include "DescriptorUID.h"

using namespace std;
using namespace org::sdas::core::utils;

namespace org
{
	namespace sdas
	{
		namespace core
		{
			namespace common
			{

				class Parameter
				{
					private:
						DescriptorUID descriptorUID;
						string mime_type;
						map<string, string> extra_info;

					public:
						Parameter(void);
						Parameter(DescriptorUID info);
						Parameter(DescriptorUID info, string mimeType);
						Parameter(DescriptorUID info, string mimeType, map<string, string> extraInfo);

					public:
						~Parameter(void);

					public:
						DescriptorUID getDescriptorUID();
						string getMimeType();
						map<string, string> getExtraInfo();
						void setDescriptorUID(DescriptorUID info);
						void setMimeType(string mimeType);
						void setExtraInfo(map<string, string> extraInfo);
				};

			} //common
		} //core
	} // sdas
} // org

#endif

