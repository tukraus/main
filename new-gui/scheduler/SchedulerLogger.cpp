/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "stdafx.h"

#include "new-gui/Common/+Utils.h"
#include "SchedulerLogger.h"


void TraceWrite( const QString& msg, int debuglevel, const char* file, const char* function, int line )	//debuglevel = 1, const char* file = nullptr, const char* function = nullptr, int line = -1 
{
	CApplicationLoggerBase::TraceWrite( msg, debuglevel, file, function, line );
}



//static 
CSchedulerLogger& CSchedulerLogger::Instance()
{
	if ( !smInstance )
		smInstance = new CSchedulerLogger();

	return *dynamic_cast< CSchedulerLogger* >( smInstance );
}


//	Dynamic instance of the class is passed to osg::setNotifyHandler and automatically deleted by OSG
//
CSchedulerLogger::CSchedulerLogger() 
	: base_t()
{
	assert__( smInstance && smInstance == this );		//smInstance not necessarily == this
}

//virtual 
CSchedulerLogger::~CSchedulerLogger()
{}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_SchedulerLogger.cpp"
