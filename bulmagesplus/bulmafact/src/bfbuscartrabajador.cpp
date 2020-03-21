/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
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

#include "blcombobox.h"

#include "bfbuscartrabajador.h"
#include "bfcompany.h"


/** Inicializa todos los componentes a NULL para que no haya confusiones sobre
    si un elemento ha sido inicializado o no.
    Hace la conexion del SIGNAL activated con m_activated para tratar el evento.
*/
/**
\param parent
**/
BfBuscarTrabajador::BfBuscarTrabajador ( QWidget *parent )
        : BlComboBox ( parent )
{
    BL_FUNC_DEBUG

    m_comboRecordSet = NULL;
    setTableName("trabajador");
    setFieldId("idtrabajador");
    setAllowNull(true);
}


/** Libera la memoria dinamica que se estaba utilizando.
*/
/**
**/
BfBuscarTrabajador::~BfBuscarTrabajador()
{
    BL_FUNC_DEBUG
}


/** Con este metodo se puede indicar al Widget cual es el trabajador seleccionado.
    Recarga el cursor de trabajadores y cuando encuentra uno con el mismo identificador
    que el que se ha pasado por parametro lo establece como elemento seleccionado.
*/
/**
\param idtrabajador
\return
**/
void BfBuscarTrabajador::setId ( QString idtrabajador )
{
    BL_FUNC_DEBUG

    /// Si lo que se pasa como forma de pago es un valor malo cogemos la forma de pago por defecto.
    if ( idtrabajador == "0" ) {
        idtrabajador = g_confpr->value( CONF_IDTRABAJADOR_DEFECTO );
    } // end if

    if ( m_comboRecordSet != NULL ) {
        delete m_comboRecordSet;
    } // end if

    m_comboRecordSet = mainCompany() ->loadQuery ( "SELECT * FROM trabajador" );
    if ( !m_comboRecordSet ) return;
    int i = 0;
    int i1 = 0;
    clear();
    addItem ( "--" );

    while ( !m_comboRecordSet->eof() ) {
        i ++;

        if ( m_comboRecordSet->value( "idtrabajador" ) == idtrabajador ) {
            i1 = i;
        } // end if

        addItem ( m_comboRecordSet->value( "apellidostrabajador" ) + ", " + m_comboRecordSet->value( "nomtrabajador" ) );
        m_comboRecordSet->nextRecord();
    } // end while

    setCurrentIndex ( i1 );

    
}



/** Devuelve el identificador del trabajador seleccionado
**/
/**
\return
**/
QString BfBuscarTrabajador::id() const
{
    BL_FUNC_DEBUG

    if (!m_comboRecordSet) {
        return "0";
    } // end if

    return m_comboRecordSet->value( "idtrabajador", currentIndex() - 1 );
}




/** SLOT que responde a la activacion de un elemento en el QComboBox y que hace que se emita el SIGNAL valueChanged
**/
/**
\param index
**/
/*
void BfBuscarTrabajador::m_activated ( int index )
{
    BL_FUNC_DEBUG

    if ( index > 0 ) {
        emit ( valueChanged ( m_comboRecordSet->value( "idtrabajador", index - 1 ) ) );
    } else {
        emit ( valueChanged ( "" ) );
    } // end if

    
}
*/


/// ========================= ITEM DELEGATE ===============================0

/** Inicializa todos los componentes del Widget a NULL para que no haya posibles confusiones
    sobre si un elemento ha sido creado o no.
    Conecta el SIGNAL activated() con m_activated() para tratarlo.
*/
/**
\param parent
**/
BfBuscarTrabajadorDelegate::BfBuscarTrabajadorDelegate ( QWidget *parent )
        : BlComboBox ( parent )
{
    BL_FUNC_DEBUG
}


/** Libera la memoria reservada.
*/
/**
**/
BfBuscarTrabajadorDelegate::~BfBuscarTrabajadorDelegate()
{
    BL_FUNC_DEBUG
}


/** Permite indicar al Widget cual es la serie de factura seleccionada por defecto.
    Recarga cursor de serie_factura y cuando encuentra un registro cuyo codigoserie_factura coincide con el pasado
    como parametro lo establece como el registro activo por el comboBox.
*/
/**
\param cod
**/
void BfBuscarTrabajadorDelegate::set ( const QString &cod )
{
    BL_FUNC_DEBUG
    int index = 0;
    QString codigo = cod;

    if ( m_comboRecordSet != NULL ) {
        delete m_comboRecordSet;
    } // end if

    m_comboRecordSet = mainCompany() ->loadQuery ( "SELECT nomtrabajador, apellidostrabajador FROM trabajador " );
    clear();

    while ( !m_comboRecordSet->eof() ) {
        addItem ( m_comboRecordSet->value( "apellidostrabajador" ) + ", " + m_comboRecordSet->value( "nomtrabajador" ) );
        m_comboRecordSet->nextRecord();
        if ( m_comboRecordSet->value( "apellidostrabajador" ) + ", " + m_comboRecordSet->value( "nomtrabajador" ) == cod )
            index = m_comboRecordSet->currentRecord();
    } // end while

    setEditText ( cod );
    setCurrentIndex ( index );

    
}

