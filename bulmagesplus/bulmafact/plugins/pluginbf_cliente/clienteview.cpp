/***************************************************************************
 *   Copyright (C) 2004 by Marcos Mezo                                     *
 *   mmezo@selexco.net                                                     *
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

#include <QLineEdit>
#include <QMessageBox>
#include <QToolButton>
#include <QWidget>
#include <QComboBox>
#include <QCloseEvent>

#include "clienteview.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "blplugins.h"
#include "bfinformecliente.h"


/** Inicializa todos los componentes.
    Mete la ventana en el workSpace.
    Resetea el control de cambios.
*/
/**
\param comp
\param parent
\return
**/
ClienteView::ClienteView ( BfCompany *comp, QWidget *parent )
        : BfForm ( comp, parent )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    setAttribute ( Qt::WA_DeleteOnClose );
    try {

        setTitleName ( _ ( "Cliente" ) );
        setDbTableName ( "cliente" );
        setDbFieldId ( "idcliente" );
        addDbField ( "idcliente", BlDbField::DbInt, BlDbField::DbPrimaryKey, _ ( "ID cliente" ) );
        addDbField ( "nomcliente", BlDbField::DbVarChar, BlDbField::DbNotNull, _ ( "Nombre completo" ) );
        addDbField ( "nomaltcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Nombre alternativo del cliente" ) );
        addDbField ( "cifcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "C.I.F. del cliente" ) );
        addDbField ( "bancocliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Numero cuenta corriente" ) );
        addDbField ( "dircliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Direccion" ) );
        addDbField ( "poblcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Poblacion" ) );
        addDbField ( "idprovincia", BlDbField::DbInt, BlDbField::DbNothing, _ ( "Provincia" ) );
        addDbField ( "cpcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Codigo postal" ) );
        addDbField ( "telcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Numero de telefono" ) );
        addDbField ( "teltrabcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Numero de telefono en el trabajo" ) );
        addDbField ( "movilcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Numero de telefono movil" ) );
        addDbField ( "faxcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Numero de fax" ) );
        addDbField ( "mailcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Direccion electronica" ) );
        addDbField ( "urlcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Identificador de presupuesto" ) );
        addDbField ( "faltacliente", BlDbField::DbDate, BlDbField::DbNothing, _ ( "Fecha de alta del cliente" ) );
        addDbField ( "fbajacliente", BlDbField::DbDate, BlDbField::DbNothing, _ ( "Fecha de baja del cliente" ) );
        addDbField ( "comentcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Comentarios" ) );
        addDbField ( "ecommercedatacliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Datos de comercio electronico" ) );
        addDbField ( "inactivocliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Cliente inactivo" ) );
        addDbField ( "regimenfiscalcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Regimen fiscal" ) );
        addDbField ( "codcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Codigo" ) );
        addDbField ( "corpcliente", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Empresa" ) );
        addDbField ( "idforma_pago", BlDbField::DbInt, BlDbField::DbNothing, _ ( "Forma de pago" ) );
        addDbField ( "recargoeqcliente", BlDbField::DbBoolean, BlDbField::DbNothing, _ ( "Recargo de Equivalencia" ) );

        /// Disparamos los plugins.
        int res = g_plugins->run ( "ClienteView_ClienteView", this );
        if ( res != 0 ) {
            return;
        } // end if

        /// Datos por defecto.
        mui_idprovincia->setMainCompany ( mainCompany() );

        mui_idprovincia->setQuery ( "SELECT idprovincia, provincia||' ('||descpais||')' AS desc FROM provincia LEFT JOIN pais ON provincia.idpais = pais.idpais ORDER BY descpais, provincia" );
        mui_idprovincia->setTableName ( "provincia" );
        mui_idprovincia->setFieldId ( "idprovincia" );
        mui_idprovincia->m_valores["desc"] = "";
        mui_idprovincia->setId ( "" );

        mui_idforma_pago->setMainCompany ( mainCompany() );
        mui_idforma_pago->setId ( 0 );

        /// Metemos la ventana en el workSpace.
        insertWindow ( windowTitle(), this, false );
        dialogChanges_readValues();
        /// Disparamos los plugins.
        res = g_plugins->run ( "ClienteView_ClienteView_Post", this );
	blScript(this);
    } catch ( ... ) {
        blMsgInfo ( _ ( "Error al crear el cliente" ) );
    } // end try
    
}


