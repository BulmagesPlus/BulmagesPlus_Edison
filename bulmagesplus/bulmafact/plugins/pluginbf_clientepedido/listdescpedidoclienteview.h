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

#ifndef LISTDESCPEDIDOCLIENTEVIEW_H
#define LISTDESCPEDIDOCLIENTEVIEW_H

#include "bfsubform.h"
#include "bfcompany.h"
#include "pdefs_pluginbf_clientepedido.h"


/// Muestra y administra la ventana de lista de descuentos por pedido de cliente.
/** */
class PLUGINBF_CLIENTEPEDIDO_EXPORT ListDescuentoPedidoClienteView : public BfSubForm
{
    Q_OBJECT

public:
    QString mdb_idpedidocliente;
    ListDescuentoPedidoClienteView ( QWidget *parent = 0 );
    ~ListDescuentoPedidoClienteView() {};

public slots:
    virtual void load ( QString idpedidocliente );
};

#endif

