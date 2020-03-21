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

#include "listlinfacturaview.h"
#include "blfunctions.h"
#include "blfixed.h"
#include "blplugins.h"


///
/**
\param parent
\returns
**/
ListLinFacturaView::ListLinFacturaView ( QWidget *parent ) : BfSubForm ( parent )
{
    BL_FUNC_DEBUG
    setDbTableName ( "lfactura" );
    setDbFieldId ( "idlfactura" );

    /// Disparamos los plugins.
    int res = g_plugins->run ( "ListLinFacturaView_ListLinFacturaView", this );
    if ( res != 0 )
        return;

    addSubFormHeader ( "idarticulo", BlDbField::DbInt, BlDbField::DbNotNull, BlSubFormHeader::DbHideView, _ ( "Id articulo" ) );
    addSubFormHeader ( "codigocompletoarticulo", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone, _ ( "Codigo completo articulo" ) );
    addSubFormHeader ( "nomarticulo", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNoWrite, _ ( "Nombre articulo" ) );
    addSubFormHeader ( "idlfactura", BlDbField::DbInt, BlDbField::DbPrimaryKey, BlSubFormHeader::DbHideView, _ ( "Id linea factura" ) );
    addSubFormHeader ( "desclfactura", BlDbField::DbVarChar, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "Descripcion" ) );
    addSubFormHeader ( "cantlfactura", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "Cantidad" ) );
    addSubFormHeader ( "pvplfactura", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "P.V.P." ) );
    addSubFormHeader ( "totallfactura", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNoWrite, _ ( "Total linea" ) );
    addSubFormHeader ( "ivalfactura", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "I.V.A." ) );
    addSubFormHeader ( "reqeqlfactura", BlDbField::DbNumeric, BlDbField::DbNothing, BlSubFormHeader::DbNone, _ ( "% Recargo E.Q." ) );
    addSubFormHeader ( "descuentolfactura", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "% Descuento" ) );
    addSubFormHeader ( "idfactura", BlDbField::DbInt, BlDbField::DbNotNull, BlSubFormHeader::DbHideView | BlSubFormHeader::DbNoWrite, _ ( "Id factura" ) );
    addSubFormHeader ( "ordenlfactura", BlDbField::DbInt, BlDbField::DbNotNullAutoFilled, BlSubFormHeader::DbHideView, _ ( "Orden" ) );
    setInsert ( true );
    setOrdenEnabled ( true );
    /// Disparamos los plugins.
    g_plugins->run ( "ListLinFacturaView_ListLinFacturaView_Post", this );

    
}


///
/**
\param idfactura
**/
void ListLinFacturaView::load ( QString idfactura )
{
    BL_FUNC_DEBUG
    mdb_idfactura = idfactura;
    /// Disparamos los plugins.
    int res = g_plugins->run ( "ListLinFacturaView_cargar", this );
    if ( res != 0 )
        return;
    /// TODO: revisar decimales fijos en total.
    BlSubForm::load ( "SELECT *, totallfactura(idlfactura) AS totallfactura FROM lfactura LEFT JOIN articulo ON lfactura.idarticulo = articulo.idarticulo WHERE idfactura=" + mdb_idfactura + " ORDER BY ordenlfactura" );
    
}


///
/**
 * \param column
 * \param ignoreDups
 **/
QStringList ListLinFacturaView::listRows ( QString column, bool ignoreDups )
{
    /// Este metodo nos devuelve un QStringList con los valores de las filas, para la columna escogida.
    QStringList rowList;
    BlDbSubFormRecord *line = 0;
    for ( int i = 0; i < rowCount(); ++i ) {
        line = lineaat ( i );
        if ( !line->dbValue(column).isEmpty() )
        {
            /// Recorremos los valores y los insertamos en la vase de datos
            rowList.append(line->dbValue(column));
        }
    }
    /// Si se ha pasado un valor True a ignoreDups, borramos los duplicados
    if (ignoreDups) rowList.removeDuplicates();
    line=0;
    delete line;
    return rowList;
}


