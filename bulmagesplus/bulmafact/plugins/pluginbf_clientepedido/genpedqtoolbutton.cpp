/***************************************************************************
 *   Copyright (C) 2006 by Arturo Martin Llado                             *
 *   amartin@conetxia.com                                                  *
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

#include <QWidget>
/// Necesarios para importacion de efactura
#include <QtXml/QDomDocument>
#include <QtXml/QDomNodeList>
#include <QtXml/QDomNode>
#include <QString>
#include <QFileDialog>
#include <QMap>
#include <QList>

#include "genpedqtoolbutton.h"
#include "blfunctions.h"
#include "presupuestoview.h"
#include "pedidoclienteview.h"
#include "blfixed.h"
#include "bfcompany.h"
#include "bldb.h"


///
/**
\param pres
\param ped
\param alb
\param fac
\param parent
**/
GenPedQToolButton::GenPedQToolButton ( QWidget *fac , QWidget *parent ) : QToolButton ( parent )
{
    BL_FUNC_DEBUG
    m_object = fac;

    m_fieldListToTransfer_form = new QList< QPair<QString, QString> >;
    m_fieldListToTransfer_subForm = new QList< QPair<QString, QString> >;

    setBoton();
    
}


///
/**
**/
GenPedQToolButton::~GenPedQToolButton()
{
    BL_FUNC_DEBUG

    delete m_fieldListToTransfer_form;
    delete m_fieldListToTransfer_subForm; 
}


///
/**
**/
void GenPedQToolButton::setBoton()
{
    BL_FUNC_DEBUG
    connect ( this, SIGNAL ( clicked() ), this, SLOT ( click() ) );
    setObjectName ( QString::fromUtf8 ( "exporta" ) );
    setStatusTip ( "Generar pedido" );
    setToolTip ( "Generar pedido" );
    setMinimumSize ( QSize ( 32, 32 ) );
    setMaximumSize ( QSize ( 32, 32 ) );
    setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/client-quote-to-order.png" ) ) );
    setIconSize ( QSize ( 22, 22 ) );
    setContentsMargins ( 0, 0, 0, 0 );
    
}


///
/**
**/
void GenPedQToolButton::click()
{
    BL_FUNC_DEBUG

    if ( m_object->objectName() == "PresupuestoClienteBase" ) {
        generarFactura1();
    }// end if


    
}


