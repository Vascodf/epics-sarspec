/*
 * ErrorInfo.h
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
#ifndef __ERROR_INFO_H__
#define __ERROR_INFO_H__

#include <string>
#include <map>

using namespace std;

namespace org
{
	namespace sdas
	{
		namespace core
		{
			namespace common
			{
				class ErrorInfo
                {
                    private:
                        int code;
                        string text;
                        
                    public:
                        ErrorInfo(void);
                        ErrorInfo(int code, string text);
                        
                    public:
                        ~ErrorInfo(void);
                        
                    public:
                        int getCode();
                        string getText();
                };

			} // common
		} // core
	} // sdas
} // org

#endif
