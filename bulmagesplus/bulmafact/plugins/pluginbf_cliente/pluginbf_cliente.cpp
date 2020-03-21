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

#include <QDebug>

#include "pluginbf_cliente.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "clienteview.h"
#include "clientslist.h"



ClientsList *g_clientesList = NULL;
BfBulmaFact *g_pluginbf_cliente = NULL;




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
    blBindTextDomain ( "pluginbf_cliente", g_confpr->value( CONF_DIR_TRADUCCION ).toLatin1().constData() );


    if ( bges->company()->hasTablePrivilege ( "cliente", "SELECT" ) ) {

        /// Miramos si existe un menu Ventas
        QMenu *pPluginMenu = bges->newMenu ( _("&Ventas"), "menuVentas", "menuMaestro" );
        pPluginMenu->addSeparator();

        g_pluginbf_cliente = bges;
        
        BlAction *accionA = new BlAction ( _ ( "&Clientes" ), 0 );
        accionA->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/client-list.png" ) ) );
        accionA->setStatusTip ( _ ( "Clientes" ) );
        accionA->setWhatsThis ( _ ( "Clientes" ) );
        accionA->setObjectName("mui_actionClientes");

        pPluginMenu->addAction ( accionA );
        bges->Listados->addAction ( accionA );

        BlAction *accionB = new BlAction ( _ ( "&Nuevo cliente" ), 0 );
        accionB->setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/client.png" ) ) );
        accionB->setStatusTip ( _ ( "Nuevo cliente" ) );
        accionB->setWhatsThis ( _ ( "Nuevo cliente" ) );
        accionB->setObjectName("mui_actionNuevoCliente");

        pPluginMenu->addAction ( accionB );
        bges->Fichas->addAction ( accionB );

    } // end if


    return 0;
}

int BlAction_actionTriggered(BlAction *accion)
{
    BL_FUNC_DEBUG

    if (accion->objectName() == "mui_actionClientes") {
        if ( g_clientesList ) {
            g_clientesList->hide();
            g_clientesList->show();
        }// end if
    } // end if
    
    if (accion->objectName() == "mui_actionNuevoCliente") {
        ClienteView * bud = new ClienteView ( g_pluginbf_cliente->company(), NULL );
        g_pluginbf_cliente->company()->m_pWorkspace->addSubWindow ( bud );
        bud->show();
        bud->setWindowTitle ( _ ( "Nuevo cliente" ) );
    } // end if
    
    return 0;
} // end if


int BfCompany_createMainWindows_Post ( BfCompany *comp )
{
    BL_FUNC_DEBUG

    if ( comp->hasTablePrivilege ( "cliente", "SELECT" ) ) {
        g_clientesList = new ClientsList ( comp, NULL );
        comp->m_pWorkspace->addSubWindow ( g_clientesList );
        g_clientesList->hide();
    }// end if
    return 0;
}


int Busqueda_on_mui_buscar_clicked ( BlSearchWidget *busq )
{
    BL_FUNC_DEBUG

    if ( busq->tableName() == "cliente" ) {

        QDialog *diag = new QDialog ( 0 );
        diag->setModal ( true );
        diag->setGeometry ( QRect ( 0, 0, 750, 550 ) );
        blCenterOnScreen ( diag );

        ClientsList *clients = new ClientsList ( ( BfCompany * ) busq->mainCompany(), diag, 0, BL_SELECT_MODE );
        busq->connect ( clients, SIGNAL ( selected ( QString ) ), diag, SLOT ( accept() ) );

        /// Creamos un layout donde estara el contenido de la ventana y la ajustamos al QDialog
        /// para que sea redimensionable y aparezca el titulo de la ventana.
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget ( clients );
        layout->setMargin ( 0 );
        layout->setSpacing ( 0 );
        diag->setLayout ( layout );
        diag->setWindowTitle ( clients->windowTitle() );

        diag->exec();

        if ( clients->idclient() != "" ) {
            busq->setId ( clients->idclient() );
        } // end if

        delete diag;
        return 1;
    } // end if

    return 0;
}


/// Esta llamada de plugin es bastante novedosa ya es una llamada que no responde a una funcion
/// Sino que se llama desde multiples partes del sistema.
int SNewClienteView ( BfCompany *v )
{
    BL_FUNC_DEBUG

    ClienteView *h = new ClienteView ( v, 0 );
    g_plugParams = h;
    return 1;
}
