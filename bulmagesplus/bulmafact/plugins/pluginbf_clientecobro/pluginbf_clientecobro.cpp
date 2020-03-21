/***************************************************************************
 *   Copyright (C) 2010 by Tomeu Borras Riera                              *
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

/* Dedicat a na Mariona Palomo (filla de en Leo i na Laida) nascuda a Barcelona el dissabte 16/01/2010 */


#include <stdio.h>

#include "pluginbf_clientecobro.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "cobroview.h"
#include "cobroslist.h"
#include "impqtoolbutton.h"


CobrosList *g_cobrosList = NULL;
BfBulmaFact *g_pluginbf_clientecobro = NULL;

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
    blBindTextDomain ( "pluginbf_clientecobro", g_confpr->value( CONF_DIR_TRADUCCION ).toLatin1().constData() );
    
    if ( bges->company()->hasTablePrivilege ( "cobro", "SELECT" ) ) {

        /// Miramos si existe un menu Ventas
        QMenu *pPluginMenu = bges->newMenu ( _("&Ventas"), "menuVentas", "menuMaestro" );
        pPluginMenu->addSeparator();

        g_pluginbf_clientecobro = bges;

        BlAction *accionA = new BlAction ( _ ( "&Cobros de clientes" ), 0 );
        accionA->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/receive-list.png" ) ) );
        accionA->setStatusTip ( _ ( "Cobros de clientes" ) );
        accionA->setWhatsThis ( _ ( "Cobros de clientes" ) );
        accionA->setObjectName("mui_actionCobroCliente");

        pPluginMenu->addAction ( accionA );
        bges->Listados->addAction ( accionA );

        BlAction *accionB = new BlAction ( _ ( "&Nuevo cobro de cliente" ), 0 );
        accionB->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/receive.png" ) ) );
        accionB->setStatusTip ( _ ( "Nuevo cobro de cliente" ) );
        accionB->setWhatsThis ( _ ( "Nuevo cobro de cliente" ) );
        accionB->setObjectName("mui_actionCobroClienteNuevo");

        pPluginMenu->addAction ( accionB );
        bges->Fichas->addAction ( accionB );
    } // end if
    
    return 0;
}

int BlAction_actionTriggered(BlAction *accion) {
    if (accion->objectName() == "mui_actionCobroCliente") {
        if ( g_cobrosList ) {
            g_cobrosList->hide();
            g_cobrosList->show();
        }// end if
    } // end if

    if (accion->objectName() == "mui_actionCobroClienteNuevo") {
        CobroView * bud = new CobroView ( g_pluginbf_clientecobro->company(), NULL );
        g_pluginbf_clientecobro->company()->m_pWorkspace->addSubWindow ( bud );
        bud->show();
    } // end if


    return 0;
} // end if


int BfCompany_createMainWindows_Post ( BfCompany *comp )
{
    if ( comp->hasTablePrivilege ( "cobro", "SELECT" ) ) {
        g_cobrosList = new CobrosList ( comp, NULL );
        comp->m_pWorkspace->addSubWindow ( g_cobrosList );
        g_cobrosList->hide();
    }// end if
    return 0;
}


int ClienteView_ClienteView_Post ( ClienteView *prov )
{
    if ( prov->mainCompany()->hasTablePrivilege ( "cobro", "SELECT" ) ) {
        CobrosList *pagosList = new CobrosList ( NULL, 0, BL_SELECT_MODE );
	pagosList->setMainCompany(( BfCompany * ) prov->mainCompany());
        pagosList->setEditMode();
        pagosList->setObjectName ( "listcobrosproveedor" );
        pagosList->hideBusqueda();
        prov->mui_tab->addTab ( pagosList, "Cobros" );
    }// end if
    return 0;
}

int ClienteView_cargarPost_Post ( ClienteView *prov )
{
    if ( prov->mainCompany()->hasTablePrivilege ( "cobro", "SELECT" ) ) {
        CobrosList *pagosList = prov->findChild<CobrosList *> ( "listcobrosproveedor" );
        pagosList->setidcliente ( prov->dbValue ( "idcliente" ) );
        pagosList->presentar();
    }// end if
    return 0;
}// end if


int BfBuscarReferencia_on_mui_abrirtodo_clicked_Post ( BfBuscarReferencia *ref )
{
    QString SQLQuery = "SELECT * FROM cobro WHERE refcobro = '" + ref->mui_referencia->text() + "'";
    BlDbRecordSet *cur = ref->mainCompany() ->loadQuery ( SQLQuery );
    while ( !cur->eof() ) {
        CobroView * bud = new CobroView ( ( BfCompany * ) ref->mainCompany(), NULL );
        ref->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
        bud->load ( cur->value( "idcobro" ) );
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
int FacturaView_FacturaView ( FacturaView *l )
{
    BL_FUNC_DEBUG
    GenCobroQToolButton *mui_exporta_efactura2 = new GenCobroQToolButton ( l, l->mui_plugbotones );

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
int AlbaranClienteView_AlbaranClienteView ( AlbaranClienteView *l )
{
    BL_FUNC_DEBUG
    GenCobroQToolButton *mui_exporta_efactura2 = new GenCobroQToolButton ( l, l->mui_plugbotones );

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
int PedidoClienteView_PedidoClienteView ( PedidoClienteView *l )
{
    BL_FUNC_DEBUG
    GenCobroQToolButton *mui_exporta_efactura2 = new GenCobroQToolButton ( l, l->mui_plugbotones );

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


/// Esta llamada de plugin es bastante novedosa ya es una llamada que no responde a una funcion
/// Sino que se llama desde multiples partes del sistema.
int SNewCobroView ( BfCompany *v )
{
    CobroView *h = new CobroView ( v, 0 );
    g_plugParams = h;
    return 1;
}
