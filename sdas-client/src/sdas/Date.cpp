/*
 * Date.cpp
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

#include "Date.h"

using namespace std;
using namespace org::sdas::core::time;

// Constructors...
Date::Date(void)
{
	this->day = 1;
	this->month = 0;
	this->year = 1970;
}

Date::Date(short year)
{
	this->year = year;
	this->month = 0;
	this->day = 1;
}

Date::Date(short year, char month)
{
	this->year = year;
	this->month = month;
	this->day = 1;
}

/*Date::Date(short year, char month, char day)
{
	this->year = year;
	this->month = month;
	this->day = day;
}*/

Date::Date(int year, int month, int day)
{
	this->year = (short) year;
	this->month = (char) month;
	this->day = (char) day;
}

// Destructor
Date::~Date(void)
{
}

// Other
char Date::getDay()
{
	return this->day;
}

char Date::getMonth()
{
	return this->month;
}

short Date::getYear()
{
	return this->year;
}

void Date::setDay(char day)
{
	this->day = day;
}

void Date::setMonth(char month)
{
	this->month = month;
}

void Date::setYear(short year)
{
	this->year = year;
}

string Date::toString()
{
	string result;
	char d[15];
	char m[15];
	char y[15];
	sprintf(d, "%d", this->day);
	sprintf(m, "%d", this->month + 1);
	sprintf(y, "%d", this->year);
	result = d;
	result += "-";
	result += m;
	result += "-";
	result += y;

	return result;
}
