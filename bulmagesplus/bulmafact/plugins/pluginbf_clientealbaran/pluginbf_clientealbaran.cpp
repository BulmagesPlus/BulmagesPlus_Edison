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

#include <stdio.h>
#include <QList>
#include <QPair>

#include "pluginbf_clientealbaran.h"
#include "blplugins.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "albaranclienteview.h"
#include "albaranclientelist.h"

#include "genalbqtoolbutton.h"


AlbaranClienteList *g_albaranClienteList = NULL;
BfBulmaFact *g_pluginbf_clientealbaran = NULL;



///
/**
\param bges
\return
**/
int entryPoint ( BfBulmaFact *bges )
{
    BL_FUNC_DEBUG

    /// Inicializa el sistema de traducciones 'gettext'.
    setlocale ( LC_ALL, "" );
    blBindTextDomain ( "pluginbf_clientealbaran", g_confpr->value( CONF_DIR_TRADUCCION ).toLatin1().constData() );


    if ( bges->company()->hasTablePrivilege ( "albaran", "SELECT" ) ) {

        /// Miramos si existe un menu Ventas
        QMenu *pPluginMenu = bges->newMenu ( _("&Ventas"), "menuVentas", "menuMaestro" );
        pPluginMenu->addSeparator();

        g_pluginbf_clientealbaran = bges;

        BlAction *accionA = new BlAction ( _ ( "&Albaranes a clientes" ), 0 );
        accionA->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/client-delivery-note-list.png" ) ) );
        accionA->setStatusTip ( _ ( "Albaranes a clientes" ) );
        accionA->setWhatsThis ( _ ( "Albaranes a clientes" ) );
        accionA->setObjectName("mui_actionAlbaranesClientes");
        pPluginMenu->addAction ( accionA );
        bges->Listados->addAction ( accionA );

        BlAction *accionB = new BlAction ( _ ( "&Nuevo albaran a cliente" ), 0 );
        accionB->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/client-delivery-note.png" ) ) );
        accionB->setStatusTip ( _ ( "Nuevo albaran a cliente" ) );
        accionB->setWhatsThis ( _ ( "Nuevo albaran a cliente" ) );
        accionB->setObjectName("mui_actionAlbaranClienteNuevo");
        pPluginMenu->addAction ( accionB );
        bges->Fichas->addAction ( accionB );

    }// end if


    return 0;
}


int BlAction_actionTriggered(BlAction *accion) {
    if (accion->objectName() == "mui_actionAlbaranesClientes") {
        if ( g_albaranClienteList ) {
            g_albaranClienteList->hide();
            g_albaranClienteList->show();
        }// end if
    } // end if

    if (accion->objectName() == "mui_actionAlbaranClienteNuevo") {
        AlbaranClienteView * bud = new AlbaranClienteView ( g_pluginbf_clientealbaran->company(), NULL );
        g_pluginbf_clientealbaran->company()->m_pWorkspace->addSubWindow ( bud );
        bud->inicializar();
        bud->show();

    } // end if

    return 0;
} // end if



int BfCompany_createMainWindows_Post ( BfCompany *comp )
{
    if ( comp->hasTablePrivilege ( "albaran", "SELECT" ) ) {
        g_albaranClienteList = new AlbaranClienteList ( comp, NULL );
        comp->m_pWorkspace->addSubWindow ( g_albaranClienteList );
        g_albaranClienteList->hide();
    }// end if
    return 0;
}


int ClienteView_ClienteView_Post ( ClienteView *prov )
{
    if ( prov->mainCompany()->hasTablePrivilege ( "albaran", "SELECT" ) ) {
        AlbaranClienteList *albaranesList = new AlbaranClienteList ( NULL, 0, BL_SELECT_MODE );
	albaranesList->setMainCompany(( BfCompany * ) prov->mainCompany());
        albaranesList->setEditMode();
        albaranesList->setObjectName ( "listalbaranes" );
        albaranesList->hideBusqueda();
        prov->mui_tab->addTab ( albaranesList, "Albaranes" );
    }// end if
    return 0;
}

int ClienteView_cargarPost_Post ( ClienteView *prov )
{
    if ( prov->mainCompany()->hasTablePrivilege ( "albaran", "SELECT" ) ) {
        AlbaranClienteList *albaranesList = prov->findChild<AlbaranClienteList *> ( "listalbaranes" );
        albaranesList->setidcliente ( prov->dbValue ( "idcliente" ) );
        albaranesList->presentar();
    }// end if
    return 0;
}// end if


