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

#include "bfproveedoralbaransubform.h"
#include "blfunctions.h"
#include "blfixed.h"
#include "blplugins.h"


///
/**
\param parent
\return
**/
BfProveedorAlbaranSubForm::BfProveedorAlbaranSubForm ( QWidget *parent ) : BfSubForm ( parent )
{
    BL_FUNC_DEBUG
    setDbTableName ( "lalbaranp" );
    setDbFieldId ( "numlalbaranp" );

    /// Disparamos los plugins.
    int res = g_plugins->run ( "BfProveedorAlbaranSubForm_BfProveedorAlbaranSubForm", this );
    if ( res != 0 )
        return;

    addSubFormHeader ( "idarticulo", BlDbField::DbInt, BlDbField::DbNotNull, BlSubFormHeader::DbHideView, _ ( "Id articulo" ) );
    addSubFormHeader ( "codigocompletoarticulo", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone, _ ( "Codigo completo articulo" ) );
    addSubFormHeader ( "nomarticulo", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNoWrite, _ ( "Nombre articulo" ) );
    addSubFormHeader ( "numlalbaranp", BlDbField::DbInt, BlDbField::DbPrimaryKey, BlSubFormHeader::DbHideView, _ ( "Numero de linea" ) );
    addSubFormHeader ( "desclalbaranp", BlDbField::DbVarChar, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "Descripcion" ) );
    addSubFormHeader ( "cantlalbaranp", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "Cantidad" ) );
    addSubFormHeader ( "pvplalbaranp", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "Precio" ) );
    addSubFormHeader ( "totallalbaranp", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNoWrite, _ ( "Total Linea" ) );
    addSubFormHeader ( "ivalalbaranp", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "% I.V.A." ) );
    addSubFormHeader ( "reqeqlalbaranp", BlDbField::DbNumeric, BlDbField::DbNothing, BlSubFormHeader::DbNone, _ ( "% R.E." ) );
    addSubFormHeader ( "descuentolalbaranp", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "% Descuento" ) );
    addSubFormHeader ( "idalbaranp", BlDbField::DbInt, BlDbField::DbNotNull, BlSubFormHeader::DbHideView | BlSubFormHeader::DbNoWrite, _ ( "Id albaran" ) );
    addSubFormHeader ( "ordenlalbaranp", BlDbField::DbInt, BlDbField::DbNotNullAutoFilled, BlSubFormHeader::DbHideView, _ ( "Orden" ) );
    setInsert ( true );
    setDelete ( true );
    setOrdenEnabled ( true );
    /// Disparamos los plugins.
    g_plugins->run ( "BfProveedorAlbaranSubForm_BfProveedorAlbaranSubForm_Post", this );
    
}


///
/**
\param isalbaranp
**/
void BfProveedorAlbaranSubForm::load ( QString idalbaranp )
{
    BL_FUNC_DEBUG
    mdb_idalbaranp = idalbaranp;
    BlSubForm::load ( "SELECT *, (cantlalbaranp * pvplalbaranp)::NUMERIC(12,2) AS totallalbaranp FROM lalbaranp LEFT JOIN articulo ON lalbaranp.idarticulo = articulo.idarticulo WHERE idalbaranp = " + mdb_idalbaranp + " ORDER BY ordenlalbaranp" );
    
}



