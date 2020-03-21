/***************************************************************************
 *   Copyright (C) 2004 by J. M. Estopa Rey                                *
 *   pepma@telefonica.net                                                  *
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
#include <QTextStream>
#include <QFileDialog>
#include <QCheckBox>
#include <QMessageBox>
#include <QMenu>

#include "clientslist.h"
#include "clienteview.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "blimportexport.h"
#include "blplugins.h"


/** Inicializa todos los componentes del listado.
    Hace una presentacion inicial.
    Mete la ventana en el workSpace si estamos en modo edicion.
*/
ClientsList::ClientsList ( BfCompany *comp, QWidget *parent, Qt::WindowFlags flag, edmode editmode )
        : BlFormList ( comp, parent, flag, editmode ), BlImportExport ( comp )
{
    BL_FUNC_DEBUG
    setupUi ( this );

    /// Disparamos los plugins.
    int res = g_plugins->run ( "ClientsList_ClientsList", this );
    if ( res != 0 )
        return;

    mui_list->setMainCompany ( comp );
    setSubForm ( mui_list );
    mdb_idcliente = "";
    mdb_cifcliente = "";
    mdb_nomcliente = "";
    hideBusqueda();
    /// Si estamos en el modo edici&oacute;n metemos la ventana en el workSpace.
    if ( editMode() ) {
        mainCompany() ->insertWindow ( windowTitle(), this );
    } else {
        setWindowTitle ( _ ( "Selector de clientes" ) );
        mui_editar->setHidden ( true );
        mui_borrar->setHidden ( true );
        mui_exportar->setHidden ( true );
        mui_importar->setHidden ( true );
        mui_imprimir->setHidden ( true );
    } // end if
    
    /// Cargamos los filtros guardados.
    cargaFiltrosXML();
    
    presentar();
    /// Hacemos el tratamiento de los permisos que desabilita botones en caso de no haber suficientes permisos.
    trataPermisos ( "cliente" );
    /// Llamamos a los scripts
    blScript(this);
    
}


/** No requiere acciones especiales en el destructor de la clase.
*/
ClientsList::~ClientsList()
{
    BL_FUNC_DEBUG
    
}


/** Todo el listado de clientes lo presenta el Subformulario mui_list del tipo ListLinCliente.
*/
/// \TODO: Mejorar el sistema de filtrado incluyendo una funcion de generar Filtro.
void ClientsList::presentar()
{
    BL_FUNC_DEBUG
    mui_list->load ( "SELECT * FROM cliente WHERE 1=1 " + generaFiltro() );
    
}

/** Metodo auxiliar que crea la clausula WHERE del query de carga  \ref presenta()
    La clausula WHERE utiliza todas las opciones de filtrado para crearse. */
/**
\return
**/
const QString ClientsList::generaFiltro()
{
    BL_FUNC_DEBUG
    /// Tratamiento de los filtros.
    QString filtro = "";
    
    /// Hacemos el filtrado like del campo m_filtro
    filtro += mui_list->likeFilterSQL(m_filtro->text());
    
    if ( m_facturas->isChecked()) {
        filtro += " AND idcliente IN (SELECT DISTINCT idcliente FROM factura WHERE procesadafactura = FALSE)";
    } // end if

    if ( m_albaranes->isChecked()) {
        filtro += " AND idcliente IN (SELECT DISTINCT idcliente FROM albaran WHERE procesadoalbaran = FALSE)";
    } // end if
    
    
    return ( filtro );
}


/** LA accion de editar tiene una doble vertiente dependiendo del modo en que se ha abierto la ventana.
    Modo Edicion:
 - Crea una instancia de ClienteView, lo inicializa y lo presenta.
    Modo Seleccion:
 - Establece cual es el elemento seleccionado y se cierra la ventana para devolver el control.
*/
void ClientsList::editar ( int row )
{
    BL_FUNC_DEBUG
    mdb_idcliente = mui_list->dbValue ( "idcliente", row );
    mdb_cifcliente = mui_list->dbValue ( "cifcliente", row );
    mdb_nomcliente = mui_list->dbValue ( "nomcliente", row );
    if ( editMode() ) {
        ClienteView * prov = new ClienteView ( ( BfCompany * ) mainCompany(), 0 ) ;
        if ( prov->load ( mdb_idcliente ) ) {
            delete prov;
            return;
        } // end if
        mainCompany() ->m_pWorkspace->addSubWindow ( prov );
        prov->show();
    } else {
        emit ( selected ( mdb_idcliente ) );
    } // end if
    
}