int BfBuscarReferencia_on_mui_abrirtodo_clicked_Post ( BfBuscarReferencia *ref )
{
    QString SQLQuery = "SELECT * FROM albaran WHERE refalbaran = '" + ref->mui_referencia->text() + "'";
    BlDbRecordSet *cur = ref->mainCompany() ->loadQuery ( SQLQuery );
    while ( !cur->eof() ) {
        AlbaranClienteView * bud = new AlbaranClienteView ( ( BfCompany * ) ref->mainCompany(), NULL );
        ref->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
        bud->load ( cur->value( "idalbaran" ) );
        bud->show();
        cur->nextRecord();
    } // end while
    delete cur;
    return 0;
}// end if


///
/**
\param l
\return
**/
int PedidoClienteView_PedidoClienteView ( PedidoClienteView *l )
{

    BL_FUNC_DEBUG

    GenAlbQToolButton *mui_exporta_efactura2 = new GenAlbQToolButton ( l, l->mui_plugbotones );

    QHBoxLayout *m_hboxLayout1 = l->mui_plugbotones->findChild<QHBoxLayout *> ( "hboxLayout1" );

    if ( !m_hboxLayout1 ) {
        m_hboxLayout1 = new QHBoxLayout ( l->mui_plugbotones );
        m_hboxLayout1->setSpacing ( 5 );
        m_hboxLayout1->setMargin ( 0 );
        m_hboxLayout1->setObjectName ( QString::fromUtf8 ( "hboxLayout1" ) );
    }// end if
    m_hboxLayout1->addWidget ( mui_exporta_efactura2 );

    

    return 0;
}


///
/**
\param l
\return
**/
int PresupuestoView_PresupuestoView ( PresupuestoView *l )
{

    BL_FUNC_DEBUG

    GenAlbQToolButton *mui_exporta_efactura2 = new GenAlbQToolButton ( l, l->mui_plugbotones );

    QHBoxLayout *m_hboxLayout1 = l->mui_plugbotones->findChild<QHBoxLayout *> ( "hboxLayout1" );

    if ( !m_hboxLayout1 ) {
        m_hboxLayout1 = new QHBoxLayout ( l->mui_plugbotones );
        m_hboxLayout1->setSpacing ( 5 );
        m_hboxLayout1->setMargin ( 0 );
        m_hboxLayout1->setObjectName ( QString::fromUtf8 ( "hboxLayout1" ) );
    }// end if
    m_hboxLayout1->addWidget ( mui_exporta_efactura2 );

    

    return 0;
}




///
/**
\param l
\return
**/
int FacturaView_FacturaView ( FacturaView *l )
{

    BL_FUNC_DEBUG

    BlToolButton *agalbaran = new BlToolButton ( l );
    agalbaran->setObjectName("mui_agalbaran12");
    
    agalbaran->setStatusTip ( "Agregar albaran" );
    agalbaran->setToolTip ( "Agregar albaran" );
    agalbaran->setMinimumSize ( QSize ( 32, 32 ) );
    agalbaran->setMaximumSize ( QSize ( 32, 32 ) );
    agalbaran->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/client-delivery-note-add-to-invoice.png" ) ) );
    agalbaran->setIconSize ( QSize ( 22, 22 ) );
    agalbaran->setContentsMargins ( 0, 0, 0, 0 );
    
    
    QHBoxLayout *m_hboxLayout1 = l->mui_plugbotones->findChild<QHBoxLayout *> ( "hboxLayout1" );

    if ( !m_hboxLayout1 ) {
        m_hboxLayout1 = new QHBoxLayout ( l->mui_plugbotones );
        m_hboxLayout1->setSpacing ( 5 );
        m_hboxLayout1->setMargin ( 0 );
        m_hboxLayout1->setObjectName ( QString::fromUtf8 ( "hboxLayout1" ) );
    }// end if
    
    m_hboxLayout1->addWidget ( agalbaran );

    

    return 0;
}


