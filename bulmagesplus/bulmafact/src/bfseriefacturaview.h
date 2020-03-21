/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
 *   Copyright (C) 2006 by Fco. Javier M. C. (Porting to QT4)              *
 *   fcojavmc@todo-redes.com                                               *
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

#ifndef BFSERIEFACTURAVIEW_H
#define BFSERIEFACTURAVIEW_H

#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

#include "blpostgresqlclient.h"
#include "bldatesearch.h"
#include "bfsubform.h"
#include "bfform.h"


class BfCompany;


/// Administra el detalle del listado de series de factura.
/** */
class BF_EXPORT BfSerieFacturaSubForm : public BfSubForm
{
    Q_OBJECT

public:
    explicit BfSerieFacturaSubForm(QWidget *parent = 0);
    ~BfSerieFacturaSubForm() {}

public slots:
    virtual void load() {
        BL_FUNC_DEBUG
        BlSubForm::load ( "SELECT *, codigoserie_factura AS codigoserie_facturaorig FROM serie_factura" );
    }
};


#include "ui_bfseriefacturabase.h"


/// Muestra y administra la ventana con la lista de series de facturas.
/** */
class BF_EXPORT BfSerieFacturaView : public BfForm, public Ui_BfSerieFacturaBase
{
    Q_OBJECT

public:
    BfSerieFacturaView ( BfCompany *, QWidget * );
    virtual ~BfSerieFacturaView();

public slots:
    virtual void on_mui_aceptar_clicked() {
        mui_listado->save();
        close();
    }

};

#endif

