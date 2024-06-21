/*
 * ErrorInfo.cpp
 *
 * Created on May 22, 2024, 00:00 AM
 *
 * <p>Title: SDAS</p>
 *
 * <p>Description: Shared Data Access System</p>
 *
 * <p>Copyright: (C) Copyright 2024-2025, by Centro de Fusao Nuclear
 *
 * Project Info:  	http://baco.ipfn.tecnico.ulisboa.pt/sdas
 *                      https://www.ipfn.tecnico.ulisboa.pt
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
 * </p>
 *
 * <p>Company: IPFN - EURATOM/IST- https://www.ipfn.tecnico.ulisboa.pt</p>
 *
 * Author: Andre Sancho Duarte
 * Version: 1.0.0
 *
 */
 
#include "ErrorInfo.h"

using namespace org::sdas::core::common;

// Constructors
ErrorInfo::ErrorInfo(void)
{
    this->code = 0;
    this->text = "";
}

ErrorInfo::ErrorInfo(int code, string text)
{
    this->code = code;
    this->text = text;
}

// Destructor
ErrorInfo::~ErrorInfo(void)
{
}
int ErrorInfo::getCode()
{
    return this->code;
}

string ErrorInfo::getText()
{
    return this->text;
}