/** SLOT que responde a la pulsacion del boton imprimir.
    La impresion de listados esta completamente delegada a SubForm3
*/
void ClientsList::imprimir()
{
    BL_FUNC_DEBUG
    mui_list->printPDF ( _ ( "Listado de Clientes" ) );
    
}



/** SLOT que responde a la pulsacion del boton borrar.
    Instancia la clase ClienteView, lo inicializa con el cliente seleccionado y le lanza el evento de borrar.
    Esta es la forma adecuada de borrar desde el listado ya que asi preservamos el tema plugins.
*/
void ClientsList::remove()
{
    BL_FUNC_DEBUG
    try {
        QString idcliente = mui_list->dbValue ( "idcliente" );
        ClienteView *cli = new ClienteView ( ( BfCompany * ) mainCompany(), 0 ) ;
        if ( cli->load ( idcliente ) ) {
            delete cli;
            throw - 1;
        } // end if
        cli->on_mui_borrar_clicked();
        delete cli;
        presentar();
    } catch ( ... ) {
        blMsgInfo ( _ ( "Error al borrar un cliente" ) );
    } // end try
    
}


/** SLOT que responde a la pulsacion del boton exportar.
    Saca un dialog selector de archivo para que indiquemos a que archivo exportar.
    Llama a bulmafact2XML para hacer la exportacion.
*/
void ClientsList::on_mui_exportar_clicked()
{
    BL_FUNC_DEBUG
    QFile filexml ( QFileDialog::getSaveFileName (
                        this,
                        _ ( "Elija el archivo" ),
                        g_confpr->value( CONF_DIR_USER ),
                        _ ( "Clientes (*.xml)" ) ) );
    if ( filexml.open ( QIODevice::WriteOnly ) ) {
        bulmafact2XML ( filexml, IMPORT_CLIENTES );
        filexml.close();
    } else {
	BlDebug::blDebug ( Q_FUNC_INFO, 0, _("Error al abrir el archivo.") );
    } // end if
    
}


/** SLOT que responde a la pulsacion del boton importar.
    Saca un selector de archivos para indicar desde que archivo importar.
    Llama a XML2BulmaFact para hacer la importacion.
    Refresca el listado.
*/
void ClientsList::on_mui_importar_clicked()
{
    BL_FUNC_DEBUG
    QFile filexml ( QFileDialog::getOpenFileName (
                        this,
                        _ ( "Elija el archivo" ),
                        g_confpr->value( CONF_DIR_USER ),
                        _ ( "Clientes (*.xml)" ) ) );

    if ( filexml.open ( QIODevice::ReadOnly ) ) {
        XML2BulmaFact ( filexml, IMPORT_CLIENTES );
        filexml.close();
        presentar();
    } else {
	BlDebug::blDebug ( Q_FUNC_INFO, 0, _("Error al abrir el archivo.") );
    } // end if
    
}


/** Devuelve el identificador del cliente seleccionado
**/
QString ClientsList::idclient()
{
    return mdb_idcliente;
}

/** Devuelve el nombre del cliente seleccionado
**/
QString ClientsList::nomclient()
{
    return mdb_nomcliente;
}

/** Devuelve el cif del cliente seleccionado
**/
QString ClientsList::cifclient()
{
    return mdb_cifcliente;
}


