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
#include <QMenu>
#include <QKeyEvent>
#include <QEvent>

#include "bfclientealbarandescuentosubform.h"
#include "blfunctions.h"


///
/**
\param parent
**/
BfClienteAlbaranDescuentoSubForm::BfClienteAlbaranDescuentoSubForm ( QWidget *parent )
        : BfSubForm ( parent )
{
    BL_FUNC_DEBUG

    setDbTableName ( "dalbaran" );
    setDbFieldId ( "iddalbaran" );
    addSubFormHeader ( "iddalbaran", BlDbField::DbInt, BlDbField::DbPrimaryKey, BlSubFormHeader::DbHideView | BlSubFormHeader::DbNoWrite, _ ( "Id descuento" ) );
    addSubFormHeader ( "conceptdalbaran", BlDbField::DbVarChar, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "Concepto descuento" ) );
    addSubFormHeader ( "proporciondalbaran", BlDbField::DbNumeric, BlDbField::DbNotNull, BlSubFormHeader::DbNone, _ ( "% Descuento" ) );
    addSubFormHeader ( "idalbaran", BlDbField::DbInt, BlDbField::DbNotNull, BlSubFormHeader::DbHideView | BlSubFormHeader::DbNoWrite, _ ( "Id albaran" ) );
    setInsert ( true );
    
}


///
/**
\param idalbaran
**/
void BfClienteAlbaranDescuentoSubForm::load ( QString idalbaran )
{
    BL_FUNC_DEBUG
    mdb_idalbaran = idalbaran;
    BlSubForm::load ( "SELECT * FROM dalbaran WHERE idalbaran=" + mdb_idalbaran );
}
