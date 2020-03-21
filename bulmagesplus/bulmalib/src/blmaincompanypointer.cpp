/***************************************************************************
 *   Copyright (C) 2006 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2012 by Fco. Javier Miró Carmona.                       *
 *                                                                         *
 *   https://bulmagesplus.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "blmaincompanypointer.h"


///
/**
\param emp
**/
BlMainCompanyPointer::BlMainCompanyPointer(BlMainCompany *mainCompany)
{
    BL_FUNC_DEBUG

    m_mainCompany = mainCompany;
}


///
/**
**/
BlMainCompanyPointer::BlMainCompanyPointer()
{
    BL_FUNC_DEBUG
    m_mainCompany = NULL;
}


///
/**
**/
BlMainCompanyPointer::~BlMainCompanyPointer()
{
    BL_FUNC_DEBUG
}


///
/**
\param emp
**/
void BlMainCompanyPointer::setMainCompany ( BlMainCompany *emp )
{
    BL_FUNC_DEBUG
    m_mainCompany = emp;
    
}


///
/**
\return
**/
BlMainCompany *BlMainCompanyPointer::mainCompany() const
{
    BL_FUNC_DEBUG
    
    return m_mainCompany;
}


///
/**
\return
**/
QString BlMainCompanyPointer::fieldValue() const
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlMainCompanyPointer::fieldValue", 2, "debe ser derivado" );
    return "";
}


///
/**
**/
void BlMainCompanyPointer::setFieldValue(QString)
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlMainCompanyPointer::setFieldValue", 2, "debe ser derivado" );
}

