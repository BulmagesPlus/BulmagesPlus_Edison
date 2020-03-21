/***************************************************************************
 *   Copyright (C) 2007 by Tomeu Borrás Riera.                             *
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

#include "blradiobutton.h"


/** Inicializa todos los componenetes a NULL para que no se produzcan confusiones
    sobre si un elemento ha sido creado o no.
    Conecta el activar un tipo con m_activated.
*/
/**
\param parent
**/
BlRadioButton::BlRadioButton ( QWidget *parent )
        : QRadioButton ( parent )
{
    BL_FUNC_DEBUG
    
}


/** Inicializa todos los componenetes a NULL para que no se produzcan confusiones
    sobre si un elemento ha sido creado o no.
    Conecta el activar un tipo con m_activated.
*/
/**
\param text
\param parent
**/
BlRadioButton::BlRadioButton ( const QString & text, QWidget * parent )
        : QRadioButton ( text, parent )
{
    BL_FUNC_DEBUG
}


/// Se libera memoria destruyendo el cursor que almacena los tipos de IVA si es que esta creado.
/**
**/
BlRadioButton::~BlRadioButton()
{
    BL_FUNC_DEBUG
}


/**
**/
void BlRadioButton::setFieldValue(QString fieldValue)
{
    m_fieldValue = fieldValue;
}


/**
**/
QString BlRadioButton::fieldValue() const
{
    return m_fieldValue;
}
