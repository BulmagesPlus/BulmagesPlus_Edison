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

#include "pluginbf_clientepedido.h"
#include "blplugins.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "pedidoclienteview.h"
#include "pedidosclientelist.h"

#include "genpedqtoolbutton.h"


PedidosClienteList *g_pedidosClienteList = NULL;
BfBulmaFact *g_pluginbf_clientepedido = NULL;



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
    blBindTextDomain ( "pluginbf_clientepedido", g_confpr->value( CONF_DIR_TRADUCCION ).toLatin1().constData() );
    g_pluginbf_clientepedido = bges;
    
    if ( bges->company()->hasTablePrivilege ( "pedidocliente", "SELECT" ) ) {
        /// Miramos si existe un menu Ventas
        QMenu *pPluginMenu = bges->newMenu ( _("&Ventas"), "menuVentas", "menuMaestro" );
        pPluginMenu->addSeparator();

        BlAction *accionA = new BlAction ( _ ( "&Pedidos de clientes" ), 0 );
        accionA->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/client-delivery-order-list.png" ) ) );
        accionA->setStatusTip ( _ ( "Pedidos de clientes" ) );
        accionA->setWhatsThis ( _ ( "Pedidos de clientes" ) );
        accionA->setObjectName("mui_actionClientePedido");

        pPluginMenu->addAction ( accionA );
        bges->Listados->addAction ( accionA );

        BlAction *accionB = new BlAction ( _ ( "&Nuevo pedido de cliente" ), 0 );
        accionB->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/client-delivery-order.png" ) ) );
        accionB->setStatusTip ( _ ( "Nuevo pedido de cliente" ) );
        accionB->setWhatsThis ( _ ( "Nuevo pedido de cliente" ) );
        accionB->setObjectName("mui_actionClientePedidoNuevo");

        pPluginMenu->addAction ( accionB );


    } // end if
    
    return 0;
}


int BlAction_actionTriggered(BlAction *accion) {
    if (accion->objectName() == "mui_actionClientePedido") {
        if ( g_pedidosClienteList ) {
            g_pedidosClienteList->hide();
            g_pedidosClienteList->show();
        } // end if
    } // end if
    
    if (accion->objectName() == "mui_actionClientePedidoNuevo") {
        PedidoClienteView * bud = new PedidoClienteView ( g_pluginbf_clientepedido->company(), NULL );
        g_pluginbf_clientepedido->company() ->m_pWorkspace->addSubWindow ( bud );
        bud->inicializar();
        bud->show();
    
    } // end if
    return 0;
} // end if



int BfCompany_createMainWindows_Post ( BfCompany *comp )
{
    if ( comp->hasTablePrivilege ( "pedidocliente", "SELECT" ) ) {
        g_pedidosClienteList = new PedidosClienteList ( comp, NULL );
        comp->m_pWorkspace->addSubWindow ( g_pedidosClienteList );
        g_pedidosClienteList->hide();
    }// end if
    return 0;
}


int ClienteView_ClienteView_Post ( ClienteView *prov )
{
    if ( prov->mainCompany()->hasTablePrivilege ( "pedidocliente", "SELECT" ) ) {
        PedidosClienteList *pedidosClienteList = new PedidosClienteList ( NULL, 0, BL_SELECT_MODE );
	pedidosClienteList->setMainCompany(( BfCompany * ) prov->mainCompany());
        pedidosClienteList->setEditMode();
        pedidosClienteList->setObjectName ( "listpedidoclientees" );
        pedidosClienteList->hideBusqueda();
        prov->mui_tab->addTab ( pedidosClienteList, "Pedidos" );
    }// end if
    return 0;
}

int ClienteView_cargarPost_Post ( ClienteView *prov )
{
    if ( prov->mainCompany()->hasTablePrivilege ( "pedidocliente", "SELECT" ) ) {
        PedidosClienteList *pedidosClienteList = prov->findChild<PedidosClienteList *> ( "listpedidoclientees" );
        pedidosClienteList->setidcliente ( prov->dbValue ( "idcliente" ) );
        pedidosClienteList->presentar();
    }// end if
    return 0;
}// end if


int BfBuscarReferencia_on_mui_abrirtodo_clicked_Post ( BfBuscarReferencia *ref )
{
    QString SQLQuery = "SELECT * FROM pedidocliente WHERE refpedidocliente = '" + ref->mui_referencia->text() + "'";
    BlDbRecordSet *cur = ref->mainCompany() ->loadQuery ( SQLQuery );
    while ( !cur->eof() ) {
        PedidoClienteView * bud = new PedidoClienteView ( ( BfCompany * ) ref->mainCompany(), NULL );
        ref->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
        bud->load ( cur->value( "idpedidocliente" ) );
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
int PresupuestoView_PresupuestoView ( PresupuestoView *l )
{

    BL_FUNC_DEBUG

    GenPedQToolButton *mui_exporta_efactura2 = new GenPedQToolButton ( l, l->mui_plugbotones );

    QHBoxLayout *m_hboxLayout1 = l->mui_plugbotones->findChild<QHBoxLayout *> ( "hboxLayout1" );

    if ( !m_hboxLayout1 ) {
        m_hboxLayout1 = new QHBoxLayout ( l->mui_plugbotones );
        m_hboxLayout1->setSpacing ( 4 );
        m_hboxLayout1->setMargin ( 0 );
        m_hboxLayout1->setObjectName ( QString::fromUtf8 ( "hboxLayout1" ) );
    }// end if
    m_hboxLayout1->addWidget ( mui_exporta_efactura2 );

    

    return 0;
}

/// Esta llamada de plugin es bastante novedosa ya es una llamada que no responde a una funcion
/// Sino que se llama desde multiples partes del sistema.
int SNewPedidoClienteView ( BfCompany *v )
{
    PedidoClienteView *h = new PedidoClienteView ( v, 0 );
    g_plugParams = h;
    return 1;
}

