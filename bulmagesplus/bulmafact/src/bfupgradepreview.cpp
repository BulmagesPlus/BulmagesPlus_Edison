/***************************************************************************
 *   Copyright (C) 2012 by Daniel Ripoll Osma.                             *
 *   info@danielripoll.es                                                  *
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


#include <QFileInfo>
#include <QString>
#include "blplugins.h"
#include "bfbulmafact.h"
#include "bfupgradepreview.h"


BfUpgradePreview::BfUpgradePreview(BlMainWindow *parent)
{
    m_parent = parent;
    uP = new BlUpgradePreview;
      
    BfBulmaFact *bf = ((BfBulmaFact *) m_parent);

    /// Creamos un nuevo menu Ventana si no esta creado
    QMenu *pWindowMenu = bf->newMenu ( _("&Ventana"), "menuVentana", "menuMaestro" );

    /// Creamos una nueva accion del menuVentana si no esta creada y la hacemos checkeable
    QAction *accionA = bf->newMenuQAction( _("Modo &Extendido"), "mui_actionExtendido", pWindowMenu, _("Modo Extendido") , false );
    accionA->setCheckable(true);

    /// Cargamos la accion dentro del menu
    bf->loadAction(accionA, pWindowMenu);

    /// Checkeamos si la preview esta cargada o no
    if (g_confpr->value(CONF_PREVIEW) == "TRUE")
    {
        accionA->setChecked(true);
    }
    else
    {
        accionA->setChecked(false);
    } // end if

    /// Conectamos nuestro checkbox-triggered para desactivar o activar el modo extendido
    connect(accionA, SIGNAL(triggered(bool)), this, SLOT(extendido_triggered(bool)));
    
}


BfUpgradePreview::~BfUpgradePreview()
{
}


int BfUpgradePreview::packProveedorTriggered(bool triggered)
{
    BL_FUNC_DEBUG

    /// Salta nuestra ventana de adquisicion del pack Proveedores
    uP->showUpgradePreviewInfo("?name=pluginbf_pack_proveedores");

    return 0;
}

int BfUpgradePreview::packTriggered(bool triggered)
{
    BL_FUNC_DEBUG

    /// Salta nuestra ventana de adquisicion del pack por defecto
    uP->showUpgradePreviewInfo("?name=pluginbf_otros");

    return 0;
}

void BfUpgradePreview::loadActions(){
    BL_FUNC_DEBUG

    /// Checkeamos nuevamente si esta la preview activada
    if (g_confpr->value(CONF_PREVIEW) == "TRUE")
    {
        loadMenuArticulos();
        loadMenuCompras();
    }
}


int BfUpgradePreview::loadMenuCompras( )
{
    BL_FUNC_DEBUG
  
    BfBulmaFact *bf = ((BfBulmaFact *) m_parent);

    /// Creamos el menu compras si no esta creado
    QMenu *pPluginMenu = bf->newMenu ( _("&Compras"), "menuCompras", "menuMaestro" );

    /// Checkeamos si esta accion-demo existe como plugin
    if (!uP->unloadPreview("libpluginbf_proveedor"))
    {
        /// Creamos las acciones y separador correspondientes y las metemos en nuestro menu y barra correspondientes
        pPluginMenu->addSeparator();
        QAction *accionA = bf->newMenuQAction ( _("&Proveedores"), "mui_actionUpgradeProveedores", pPluginMenu, _("Proveedores") , false, ":/Images/supplier-list.png" );
        QAction *accionB = bf->newMenuQAction ( _("&Nuevo proveedor"), "mui_actionUpgradeProveedorNuevo", pPluginMenu,  _("Nuevo proveedor"), false , ":/Images/supplier.png" );

        m_listActions << accionA << accionB;

        connect (accionA, SIGNAL(triggered(bool)), this, SLOT(packProveedorTriggered(bool)));
        connect (accionB, SIGNAL(triggered(bool)), this, SLOT(packProveedorTriggered(bool)));
        
        bf->loadAction(accionA, pPluginMenu);
        bf->loadAction(accionA, bf->Fichas);
        bf->loadAction(accionB, pPluginMenu);
        bf->loadAction(accionB, bf->Fichas);
    }

    /// Checkeamos si esta accion-demo existe como plugin
    if (!uP->unloadPreview("libpluginbf_proveedoralbaran"))
    {
        /// Creamos las acciones y separador correspondientes y las metemos en nuestro menu y barra correspondientes
        pPluginMenu->addSeparator();
        QAction *accionC = bf->newMenuQAction ( _("&Albaranes de proveedores"), "mui_actionUpgradeProveedoresAlbaran", pPluginMenu,  _("Albaranes de proveedor"), false,  ":/Images/supplier-delivery-note-list.png" );
        QAction *accionD = bf->newMenuQAction ( _("&Nuevo albaran de proveedor"), "mui_actionUpgradeProveedAlbaranNuevo", pPluginMenu,  _("Nuevo albaran de proveedor"), false , ":/Images/supplier-delivery-note.png" );

        m_listActions << accionC << accionD;

        connect (accionC, SIGNAL(triggered(bool)), this, SLOT(packProveedorTriggered(bool)));
        connect (accionD, SIGNAL(triggered(bool)), this, SLOT(packProveedorTriggered(bool)));

        bf->loadAction(accionC, pPluginMenu);
        bf->loadAction(accionC, bf->Fichas);
        bf->loadAction(accionD, pPluginMenu);
        bf->loadAction(accionD, bf->Fichas);
    }

    /// Checkeamos si esta accion-demo existe como plugin
    if (!uP->unloadPreview("libpluginbf_proveedorfacturas"))
    {
        /// Creamos las acciones y separador correspondientes y las metemos en nuestro menu y barra correspondientes
        pPluginMenu->addSeparator();
        QAction *accionE = bf->newMenuQAction ( _("&Facturas de proveedores"), "mui_actionUpgradeProveedoresFacturas", pPluginMenu,  _("Facturas de proveedores") , false, ":/Images/supplier-invoice-list.png" );
        QAction *accionF = bf->newMenuQAction ( _("&Nueva factura de proveedor"), "mui_actionUpgradeProveedorFacturaNueva", pPluginMenu,  _("Nueva factura de proveedor"), false , ":/Images/supplier-invoice.png" );

        m_listActions << accionE << accionF;

        connect (accionE, SIGNAL(triggered(bool)), this, SLOT(packProveedorTriggered(bool)));
        connect (accionF, SIGNAL(triggered(bool)), this, SLOT(packProveedorTriggered(bool)));

        bf->loadAction(accionE, pPluginMenu);
        bf->loadAction(accionE, bf->Fichas);
        bf->loadAction(accionF, pPluginMenu);
        bf->loadAction(accionF, bf->Fichas);
    }

    /// Checkeamos si esta accion-demo existe como plugin
    if (!uP->unloadPreview("libpluginbf_proveedorpedido"))
    {
        /// Creamos las acciones y separador correspondientes y las metemos en nuestro menu y barra correspondientes
        pPluginMenu->addSeparator();
        QAction *accionG = bf->newMenuQAction ( _("&Pedidos a proveedores"), "mui_actionUpgradeProveedoresPedidos", pPluginMenu,  _("Pedidos a proveedores") , false, ":/Images/supplier-purchase-order-list.png" );
        QAction *accionH = bf->newMenuQAction ( _("&Nuevo pedido a proveedor"), "mui_actionUpgradeProveedorPedidoNuevo", pPluginMenu,  _("Nuevo pedido a proveedor"), false , ":/Images/supplier-purchase-order.png" );

        m_listActions << accionG << accionH;

        connect (accionG, SIGNAL(triggered(bool)), this, SLOT(packProveedorTriggered(bool)));
        connect (accionH, SIGNAL(triggered(bool)), this, SLOT(packProveedorTriggered(bool)));
        
        bf->loadAction(accionG, pPluginMenu);
        bf->loadAction(accionG, bf->Fichas);
        bf->loadAction(accionH, pPluginMenu);
        bf->loadAction(accionH, bf->Fichas);
    }

    /// Checkeamos si esta accion-demo existe como plugin
    if (!uP->unloadPreview("libpluginbf_proveedorpago"))
    {
        /// Creamos las acciones y separador correspondientes y las metemos en nuestro menu y barra correspondientes
        pPluginMenu->addSeparator();
        QAction *accionI = bf->newMenuQAction ( _("&Pagos a proveedores"), "mui_actionUpgradeProveedorPagoNuevo", pPluginMenu,  _("Pagos a proveedores") , false, ":/Images/pay-list.png" );
        QAction *accionJ = bf->newMenuQAction ( _("&Nuevo pago a proveedor"), "mui_actionUpgradeProveedoresPagos", pPluginMenu,  _("Nuevo pago a proveedor"), false , ":/Images/pay.png" );

        m_listActions << accionI << accionJ;

        connect (accionI, SIGNAL(triggered(bool)), this, SLOT(packProveedorTriggered(bool)));
        connect (accionJ, SIGNAL(triggered(bool)), this, SLOT(packProveedorTriggered(bool)));
        
        bf->loadAction(accionI, pPluginMenu);
        bf->loadAction(accionI, bf->Fichas);
        bf->loadAction(accionJ, pPluginMenu);
        bf->loadAction(accionJ, bf->Fichas);
    }

    /// Checkeamos si esta accion-demo existe como plugin
    if (!uP->unloadPreview("libpluginbf_carterapagos"))
    {
        /// Creamos las acciones y separador correspondientes y las metemos en nuestro menu y barra correspondientes
        pPluginMenu->addSeparator();
        QAction *accionK = bf->newMenuQAction ( _("&Cartera de Pagos"), "mui_actionUpgradeCarteraPagos", pPluginMenu,  _("Cartera de Pagos"), false , ":/Images/pay-list.png" );

        m_listActions << accionK;

        connect (accionK, SIGNAL(triggered(bool)), this, SLOT(packTriggered(bool)));
        
        bf->loadAction(accionK, pPluginMenu);
        bf->loadAction(accionK, bf->Listados);
    }

    return 0;
}


int BfUpgradePreview::loadMenuArticulos()
{

    BL_FUNC_DEBUG

    BfBulmaFact *bf = ((BfBulmaFact *) m_parent);

    /// Creamos el menu articulos si no esta creado
    QMenu *pPluginMenu = bf->newMenu ( _("&Articulos"), "menuArticulos", "menuMaestro" );

    /// Checkeamos si esta accion-demo existe como plugin
    if (!uP->unloadPreview("libpluginbf_inventario"))
    {
        /// Creamos las acciones y separador correspondientes y las metemos en nuestro menu y barra correspondientes
        pPluginMenu->addSeparator();

        QAction *accionI = bf->newMenuQAction ( _("&Inventarios"), "mui_actionUpgradeInventario", pPluginMenu,  _("Inventarios"), false , ":/Images/product-list.png" );

        m_listActions << accionI;
        
        connect (accionI, SIGNAL(triggered(bool)), this, SLOT(packTriggered(bool)));

        bf->loadAction(accionI, pPluginMenu);
    }

    return 0;
}

void BfUpgradePreview::unloadActions()
{
    BL_FUNC_DEBUG
    BfBulmaFact *bf = ((BfBulmaFact *) m_parent);

    QMenu *actionParent = NULL;

    /// Recorremos la lista de acciones para destruirlas
    foreach (QAction *actionItem, m_listActions)
    {
        /// Hacemos un casting de QWidget* a QMenu*, para comprobar si esta vacio luego
        actionParent = (QMenu*) actionItem->parentWidget();

        /// Elimimanos la accion del menu
        actionParent->removeAction(actionItem);

        /// Checkeamos si esta vacio el menu
        if (actionParent->isEmpty())
        {
            /// Si lo esta, destruye el menu
            actionParent->deleteLater();
        }
        bf->Listados->removeAction(actionItem);
        bf->Fichas->removeAction(actionItem);
    }
    
    /// Limpiamos la lista
    m_listActions.clear();
}


void BfUpgradePreview::extendido_triggered(bool triggered)
{
    /// Checkeamos si han activado o desactivado el checkbox de extendido
    if (triggered)
    {
        /// Carga los menus
        g_confpr->setValue( CONF_PREVIEW, "TRUE" );
        loadActions();
    }
    else
    {
        /// Descarga los menus
        g_confpr->setValue( CONF_PREVIEW, "FALSE" );
        unloadActions();
        //uP->unloadActions();
    }
}
