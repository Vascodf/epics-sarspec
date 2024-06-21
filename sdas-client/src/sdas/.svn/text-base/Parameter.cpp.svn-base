/*
 * Parameter.cpp
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

#include "Parameter.h"

using namespace org::sdas::core::common;

// Constructors
Parameter::Parameter(void)
{
}

Parameter::Parameter(DescriptorUID info)
{
	this->setDescriptorUID(info);
}

Parameter::Parameter(DescriptorUID info, string mimeType)
{
	this->setDescriptorUID(info);
	this->setMimeType(mimeType);
}

Parameter::Parameter(DescriptorUID info, string mimeType, map<string, string> extraInfo)
{
	this->setDescriptorUID(info);
	this->setMimeType(mimeType);
	this->setExtraInfo(extraInfo);
}

// Destructor
Parameter::~Parameter(void)
{
}

DescriptorUID Parameter::getDescriptorUID()
{
	return this->descriptorUID;
}

string Parameter::getMimeType()
{
	return this->mime_type;
}

map<string, string> Parameter::getExtraInfo()
{
	return this->extra_info;
}

void Parameter::setDescriptorUID(DescriptorUID info)
{
	this->descriptorUID = info;
}

void Parameter::setMimeType(string mimeType)
{
	this->mime_type = mimeType;
}

void Parameter::setExtraInfo(map<string, string> extraInfo)
{
	this->extra_info = extraInfo;
}
