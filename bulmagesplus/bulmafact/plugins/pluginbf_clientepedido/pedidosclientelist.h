/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
 *   http://www.iglues.org                                                 *
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

#ifndef PEDIDOSCLIENTELIST_H
#define PEDIDOSCLIENTELIST_H

#include "bfbuscararticulo.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "bfsubform.h"
#include "blformlist.h"
#include "pdefs_pluginbf_clientepedido.h"


/// Administra las l&iacute;neas de detalle del listado de pedidos de cliente.
/** */
class PLUGINBF_CLIENTEPEDIDO_EXPORT PedidosClienteListSubform : public BfSubForm
{
    Q_OBJECT

public:
    PedidosClienteListSubform ( QWidget *parent = 0, const char *name = 0 );
    ~PedidosClienteListSubform() {};

public slots:
    virtual void load();
    virtual void load ( QString query );
};


#include "ui_pedidosclientelistbase.h"


/// Muestra y administra el listado de pedidos de cliente.
/** */
class PLUGINBF_CLIENTEPEDIDO_EXPORT PedidosClienteList : public BlFormList, public Ui_PedidosClienteListBase
{
    Q_OBJECT

private:
    QString m_idpedidocliente;

public:
    PedidosClienteList ( QWidget *parent = 0, Qt::WindowFlags flag = 0, edmode editmodo = BL_EDIT_MODE );
    PedidosClienteList ( BfCompany *, QWidget *parent = 0, Qt::WindowFlags flag = 0, edmode editmodo = BL_EDIT_MODE );
    ~PedidosClienteList();
    void presentar();
    void imprimir();
    void editar ( int );
    void remove();
    void crear();
    void setMainCompany ( BfCompany *comp );
    void setidcliente ( QString val );
    QString idpedidocliente();
    QString generarFiltro();
    void iniciaForm();

};

#endif

