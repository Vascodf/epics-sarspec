/*
 * Time.cpp
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

#include <stdio.h>
#include "Time.h"

using namespace org::sdas::core::time;

// Constructors...
Time::Time(void)
{
	this->hours = 0;
	this->minutes = 0;
	this->seconds = 0;
	this->millis = 0;
	this->micros = 0;
	this->nanos = 0;
	this->picos = 0;
}

Time::Time(char hours, char minutes, char seconds, short millis, short micros, short nanos, short picos)
{
	this->hours = hours;
	this->minutes = minutes;
	this->seconds = seconds;
	this->millis = millis;
	this->micros = micros;
	this->nanos = nanos;
	this->picos = picos;
}

Time::~Time(void)
{
}

char Time::getHours(void)
{
	return this->hours;
}

char Time::getMinutes(void)
{
	return this->minutes;
}

char Time::getSeconds(void)
{
	return this->seconds;
}

short Time::getMillis(void)
{
	return this->millis;
}

short Time::getMicros(void)
{
	return this->micros;
}

short Time::getNanos(void)
{
	return this->nanos;
}

short Time::getPicos(void)
{
	return this->picos;
}

void Time::setHours(char hours)
{
	this->hours = hours;
}

void Time::setMinutes(char minutes)
{
    if (minutes >= 60)
    {
        this->setHours(this->getHours() + (minutes/60));
        minutes %= 60;
    }
	this->minutes = minutes;
}

void Time::setSeconds(char seconds)
{
    if (seconds >= 60)
    {
        this->setMinutes(this->getMinutes() + (seconds/60));
        seconds %= 60;
    }
	this->seconds = seconds;
}

void Time::setMillis(short millis)
{
    if (millis >= 1000)
    {
        this->setSeconds(this->getSeconds() + (millis/1000));
        millis %= 1000;
    }
	this->millis = millis;
}

void Time::setMicros(short micros)
{
    if (micros >= 1000)
    {
        this->setMillis(this->getMillis() + (micros/1000));
        micros %= 1000;
    }
	this->micros = micros;
}

void Time::setNanos(short nanos)
{
	this->nanos = nanos;
}

void Time::setPicos(short picos)
{
    if (picos >= 1000)
    {
        this->setMicros(this->getMicros() + (picos/1000));
        picos %= 1000;
    }
	this->picos = picos;
}

std::string Time::toString(void)
{
	std::string result;
	char h[15];
	char min[15];
	char s[15];
	char mil[15];
	char mic[15];
	char n[15];
	char p[15];
	sprintf(h, "%d", this->hours);
	sprintf(min, "%d", this->minutes);
	sprintf(s, "%d", this->seconds);
	sprintf(mil, "%d", this->millis);
	sprintf(mic, "%d", this->micros);
	sprintf(n, "%d", this->nanos);
	sprintf(p, "%d", this->picos);
	result = h;
	result += ":";
	result += min;
	result += ":";
	result += s;
	result += ".";
	result += mil;
	result += ".";
	result += mic;
	result += ".";
	result += n;
	result += ".";
	result += p;

	return result;
}
