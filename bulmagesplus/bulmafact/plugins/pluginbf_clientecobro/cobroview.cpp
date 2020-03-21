/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borras Riera                              *
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

#include <fstream>

#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>

#include "cobroview.h"
#include "bfcompany.h"
#include "blconfiguration.h"
#include "bldatesearch.h"
#include "blfunctions.h"


/** inicializa todos los componentes de la clase.
    Resetea el sistema de control de cambios para que considere que no hay cambios por parte del usuario.
    Mete la ventana en el workSpace.
*/
CobroView::CobroView ( BfCompany *comp, QWidget *parent )
        : BfForm ( comp, parent )
{
    BL_FUNC_DEBUG
    setAttribute ( Qt::WA_DeleteOnClose );
    try {
        setupUi ( this );
        blCenterOnScreen ( this );
        /// Usurpamos la identidad de mlist y ponemos nuestro propio widget con sus cosillas.
        mui_idcliente->setMainCompany ( comp );
        mui_refcobro->setMainCompany ( comp );
        mui_idforma_pago->setMainCompany ( comp );
        mui_idtrabajador->setMainCompany ( comp );
      
        /// Establecemos los parametros de busqueda del Cliente
        mui_idcliente->setLabel ( _ ( "Cliente *:" ) );
        mui_idcliente->setTableName ( "cliente" );
        mui_idcliente->m_valores["cifcliente"] = "";
        mui_idcliente->m_valores["nomcliente"] = "";
        mui_idtrabajador->setFieldValue ( "0" );

        setTitleName ( _ ( "Cobro" ) );
        setDbTableName ( "cobro" );
        setDbFieldId ( "idcobro" );
        addDbField ( "idcobro", BlDbField::DbInt, BlDbField::DbPrimaryKey, _ ( "Id cobro" ) );
        addDbField ( "idcliente", BlDbField::DbInt, BlDbField::DbNotNull, _ ( "Cliente" ) );
        addDbField ( "previsioncobro", BlDbField::DbBoolean, BlDbField::DbNothing, _ ( "Prevision" ) );
        addDbField ( "fechacobro", BlDbField::DbDate, BlDbField::DbNothing, _ ( "Fecha del cobro" ) );
        addDbField ( "fechavenccobro", BlDbField::DbDate, BlDbField::DbNothing, _ ( "Fecha de vencimiento" ) );
        addDbField ( "refcobro", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Referencia" ) );
        addDbField ( "cantcobro", BlDbField::DbNumeric, BlDbField::DbNotNull, _ ( "Total cobro" ) );
        addDbField ( "comentcobro", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Descripcion del cobro" ) );
        addDbField ( "idforma_pago", BlDbField::DbInt, BlDbField::DbNothing, _ ( "Forma de Pago" ) );
        addDbField ( "idtrabajador", BlDbField::DbInt, BlDbField::DbNothing, _ ( "Trabajador" ) );

        insertWindow ( windowTitle(), this, false );
        pintar();
        dialogChanges_readValues();
	blScript(this);
    } catch ( ... ) {
        blMsgInfo ( _ ( "Error al crear el cobro" ), this );
    } // end try
    
}


/** No precisa acciones adicionales en el destructor.
*/
CobroView::~CobroView()
{
    BL_FUNC_DEBUG
    
}

int CobroView::parseTags ( QString &buff )
{
    // pillar los datos de cliente equivale a un <!--QUERY-->      implicito que contiene toda la plantilla.
    QString params[1] = {dbValue ( "idcliente" ) };
    buff = parseRecordset ( mainCompany() ->loadQuery (
                             "SELECT * FROM cliente WHERE idcliente = $1" ,
                             1, params , NULL, 0, 0 ), buff );

    // mantenemos compatibilidad con RML preexistentes
    // que usen estos nombres de variable
    buff.replace ( "[referencia]" , blXMLEscape ( dbValue ( "refcobro" ) ) );
    buff.replace ( "[cantidad]" , blXMLEscape ( dbValue ( "cantcobro" ) ) );
    buff.replace ( "[comentario]" , blXMLEscape ( dbValue ( "comentcobro" ) ) );
    buff.replace ( "[fecha]" , blXMLEscape ( dbValue ( "fechacobro" ) ) );

    // admitimos sustituciones , QUERY, IFACE, SETVAR... y los
    // nombres de campos de BD para cobros
    BfForm::parseTags ( buff );
    return 0;
}

QString CobroView::templateName ( void )
{
    return QString ( "recibo" );
}

void CobroView::imprimir()
{
    BL_FUNC_DEBUG
    /// Comprobamos que se disponen de los datos minimos para imprimir el recibo.
    QString SQLQuery = "";

    if ( dbValue ( "idcliente" ).isEmpty() ) {
        /// El documento no se ha guardado y no se dispone en la base de datos de estos datos.
        blMsgInfo ( _ ( "Tiene que guardar el documento antes de poder imprimirlo." ), this );
        return;
    }
    /// Disparamos los plugins
    int res = g_plugins->run ( "CoboView_on_mui_imprimir_clicked", this );
    if ( res != 0 ) {
        return;
    } // end if
    BfForm::imprimir();

    
}


/** Cuando hacemos un cobro y el valor del cobro es igual por referencia que todas las facturas
     con la misma referencia se procesan dichas facturas **/
int CobroView::afterSave()
{
    BL_FUNC_DEBUG
    
    QString refCobro = dbValue("refcobro");
    
    if (!refCobro.isEmpty())
    {
      QString query1 = "SELECT COALESCE(sum (cantcobro), 0) AS totalc FROM cobro WHERE refcobro='" + refCobro + "'";
      BlDbRecordSet *cur1 = mainCompany()->loadQuery ( query1 );

      QString query = "SELECT COALESCE(sum(totalfactura), 0) AS total FROM factura WHERE reffactura='" + refCobro + "'";
      BlDbRecordSet *cur = mainCompany()->loadQuery ( query );

      if ( cur->value( "total" ) == cur1->value( "totalc" ) ) {

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(_("Referencia cobrada completamente"));
        msgBox.setInformativeText(_("Desea marcar como 'procesadas' todas las facturas con esta referencia?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Ok) {
          QString query2 = "UPDATE factura set procesadafactura = TRUE WHERE reffactura='" + dbValue ( "refcobro" ) + "'";
          mainCompany()->runQuery ( query2 );
        } // end if
      } // end if
      delete cur;
      delete cur1;
    } // end if

    return 0;
}

