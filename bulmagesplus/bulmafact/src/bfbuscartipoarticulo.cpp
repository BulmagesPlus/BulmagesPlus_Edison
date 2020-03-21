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

#include "bfbuscartipoarticulo.h"
#include "bfcompany.h"


/** Inicializa todos los componentes a NULL para que no haya confusiones sobre
    si un elemento ha sido inicializado o no.
*/
/**
\param parent
**/
BfBuscarTipoArticulo::BfBuscarTipoArticulo ( QWidget *parent )
        : BlSearchWidget ( parent )
{
    BL_FUNC_DEBUG

    /// Establecemos los parametros de busqueda del Cliente
    setLabel ( _ ( "Tipo Articulo:" ) );
    setTableName ( "tipo_articulo" );
    m_valores["codtipo_articulo"] = "";
    m_valores["desctipo_articulo"] = "";

    
}


/** El destructor de la clase no requiere de acciones adicionales.
*/
/**
**/
BfBuscarTipoArticulo::~BfBuscarTipoArticulo()
{
    BL_FUNC_DEBUG
    
}


/** Mediante este metodo se puede indicar cual es el tipo de articulo seleccionado.
    Busca en la base de datos el tipo que concuerde con el identificador pasado.
    Coge todos los datos necesarios para la presentacion y los muestra.
*/
/**
\param val
**/
void BfBuscarTipoArticulo::setidtipo_articulo ( QString val )
{
    BL_FUNC_DEBUG
    setId ( val );
    
}



/** Mediante este metodo se puede indicar cual es el tipo de articulo seleccionado.
    Busca en la base de datos el tipo que coincide con el codigo pasado.
    Coge todos los datos necesarios para la presentacion y los muestra.
*/
/**
\param val
**/
void BfBuscarTipoArticulo::setcodtipo_articulo ( QString val )
{
    BL_FUNC_DEBUG

    QString SQLQuery = "SELECT * FROM tipo_articulo WHERE codtipo_articulo = '" + val + "'";
    BlDbRecordSet *cur = mainCompany() ->loadQuery ( SQLQuery );

    if ( !cur->eof() ) {
        setId ( cur->value( "idtipo_articulo" ) );
    } else {
        setId ( "" );
    } // end if

    delete cur;
}



/** Devuelve el codigo de tipo de articulo seleccionado, si no hay ningun elemento seleccionado devuelve
    un string vacio
**/
/**
\return
**/
QString BfBuscarTipoArticulo::codtipo_articulo() const
{
    BL_FUNC_DEBUG

    return m_valores["codtipo_articulo"];
}


/** Devuelve el identificador del tipo de articulo seleccionado, si no hay ningun elemento seleccionado
    devuelve un string vacio
**/
/**
\return
**/
QString BfBuscarTipoArticulo::idtipo_articulo() const
{
    BL_FUNC_DEBUG

    return id();
}



/** Devuelve la descripcion del tipo de articulo seleccionado, si no hay ningun elemento seleccionado
    devuelve un string vacio
**/
/**
\return
**/
QString BfBuscarTipoArticulo::desctipo_articulo() const
{
    BL_FUNC_DEBUG

    return m_valores["desctipo_articulo"];
}
