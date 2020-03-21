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

#ifndef FACTURAVIEW_H
#define FACTURAVIEW_H

#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

#include "bldatesearch.h"
#include "bfbuscarformapago.h"
#include "bfbuscarseriefactura.h"
#include "bfbuscaralmacen.h"
#include "bldialogchanges.h"
#include "bfform.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "pdefs_pluginbf_clientefactura.h"
#include "ui_facturabase.h"


/// Muestra y administra la ventana de una factura a cliente.
/** */
class PLUGINBF_CLIENTEFACTURA_EXPORT FacturaView : public BfForm, public Ui_FacturaBase
{
    Q_OBJECT

public:
    FacturaView ( BfCompany *, QWidget *parent = 0 );
    ~FacturaView();
    void inicializar();
    void pintatotales ( BlFixed, BlFixed, BlFixed, BlFixed, BlFixed, BlFixed );
    virtual int cargarPost ( QString );
    virtual int afterSave();
    virtual int beforeDelete();

public slots:
    virtual void on_m_descuentos_editFinish ( int, int );
    virtual void on_subform2_editFinish ( int, int );
    virtual void on_mui_veralbaranes_clicked();
    virtual void on_mui_idcliente_valueChanged ( QString id );
    virtual void on_mui_idalmacen_valueChanged ( QString id );
};


#endif