int BlToolButton_released(BlToolButton *bot) {

  

  if (bot->objectName() == "mui_agalbaran12") {
    
	QList<QPair<QString, QString> > *fieldListToTransfer_subForm;
	
	
	fieldListToTransfer_subForm = new QList< QPair<QString, QString> >;
	fieldListToTransfer_subForm->clear();
	
	fieldListToTransfer_subForm->append(qMakePair(QString("codigocompletoarticulo"), QString("codigocompletoarticulo")));
	fieldListToTransfer_subForm->append(qMakePair(QString("desclfactura"), QString("desclalbaran")));
	fieldListToTransfer_subForm->append(qMakePair(QString("cantlfactura"), QString("cantlalbaran")));
	fieldListToTransfer_subForm->append(qMakePair(QString("pvplfactura"), QString("pvplalbaran")));
	fieldListToTransfer_subForm->append(qMakePair(QString("ivalfactura"), QString("ivalalbaran")));
	fieldListToTransfer_subForm->append(qMakePair(QString("reqeqlfactura"), QString("reqeqlalbaran")));
	fieldListToTransfer_subForm->append(qMakePair(QString("descuentolfactura"), QString("descuentolalbaran")));
	fieldListToTransfer_subForm->append(qMakePair(QString("idarticulo"), QString("idarticulo")));
	fieldListToTransfer_subForm->append(qMakePair(QString("nomarticulo"), QString("nomarticulo")));


	
	FacturaView *fact1 = (FacturaView *) bot->parent()->parent()->parent();

	/// Disparamos los plugins.
	int res = g_plugins->run ( "FacturaView_agregarFactura", fieldListToTransfer_subForm );
	if ( res != 0 )
	    return 1;
      

	/// AGREGAR ALBARAN A LA FACTURA
    
        QDialog *diag = new QDialog ( 0 );
	diag->setModal ( true );
	AlbaranClienteList *fac = new AlbaranClienteList ( fact1->mainCompany(), diag, 0, BL_SELECT_MODE );
	

	QObject::connect ( fac, SIGNAL ( selected ( QString ) ), diag, SLOT ( accept() ) );

	/// Hacemos que las opciones de filtrado del listado ya esten bien.
	fac->m_cliente->setId ( fact1->dbValue ( "idcliente" ) );
	fac->on_mui_actualizar_clicked();

	/// Lanzamos el dialogo.
	diag->exec();
	QString idalbaran = fac->idCliDelivNote();
	delete diag;

	/// Si no hay idfactura es que hemos abortado y por tanto cancelamos la operacion.
	if ( idalbaran == "" )
	    return 1;

	/// Creamos la factura.
	AlbaranClienteView *bud = new AlbaranClienteView ( fact1->mainCompany(), NULL );
	bud->load ( idalbaran );

	/// Agregamos a comentarios que albaran se corresponde.
	QString comm = fact1->dbValue ( "comentfactura" ) + "(" + _ ( "ALBARAN: Num " ) + bud->dbValue ( "numalbaran" ) + _ ( "Ref:" ) + " " + bud->dbValue ( "refalbaran" ) + _ ( "Fecha:" ) + " " + bud->dbValue ( "fechaalbaran" ) + ")\n";

	fact1->setDbValue ( "comentfactura", comm );
	fact1->pintar();

	/// EN TEORIA SE DEBERIA COMPROBAR QUE LA FACTURA Y EL ALBARAN SON DEL MISMO CLIENTE, pero por ahora no lo hacemos.
	BlDbSubFormRecord *linea, *linea1;
	for ( int i = 0; i < bud->getlistalineas() ->rowCount(); ++i ) {
	    linea = bud->getlistalineas() ->lineaat ( i );
	    /// Los registros vacios no se tienen en cuenta.
	    if ( linea->dbValue ( "idarticulo" ) != "" ) {
	      
		linea1 = fact1->getlistalineas() ->lineaat ( fact1->getlistalineas() ->rowCount() - 1 );

		for (int j = 0; j < fieldListToTransfer_subForm->size(); ++j) {

		    linea1->setDbValue ( fieldListToTransfer_subForm->at(j).first, linea->dbValue (fieldListToTransfer_subForm->at(j).second) );
		    
		} // end for
		
		fact1->getlistalineas() ->newRecord();
	    } // end if
	} // end for

	/// Procesamos el albaran.
	bud->mui_procesadoalbaran->setChecked ( true );
	bud->save();
	delete bud;
	delete fieldListToTransfer_subForm;

	/// Pintamos los totales.
	fact1->calculaypintatotales();
    
	
  } // end if
	
  
  return 0;
}


/// Esta llamada de plugin es bastante novedosa ya es una llamada que no responde a una funcion
/// Sino que se llama desde multiples partes del sistema.
int SNewAlbaranClienteView ( BfCompany *v )
{
    AlbaranClienteView *h = new AlbaranClienteView ( v, 0 );
    g_plugParams = h;
    return 1;
}

