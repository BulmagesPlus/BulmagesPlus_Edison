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
#include <QWaitCondition>

#include "listlinpresupuestoview.h"
#include "blfunctions.h"
#include "blfixed.h"


///
/**
\param parent
\return
**/
ListLinPresupuestoView::ListLinPresupuestoView ( QWidget *parent ) : BfSubForm ( parent )
{
    BL_FUNC_DEBUG
    setDbTableName ( "lpresupuesto" );
    setDbFieldId ( "idlpresupuesto" );
    /// Disparamos los plugins.
    int res = g_plugins->run ( "ListLinPresupuestoView_ListLinPresupuestoView", this );
    if ( res != 0 ) {
        return;
    } // end if
    addSubFormHeader ( "idarticulo", BlDbField::DbInt, BlDbField::DbNotNull, BlSubFormHeader::DbHideView, _ ( "Id articulo" ) );
    addSubFormHeader ( "codigocompletoarticulo", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone, _ ( "Codigo completo" ) );
    addSubFormHeader ( "nomarticulo", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNoWrite, _ ( "Nombre del articulo" ) );
    addSubFormHeader ( "idlpresupuesto", BlDbField::DbInt, BlDbField::DbPrimaryKey, BlSubFormHeader::DbHideView, _ ( "No de linea" ) );
    addSubFormHeader ( "desclpresupuesto", BlDbField::DbVarChar, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "Descripcion" ) );
    addSubFormHeader ( "cantlpresupuesto", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "Cantidad" ) );
    addSubFormHeader ( "pvplpresupuesto", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "P.V.P." ) );
    addSubFormHeader ( "totallpresupuesto", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNoWrite, _ ( "Total Linea" ) );
    addSubFormHeader ( "ivalpresupuesto", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "% I.V.A." ) );
    addSubFormHeader ( "reqeqlpresupuesto", BlDbField::DbNumeric, BlDbField::DbNothing, BlSubFormHeader::DbNone, _ ( "% Recargo E.Q." ) );
    addSubFormHeader ( "descuentolpresupuesto", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "% Descuento" ) );
    addSubFormHeader ( "idpresupuesto", BlDbField::DbInt, BlDbField::DbNotNull, BlSubFormHeader::DbHideView | BlSubFormHeader::DbNoWrite, _ ( "Id presupuesto" ) );
    addSubFormHeader ( "ordenlpresupuesto", BlDbField::DbInt, BlDbField::DbNotNullAutoFilled, BlSubFormHeader::DbHideView, _ ( "Orden" ) );
    setInsert ( true );
    setOrdenEnabled ( true );
    setOrdenPorQuery ( false );

    /// Lanzamos los plugins
    g_plugins->run ( "ListLinPresupuestoView_ListLinPresupuestoView_Post", this );
    
}


///
/**
\param idpresupuesto
**/
void ListLinPresupuestoView::load ( QString idpresupuesto )
{
    BL_FUNC_DEBUG
    mdb_idpresupuesto = idpresupuesto;
    /// Al hacer la carge usamos el ordenlpresupuesto para indicar que el campo de ordenacion es el que toca.
    BlSubForm::load ( "SELECT *, totallpresupuesto(idlpresupuesto) AS totallpresupuesto FROM lpresupuesto LEFT JOIN articulo ON lpresupuesto.idarticulo = articulo.idarticulo WHERE idpresupuesto = " + mdb_idpresupuesto + " ORDER BY ordenlpresupuesto" );
}



