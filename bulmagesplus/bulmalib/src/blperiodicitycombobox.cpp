/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borrás Riera.                             *
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

#include "blperiodicitycombobox.h"
#include "local_blI18n.h"

///
/**
\return
**/
QString BlPeriodicityComboBox::periodo() const
{
    BL_FUNC_DEBUG
    
    return m_valores[currentIndex()];
}


///
/**
\return
**/
QString BlPeriodicityComboBox::fieldValue() const
{
    BL_FUNC_DEBUG
    
    return m_valores[currentIndex()];
}


///
/**
\param index
**/
void BlPeriodicityComboBox::s_activated ( int index )
{
    BL_FUNC_DEBUG
    if ( index > 0 ) {
        emit ( valueChanged ( m_valores[currentIndex() ] ) );
    } else {
        emit ( valueChanged ( "" ) );
    } // end if
}


///
/**
\param parent
**/
BlPeriodicityComboBox::BlPeriodicityComboBox ( QWidget *parent ) : QComboBox ( parent )
{
    BL_FUNC_DEBUG
    m_textos[0] = _ ( "Semanal" );
    m_textos[1] = _ ( "2 Semanas" );
    m_textos[2] = _ ( "Quincenal" );
    m_textos[3] = _ ( "Mensual" );
    m_textos[4] = _ ( "Trimestral" );
    m_textos[5] = _ ( "Semestral" );
    m_textos[6] = _ ( "Anual" );
    m_textos[7] = _ ( "Bienal" );
    m_textos[8] = _ ( "Sin Periodicidad" );
    m_textos[9] = "";

    m_valores[0] = "@ 7 days";
    m_valores[1] = "@ 2 weeks";
    m_valores[2] = "@ 15 days";
    m_valores[3] = "@ 1 mon";
    m_valores[4] = "@ 3 mons";
    m_valores[5] = "@ 6 mons";
    m_valores[6] = "@ 1 year";
    m_valores[7] = "@ 2 year";
    m_valores[8] = "00:00:00";
    m_valores[9] = "";

    int i = 0;
    while ( m_textos[i] != "" ) {
        /// Anyade un nuevo elemento al final del QComboBox.
        insertItem ( this->count() + 1, m_textos[i] );
        i++;
    } // end while

    connect ( this, SIGNAL ( activated ( int ) ), this, SLOT ( s_activated ( int ) ) );
    
}


///
/**
**/
BlPeriodicityComboBox::~BlPeriodicityComboBox()
{
    BL_FUNC_DEBUG
    
}


///
/**
Establece cual es el periodo en el combobox y lo pone.
\param periodo QString en sintaxis de PostgreSQL (@ 1 year, 3 mons, etc)
**/
void BlPeriodicityComboBox::setperiodo ( QString periodo )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlPeriodicityComboBox::setperiodo", 0, periodo );
    int i = 0;
    /// Buscamos un campo que se corresponda con el periodo.
    /// El problema es que postgres acepta como sintaxis de entrada la arroba
    /// y como sintaxis de salida no la devuelve.
    while (  (m_valores[i] != periodo)   && (m_valores[i] != "@ "+ periodo) && (m_valores[i] != "") ) {
        i++;
    } // end while
    if ( m_valores[i] != "" ) {
        setCurrentIndex ( i );
    } else {
        setCurrentIndex ( 0 );
    } // end if
}


///
/**
\param periodo
**/
void BlPeriodicityComboBox::setFieldValue ( QString periodo )
{
    BL_FUNC_DEBUG
    setperiodo (periodo);
}
