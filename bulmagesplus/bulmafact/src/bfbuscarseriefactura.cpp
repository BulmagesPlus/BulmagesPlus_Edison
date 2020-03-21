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

#include "bfbuscarseriefactura.h"
#include "bfcompany.h"
#include "blfunctions.h"


/** Inicializa todos los componentes del Widget a NULL para que no haya posibles confusiones
    sobre si un elemento ha sido creado o no.
    Conecta el SIGNAL activated() con m_activated() para tratarlo.
*/
/**
\param parent
**/
BfBuscarSerieFactura::BfBuscarSerieFactura ( QWidget *parent )
        : BlComboBox ( parent )
{
    BL_FUNC_DEBUG
    m_comboRecordSet = NULL;
    m_codigoserie_factura = "";
    setTableName("serie_factura");
    setFieldId("codigoserie_factura");
    
}


/** Libera la memoria reservada.
*/
/**
**/
BfBuscarSerieFactura::~BfBuscarSerieFactura()
{
    BL_FUNC_DEBUG
    
}


/** Permite indicar al Widget cual es la serie de factura seleccionada por defecto.
    Recarga cursor de serie_factura y cuando encuentra un registro cuyo codigoserie_factura coincide con el pasado
    como parametro lo establece como el registro activo por el comboBox.
*/
/**
\param codigo
\return
**/
void BfBuscarSerieFactura::setId ( QString codigo )
{
    BL_FUNC_DEBUG
    /// Si lo que se pasa como serie es un valor malo cogemos la serie de factura por defecto.
    /// Prevalece el valor de la configuracion en la base de datos sobre el valor en el archivo de configuracion.

    if ( codigo.isEmpty() || codigo == "0" ) {
        codigo = g_confpr->value( CONF_SERIE_FACTURA_DEFECTO );
    } // end if

    if ( codigo.isEmpty() || codigo == "0" ) {
      BlDbRecordSet *cur = mainCompany() ->loadQuery ( "SELECT * FROM configuracion WHERE nombre ='SerieFacturaDefecto'" );
      if ( !cur ) return;
      if ( !cur->eof() ) {
	  codigo = cur->value( "valor" );
      } // end if
      delete cur;
    } // end if

    if ( m_comboRecordSet != NULL )
        delete m_comboRecordSet;
    m_comboRecordSet = mainCompany() ->loadQuery ( "SELECT * FROM serie_factura" );
    if ( !m_comboRecordSet ) return;
    int i = 0;
    int i1 = 0;
    int i2 = 0;
    clear();
    addItem ( "--" );
    while ( !m_comboRecordSet->eof() ) {
        i ++;
        if ( m_comboRecordSet->value( "codigoserie_factura" ) == m_codigoserie_factura )
            i2 = i;
        if ( m_comboRecordSet->value( "codigoserie_factura" ) == codigo )
            i1 = i;
        addItem ( m_comboRecordSet->value( "codigoserie_factura" ) + ".-" + m_comboRecordSet->value( "descserie_factura" ) );
        m_comboRecordSet->nextRecord();
    } // end if
    if ( i1 != 0 ) {
        setCurrentIndex ( i1 );
    } else {
        setCurrentIndex ( i2 );
    } // end if
    
}


///
/**
\param index
**/
/*
void BfBuscarSerieFactura::m_activated ( int index )
{
    BL_FUNC_DEBUG
    if ( index > 0 ) {
        emit ( valueChanged ( m_comboRecordSet->value( "codigoserie_factura", index - 1 ) ) );
    } else {
        emit ( valueChanged ( "" ) );
    } // end if
    
}
*/

///
/**
\return
**/
QString BfBuscarSerieFactura::id() const
{
    BL_FUNC_DEBUG
    int index = currentIndex();
    if (index > 0)
    {
        return (m_comboRecordSet->value("codigoserie_factura", index - 1));
    }
    else
    {
        return "";
    } // end if
}


///
/**
\param comp
\return
**/
void BfBuscarSerieFactura::setMainCompany(BfCompany *company)
{
    BL_FUNC_DEBUG

    BlMainCompanyPointer::setMainCompany(company);
    BlDbRecordSet *rs = mainCompany()->loadQuery("SELECT * FROM configuracion WHERE nombre ='SerieFacturaDefecto'");
    if (!rs) return;
    if (!rs->eof())
    {
        m_codigoserie_factura = rs->value("valor");
    } // end if

    delete rs;
}
