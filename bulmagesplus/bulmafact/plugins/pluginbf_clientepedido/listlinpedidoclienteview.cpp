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

#include <QMessageBox>
#include <QKeyEvent>
#include <QEvent>

#include "listlinpedidoclienteview.h"
#include "blfunctions.h"
#include "blfixed.h"


///
/**
\param parent
\return
**/
ListLinPedidoClienteView::ListLinPedidoClienteView ( QWidget *parent ) : BfSubForm ( parent )
{
    BL_FUNC_DEBUG
    setDbTableName ( "lpedidocliente" );
    setDbFieldId ( "numlpedidocliente" );
    /// Disparamos los plugins.
    int res = g_plugins->run ( "ListLinPedidoClienteView_ListLinPedidoClienteView", this );
    if ( res != 0 ) {
        return;
    } // end if
    addSubFormHeader ( "puntlpedidocliente", BlDbField::DbBoolean, BlDbField::DbNothing, BlSubFormHeader::DbNone, _ ( "Punteo" ) );
    addSubFormHeader ( "idarticulo", BlDbField::DbInt, BlDbField::DbNotNull, BlSubFormHeader::DbHideView, _ ( "Id articulo" ) );
    addSubFormHeader ( "codigocompletoarticulo", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone, _ ( "Codigo completo articulo" ) );
    addSubFormHeader ( "nomarticulo", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNoWrite, _ ( "Nombre articulo" ) );
    addSubFormHeader ( "numlpedidocliente", BlDbField::DbInt, BlDbField::DbPrimaryKey, BlSubFormHeader::DbHideView, _ ( "No de linea" ) );
    addSubFormHeader ( "desclpedidocliente", BlDbField::DbVarChar, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "Descripcion" ) );
    addSubFormHeader ( "cantlpedidocliente", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "Cantidad" ) );
    addSubFormHeader ( "pvplpedidocliente", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "P.V.P." ) );
    addSubFormHeader ( "totallpedidocliente", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNoWrite, _ ( "Total Linea" ) );
    addSubFormHeader ( "ivalpedidocliente", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "% I.V.A." ) );
    addSubFormHeader ( "reqeqlpedidocliente", BlDbField::DbNumeric, BlDbField::DbNothing, BlSubFormHeader::DbNone, _ ( "% Recargo E.Q." ) );
    addSubFormHeader ( "descuentolpedidocliente", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "% Descuento" ) );
    addSubFormHeader ( "idpedidocliente", BlDbField::DbInt, BlDbField::DbNotNull, BlSubFormHeader::DbHideView | BlSubFormHeader::DbNoWrite, _ ( "Id pedido cliente" ) );
    addSubFormHeader ( "ordenlpedidocliente", BlDbField::DbInt, BlDbField::DbNotNullAutoFilled, BlSubFormHeader::DbHideView, _ ( "Orden" ) );

    /// Establecemos los modos de comportamiento del subformulario
    setInsert ( true );
    setOrdenEnabled ( true );
    setOrdenPorQuery ( false );

    /// Lanzamos los plugins
    g_plugins->run ( "ListLinPedidoClienteView_ListLinPedidoClienteView_Post", this );
    
}


///
/**
\param idpedidocliente
**/
void ListLinPedidoClienteView::load ( QString idpedidocliente )
{
    BL_FUNC_DEBUG
    mdb_idpedidocliente = idpedidocliente;
    BlSubForm::load ( "SELECT *, totallpedidocliente(numlpedidocliente) AS totallpedidocliente FROM lpedidocliente LEFT JOIN articulo ON lpedidocliente.idarticulo = articulo.idarticulo WHERE idpedidocliente = " + mdb_idpedidocliente + " ORDER BY ordenlpedidocliente" );
    
}



