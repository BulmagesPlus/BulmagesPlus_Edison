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

#ifndef PEDIDOCLIENTEVIEW_H
#define PEDIDOCLIENTEVIEW_H

#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

#include "listlinpedidoclienteview.h"
#include "listdescpedidoclienteview.h"
#include "bldatesearch.h"
#include "bfbuscarformapago.h"
#include "bfbuscaralmacen.h"
#include "bfbuscartrabajador.h"
#include "bldialogchanges.h"
#include "blfixed.h"
#include "bfform.h"
#include "ui_pedidoclientebase.h"
#include "pdefs_pluginbf_clientepedido.h"


/// Muestra y administra la ventana con la informaci&oacute;n de un pedido de cliente.
/** */
class PLUGINBF_CLIENTEPEDIDO_EXPORT PedidoClienteView : public BfForm, public Ui_PedidoClienteBase
{
    Q_OBJECT

public:
    PedidoClienteView ( BfCompany *, QWidget *parent = 0 );
    ~PedidoClienteView();
    virtual int cargarPost ( QString );
    virtual int afterSave();
    virtual int beforeDelete();
    void generarAlbaran();
    void inicializar();
    void pintatotales ( BlFixed iva, BlFixed base, BlFixed total, BlFixed desc, BlFixed, BlFixed );

public slots:
    virtual void s_pintaTotales();
    virtual void on_mui_verpresupuesto_clicked();
    virtual void on_mui_pasaraalbaran_clicked();
    virtual void on_m_descuentos_editFinish ( int, int );
    virtual void on_subform3_editFinish ( int, int );
    virtual void on_mui_idcliente_valueChanged ( QString id );
    virtual void on_mui_idalmacen_valueChanged ( QString id );
};

#endif