/// Se encarga de generar una presupuesto a partir de un albar&aacute;n.
/** Primero de todo busca una presupuesto por referencia que tenga este presupuesto.
    Si dicha presupuesto existe entonces la cargamos y terminamos.
    Si no existe dicha presupuesto el sistema avisa y permite crear una poniendo
    Todos los datos del presupuesto automaticamente en ella.
*/
/**
\return
**/
void GenPedQToolButton::generarFactura1()
{
    BL_FUNC_DEBUG

    m_fieldListToTransfer_form->clear();
    m_fieldListToTransfer_subForm->clear();

    /// Creamos la tabla de valores a traspasar desde el albaran hasta la nueva factura.
    /// Datos del formulario
    m_fieldListToTransfer_form->append(qMakePair(QString("comentpedidocliente"), QString("comentpresupuesto")));
    m_fieldListToTransfer_form->append(qMakePair(QString("descpedidocliente"), QString("descpresupuesto")));
    m_fieldListToTransfer_form->append(qMakePair(QString("idforma_pago"), QString("idforma_pago")));
    m_fieldListToTransfer_form->append(qMakePair(QString("refpedidocliente"), QString("refpresupuesto")));
    m_fieldListToTransfer_form->append(qMakePair(QString("idcliente"), QString("idcliente")));
    m_fieldListToTransfer_form->append(qMakePair(QString("idalmacen"), QString("idalmacen")));
    m_fieldListToTransfer_form->append(qMakePair(QString("contactpedidocliente"), QString("contactpresupuesto")));
    m_fieldListToTransfer_form->append(qMakePair(QString("telpedidocliente"), QString("telpresupuesto")));
    m_fieldListToTransfer_form->append(qMakePair(QString("idtrabajador"), QString("idtrabajador")));

    
    /// Lineas de detalle del subformulario 
    m_fieldListToTransfer_subForm->append(qMakePair(QString("desclpedidocliente"), QString("desclpresupuesto")));
    m_fieldListToTransfer_subForm->append(qMakePair(QString("cantlpedidocliente"), QString("cantlpresupuesto")));
    m_fieldListToTransfer_subForm->append(qMakePair(QString("pvplpedidocliente"), QString("pvplpresupuesto")));
    m_fieldListToTransfer_subForm->append(qMakePair(QString("ivalpedidocliente"), QString("ivalpresupuesto")));
    m_fieldListToTransfer_subForm->append(qMakePair(QString("reqeqlpedidocliente"), QString("reqeqlpresupuesto")));
    m_fieldListToTransfer_subForm->append(qMakePair(QString("descuentolpedidocliente"), QString("descuentolpresupuesto")));
    m_fieldListToTransfer_subForm->append(qMakePair(QString("idarticulo"), QString("idarticulo")));
    m_fieldListToTransfer_subForm->append(qMakePair(QString("codigocompletoarticulo"), QString("codigocompletoarticulo")));
    m_fieldListToTransfer_subForm->append(qMakePair(QString("nomarticulo"), QString("nomarticulo")));

    PresupuestoView *fpv = ( PresupuestoView * ) m_object;

    /// Disparamos los plugins.
    int res = g_plugins->run ( "PresupuestoClienteView_generarPedido", this );
    if ( res != 0 )
        return;

    PedidoClienteView *bud = NULL;
    BlDbRecordSet *cur = NULL;

    try {
        /// Comprueba si disponemos de los datos m&iacute;nimos. Si no se hace esta
        /// comprobaci&oacute;n la consulta a la base de datos ser&aacute; erronea y al hacer
        /// el siguiente cur->eof() el programa fallar&aacute;.
        /// Comprobamos que existe el presupuesto con esos datos, y en caso afirmativo lo mostramos.

        QString SQLQuery = "";

        if ( fpv->dbValue ( "refpresupuesto" ).isEmpty() || fpv->dbValue ( "idcliente" ).isEmpty() ) {
            /// El presupuesto no se ha guardado y no se dispone en la base de datos
            /// de estos datos. Se utilizan en su lugar los del formulario.
            /// Verifica que exista, por lo menos, un cliente seleccionado.
            if ( fpv->mui_idcliente->id().isEmpty() ) {
                blMsgInfo ( _ ( "Tiene que seleccionar un cliente" ), this );
                return;
            } else {
                SQLQuery = "SELECT * FROM pedidocliente WHERE refpedidocliente = '" + fpv->mui_refpresupuesto->text() + "' AND idcliente = " + fpv->mui_idcliente->id();
            } // end if
        } else {
            SQLQuery = "SELECT * FROM pedidocliente WHERE refpedidocliente = '" + fpv->dbValue ( "refpresupuesto" ) + "' AND idcliente = " + fpv->dbValue ( "idcliente" );
        } // end if

        cur = fpv->mainCompany() ->loadQuery ( SQLQuery );

        if ( !cur->eof() ) {
            /// Informamos que ya hay un pedidocliente y que la abriremos.
            /// Si no salimos de la funci&oacute;n.
            if ( QMessageBox::question ( this,
                                         _ ( "Pedido ya existe" ),
                                         _ ( "Existe un pedidocliente a este cliente con la misma referencia que este pedido. Desea abrirlo para verificar?" ),
                                         _ ( "&Si" ), _ ( "&No" ), QString::null, 0, 1 ) ) {
                return;
            } // end if
            bud = new PedidoClienteView ( fpv->mainCompany(), NULL );
            fpv->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->load ( cur->value( "idpedidocliente" ) );
            bud->show();
            return;
        } // end if
        delete cur;

        /// Creamos el pedidocliente.
        bud = new PedidoClienteView ( ( BfCompany * ) fpv->mainCompany(), 0 );
        fpv->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
        bud->load ( "0" );

        /// Traspasamos los datos al pedidocliente.
        fpv->recogeValores();

	for (int j = 0; j < m_fieldListToTransfer_form->size(); ++j) {

	    bud->setDbValue ( m_fieldListToTransfer_form->at(j).first, fpv->dbValue (m_fieldListToTransfer_form->at(j).second) );
	    
	} // end for


        /// Traspasamos las lineas al pedidocliente.
        BlDbSubFormRecord *linea, *linea1;
        for ( int i = 0; i < fpv->m_listalineas->rowCount(); ++i ) {
            linea = fpv->m_listalineas->lineaat ( i );
            if ( linea->dbValue ( "idarticulo" ) != "" ) {
                linea1 = bud->getlistalineas() ->lineaat ( bud->getlistalineas() ->rowCount() - 1 );
                bud->getlistalineas() ->newRecord();
                bud->getlistalineas() ->setProcesarCambios ( false );

		for (int j = 0; j < m_fieldListToTransfer_subForm->size(); ++j) {

		    linea1->setDbValue ( m_fieldListToTransfer_subForm->at(j).first, linea->dbValue (m_fieldListToTransfer_subForm->at(j).second) );
		    
		} // end for

                bud->getlistalineas() ->setProcesarCambios ( true );
            } // end if
        } // end for

        /// Traspasamos los descuentos.
        for ( int i = 0; i < fpv->m_listadescuentos->rowCount(); ++i ) {
            linea1 = fpv->m_listadescuentos->lineaat ( i );
            if ( linea1->dbValue ( "proporciondpresupuesto" ) != "" ) {
                linea = bud->getlistadescuentos() ->lineaat ( bud->getlistadescuentos() ->rowCount() - 1 );
                bud->getlistadescuentos() ->setProcesarCambios ( false );
                linea->setDbValue ( "conceptdpedidocliente", linea1->dbValue ( "conceptdpresupuesto" ) );
                linea->setDbValue ( "proporciondpedidocliente", linea1->dbValue ( "proporciondpresupuesto" ) );
                bud->getlistadescuentos() ->setProcesarCambios ( true );
                bud->getlistadescuentos() ->newRecord();
            } // end if
        } // end for
        /// Pintamos el pedido y lo presentamos.
        bud->pintar();
        bud->calculaypintatotales();
        bud->show();

	/// Procesamos el presupuesto
	fpv->mui_procesadopresupuesto->setChecked(true);

    } catch ( ... ) {
        blMsgInfo ( _ ( "Error inesperado" ), this );
        if ( cur ) delete cur;
        if ( bud ) delete bud;
    } // end try

}




