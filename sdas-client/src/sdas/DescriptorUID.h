/*
 * DescriptorUID.h
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

#ifndef __DESCRIPTORUID_H__
#define __DESCRIPTORUID_H__

#include <string>
#include "Descriptor.h"

namespace org
{
	namespace sdas
	{
		namespace core
		{
			namespace utils
			{

				class DescriptorUID : public Descriptor
				{
					private:
						//std::string name;
						//std::string description;
						//std::string locale;
						std::string uniqueID;

					public:
						DescriptorUID(void);
						DescriptorUID(std::string name);
						DescriptorUID(std::string name, std::string description);
						DescriptorUID(std::string name, std::string description, std::string locale);
						DescriptorUID(std::string name, std::string description, std::string locale, std::string uniqueID);

					public:
						~DescriptorUID(void);

					public:
						std::string getUniqueID(void);
						void setUniqueID(std::string uniqueID);
						/*std::string getName(void);
						std::string getDescription(void);
						std::string getLocale(void);
						void setName(std::string name);
						void setDescription(std::string description);
						void setLocale(std::string locale);*/
				};

			} // utils
		} // core
	} // sdas
} // org

#endif
