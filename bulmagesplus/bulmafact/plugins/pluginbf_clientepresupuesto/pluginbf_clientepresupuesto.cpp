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

#include "pluginbf_clientepresupuesto.h"
#include "blplugins.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "presupuestoview.h"
#include "presupuestolist.h"

#include "genpreqtoolbutton.h"


PresupuestoList *g_presupuestosList = NULL;
BfBulmaFact *g_pluginbf_clientepresupuesto = NULL;


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
    blBindTextDomain ( "pluginbf_clientepresupuesto", g_confpr->value( CONF_DIR_TRADUCCION ).toLatin1().constData() );

    if ( bges->company()->hasTablePrivilege ( "presupuesto", "SELECT" ) ) {

        /// Miramos si existe un menu Ventas
        QMenu *pPluginMenu = bges->newMenu ( _("&Ventas"), "menuVentas", "menuMaestro" );
        pPluginMenu->addSeparator();

        /// El men&uacute; de Tarifas en la secci&oacute;n de art&iacute;culos.
        g_pluginbf_clientepresupuesto = bges;
        BlAction *accionA = new BlAction ( _ ( "&Presupuestos a clientes" ), 0 );
        accionA->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/client-quote-list.png" ) ) );
        accionA->setStatusTip ( _ ( "Presupuestos a clientes" ) );
        accionA->setWhatsThis ( _ ( "Presupuestos a clientes" ) );
        accionA->setObjectName("mui_actionClientePresupuesto");
        pPluginMenu->addAction ( accionA );
        bges->Listados->addAction ( accionA );

        BlAction *accionB = new BlAction ( _ ( "&Nuevo presupuesto a cliente" ), 0 );
        accionB->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/client-quote.png" ) ) );
        accionB->setStatusTip ( _ ( "Nuevo presupuesto a cliente" ) );
        accionB->setWhatsThis ( _ ( "Nuevo presupuesto a cliente" ) );
        accionB->setObjectName("mui_actionClientePresupuestoNuevo");
        pPluginMenu->addAction ( accionB );
        bges->Fichas->addAction ( accionB );
    } // end if

    return 0;
}


int BlAction_actionTriggered(BlAction *accion) {
    if (accion->objectName() == "mui_actionClientePresupuesto") {
        if ( g_presupuestosList ) {
            g_presupuestosList->hide();
            g_presupuestosList->show();
        } // end if
    } // end if
    if (accion->objectName() == "mui_actionClientePresupuestoNuevo") {
        PresupuestoView * bud = new PresupuestoView ( g_pluginbf_clientepresupuesto->company(), NULL );
        g_pluginbf_clientepresupuesto->company()->m_pWorkspace->addSubWindow ( bud );
        bud->inicializar();
        bud->show();
    } // end if
    return 0;
}


int BfCompany_createMainWindows_Post ( BfCompany *comp )
{
    if ( comp->hasTablePrivilege ( "presupuesto", "SELECT" ) ) {
        g_presupuestosList = new PresupuestoList ( comp, NULL );
        comp->m_pWorkspace->addSubWindow ( g_presupuestosList );
        g_presupuestosList->hide();
    } // end if
    return 0;
}


int ClienteView_ClienteView_Post ( ClienteView *prov )
{
    if ( prov->mainCompany()->hasTablePrivilege ( "presupuesto", "SELECT" ) ) {
        PresupuestoList *presupuestosList = new PresupuestoList ( NULL, 0, BL_SELECT_MODE );
	presupuestosList->setMainCompany(( BfCompany * ) prov->mainCompany());
        presupuestosList->setEditMode();
        presupuestosList->setObjectName ( "listpresupuestos" );
        presupuestosList->hideBusqueda();
        prov->mui_tab->addTab ( presupuestosList, "Presupuestos" );
    } // end if
    return 0;
}


int ClienteView_cargarPost_Post ( ClienteView *prov )
{
    if ( prov->mainCompany()->hasTablePrivilege ( "presupuesto", "SELECT" ) ) {
        PresupuestoList *presupuestosList = prov->findChild<PresupuestoList *> ( "listpresupuestos" );
        presupuestosList->setidcliente ( prov->dbValue ( "idcliente" ) );
        presupuestosList->presentar();
    } // end if
    return 0;
}


int BfBuscarReferencia_on_mui_abrirtodo_clicked_Post ( BfBuscarReferencia *ref )
{
    QString SQLQuery = "SELECT * FROM presupuesto WHERE refpresupuesto = '" + ref->mui_referencia->text() + "'";
    BlDbRecordSet *cur = ref->mainCompany() ->loadQuery ( SQLQuery );
    while ( !cur->eof() ) {
        PresupuestoView * bud = new PresupuestoView ( ( BfCompany * ) ref->mainCompany(), NULL );
        ref->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
        bud->load ( cur->value( "idpresupuesto" ) );
        bud->show();
        cur->nextRecord();
    } // end while
    delete cur;
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

    VerPreQToolButton *mui_exporta_epresupuesto2 = new VerPreQToolButton ( l, l->mui_plugbotones );

    QHBoxLayout *m_hboxLayout1 = l->mui_plugbotones->findChild<QHBoxLayout *> ( "hboxLayout1" );

    if ( !m_hboxLayout1 ) {
        m_hboxLayout1 = new QHBoxLayout ( l->mui_plugbotones );
        m_hboxLayout1->setSpacing ( 4 );
        m_hboxLayout1->setMargin ( 0 );
        m_hboxLayout1->setObjectName ( QString::fromUtf8 ( "hboxLayout1" ) );
    } // end if
    m_hboxLayout1->addWidget ( mui_exporta_epresupuesto2 );

    return 0;
}


/// Esta llamada de plugin es bastante novedosa ya es una llamada que no responde a una funcion
/// Sino que se llama desde multiples partes del sistema.
int SNewPresupuestoView ( BfCompany *v )
{
    PresupuestoView *h = new PresupuestoView ( v, 0 );
    g_plugParams = h;
    return 1;
}