/** SLOT automatico que se ejecuta al pulsar sobre el boton de crear en la botonera
**/
void ClientsList::crear()
{
    BL_FUNC_DEBUG
    if (selectMode()) {
	/// El modo consulta funciona algo diferente
        QDialog *diag = new QDialog ( 0 );
        diag->setModal ( true );
        diag->setGeometry ( QRect ( 0, 0, 750, 550 ) );
        blCenterOnScreen ( diag );

	ClienteView *bud = new ClienteView ( ( BfCompany * ) mainCompany(), 0 );
        /// Creamos un layout donde estara el contenido de la ventana y la ajustamos al QDialog
        bud->connect ( bud, SIGNAL ( destroyed ( QObject * ) ), diag, SLOT ( accept() ) );

        /// para que sea redimensionable y aparezca el titulo de la ventana.
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget ( bud );
        layout->setMargin ( 0 );
        layout->setSpacing ( 0 );
        diag->setLayout ( layout );
        diag->setWindowTitle ( bud->windowTitle() );

	bud->show();
	bud->pintar();

	QString idclienteold = "";
	BlDbRecordSet *curold = mainCompany()->loadQuery("SELECT max(idcliente) AS id FROM cliente");
	if( !curold->eof()) {
		      idclienteold = curold->value("id");
	} // end if
	delete curold;
	
	bud->mui_cifcliente->setFocus ( Qt::OtherFocusReason );	
        diag->exec();      
      
	BlDbRecordSet *cur = mainCompany()->loadQuery("SELECT max(idcliente) AS id FROM cliente");
	if( !cur->eof()) {
		  if (cur->value("id") != idclienteold) {
		      mdb_idcliente = cur->value("id");
	              emit ( selected ( mdb_idcliente ) );
		  } // end if
	} // end if
	delete cur;
	
    } else {
	ClienteView *bud = new ClienteView ( ( BfCompany * ) mainCompany() , 0 );
	mainCompany()->m_pWorkspace->addSubWindow ( bud );
	bud->pintar();
	bud->show();
	bud->setWindowTitle ( _ ( "Nuevo cliente" ) );
	/// Deshabilitamos las pestanyas que no se utilizan al crear un nuevo cliente.
	bud->deactivateDocuments();
	bud->mui_cifcliente->setFocus ( Qt::OtherFocusReason );
    } // end if
    

}


/** \TODO: REVISAR ESTE METODO YA QUE NO PARECE SER EL ADECUADO
    EN LA LLAMADA DE SUBMENUS
*/
void ClientsList::submenu ( const QPoint & )
{
    BL_FUNC_DEBUG
    int a = mui_list->currentRow();
    if ( a < 0 )
        return;
    QMenu *popup = new QMenu ( this );
    QAction *edit = popup->addAction ( _ ( "Editar cliente" ) );
    QAction *del = popup->addAction ( _ ( "Borrar cliente" ) );
    QAction *opcion = popup->exec ( QCursor::pos() );
    if ( opcion == del )
        on_mui_borrar_clicked();
    if ( opcion == edit )
        on_mui_editar_clicked();
    delete popup;
    
}

/// =============================================================================
///                    SUBFORMULARIO
/// =============================================================================
/** PRepara el subformulario para trabajar con la tabla cliente.
    Establece que no se puedan insertar ni borrar lineas.
*/
ClienteListSubform::ClienteListSubform ( QWidget *parent, const char * ) : BfSubForm ( parent )
{
    BL_FUNC_DEBUG
    setDbTableName ( "cliente" );
    setDbFieldId ( "idcliente" );
    addSubFormHeader ( "idcliente", BlDbField::DbInt, BlDbField::DbNotNull | BlDbField::DbPrimaryKey, BlSubFormHeader::DbHideView | BlSubFormHeader::DbNoWrite, _ ( "ID cliente" ) );
    addSubFormHeader ( "codcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Codigo" ) );
    addSubFormHeader ( "cifcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "C.I.F." ) );
    addSubFormHeader ( "nomcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Nombre de cliente" ) );
    addSubFormHeader ( "nomaltcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Nombre alternativo del cliente" ) );
    addSubFormHeader ( "bancocliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Numero cuenta corriente" ) );
    addSubFormHeader ( "dircliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Direccion" ) );
    addSubFormHeader ( "poblcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Poblacion" ) );
    addSubFormHeader ( "cpcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Codigo postal" ) );
    addSubFormHeader ( "telcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Numero de telefono" ) );
    addSubFormHeader ( "movilcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Numero de movil" ) );
    addSubFormHeader ( "faxcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Numero de fax" ) );
    addSubFormHeader ( "mailcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Direccion de correo electronica" ) );
    addSubFormHeader ( "urlcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Direccion URL" ) );
    addSubFormHeader ( "corpcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Empresa" ) );
    addSubFormHeader ( "faltacliente", BlDbField::DbDate, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Fecha de alta del cliente" ) );
    addSubFormHeader ( "fbajacliente", BlDbField::DbDate, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Fecha de baja del cliente" ) );
    addSubFormHeader ( "comentcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Comentarios" ) );
    setInsert ( false );
    setDelete ( false );
    setSortingEnabled ( true );
    
}

ClienteListSubform::~ClienteListSubform()
{
    BL_FUNC_DEBUG
    
}