/** No precisa de acciones adicionales.
*/
/**
**/
ClienteView::~ClienteView()
{
    BL_FUNC_DEBUG
    /// Disparamos los plugins.
    g_plugins->run ( "ClienteView_Des_ClienteView", this );
    mainCompany() ->removeWindow ( this );
    
}


int ClienteView::beforeSave()
{
    /// Comprueba que no existe otro cliente diferente con el mismo nombre.
    QString query = "";
    if (dbValue("idcliente") == "")
    {
      query = QString("nomcliente = '") + mainCompany()->sanearCadenaUtf8(mui_nomcliente->text()) + QString("'");
    } else {
      query = QString("nomcliente = '") + mainCompany()->sanearCadenaUtf8(mui_nomcliente->text()) + QString("' AND idcliente != ") + dbValue("idcliente");
    } // end if

    if (mainCompany()->existsAnyRecord("cliente", query))
    {
      blMsgError (_("Ya existe un cliente con este nombre."));
      return -1;
    } else {
      /// Disparamos los plugins.
      int result = g_plugins->run("ClienteView_Guardar_Pre", this);
      if (result != 0)
      {
        throw -1;
      } // end if
    } // end if

    return 0;
}


int ClienteView::afterSave()
{

	int res1;

    /// Disparamos los plugins.
    res1 = g_plugins->run ( "ClienteView_Guardar_Post", this );
	if ( res1 != 0 ) {
	    return -1;
	} // end if
	
	return 0;
}


/**
* cargar
*
* Given a valid client ID this function loads the client into the
* form so that we can edit it.
*
* Otherwise it empties the form and sets it so that we can add
* a new client
*
* Si la funcion falla imprime un mensaje de error y devuelve -1
* Si todo va bien devuelve 0
**/
/**
\param idcliente
\return
**/
int ClienteView::cargarPost ( QString idcliente )
{
    BL_FUNC_DEBUG

    /// Lanzamos los plugins de carga
    g_plugins->run ( "ClienteView_cargarPost_Post", this );

    
    return 0;
}


/** SLOT que responde a la pulsacion del boton informe de cliente.
    Crea una instancia de InformeCliente, la inicializa y la lanza.
*/
/**
**/
void ClienteView::on_mui_informe_clicked()
{
    BL_FUNC_DEBUG

    if ( dbValue ( "idcliente" ).isEmpty() ) {
        blMsgInfo ( _ ( "Tiene que guardar el documento antes de poder imprimirlo." ) );
    } else {
        BfInformeCliente inf ( mainCompany() );
        inf.setCliente ( dbValue ( "idcliente" ) );
        inf.generarInforme();
    } // end if

    
}


/**
*/
/**
**/
void ClienteView::on_mui_cifcliente_lostFocus()
{
    BL_FUNC_DEBUG
    QChar digito;
    if ( !blValidateSpainCIFNIFCode ( mui_cifcliente->text(), digito ) ) {
        blMsgInfo ( "Error en el CIF del cliente. Control:" + QString ( digito ) );
    } // end if
    
}





/**
*/
/**
**/
void ClienteView::activateDocuments()
{
    BL_FUNC_DEBUG
    mui_tab->setTabEnabled ( 2, true );
    mui_tab->setTabEnabled ( 3, true );
    mui_tab->setTabEnabled ( 4, true );
    mui_tab->setTabEnabled ( 5, true );
    mui_tab->setTabEnabled ( 6, true );

    emit activatedDocuments(this, true);
}


/**
*/
/**
**/
void ClienteView::deactivateDocuments()
{
    BL_FUNC_DEBUG
    mui_tab->setTabEnabled ( 2, false );
    mui_tab->setTabEnabled ( 3, false );
    mui_tab->setTabEnabled ( 4, false );
    mui_tab->setTabEnabled ( 5, false );
    mui_tab->setTabEnabled ( 6, false );
    
    emit deactivatedDocuments(this, false);
}

