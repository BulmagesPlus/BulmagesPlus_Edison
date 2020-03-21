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

#ifndef FACTURASLIST_H
#define FACTURASLIST_H

#include "bfbuscararticulo.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "bfsubform.h"
#include "blformlist.h"
#include "pdefs_pluginbf_clientefactura.h"


/// Administra el detalle del listado de facturas a clientes.
/** */
class PLUGINBF_CLIENTEFACTURA_EXPORT FacturasListSubform : public BfSubForm
{
    Q_OBJECT

public:
    FacturasListSubform ( QWidget *parent = 0, const char *name = 0 );
    ~FacturasListSubform();

public slots:
    virtual void load();
    virtual void load ( QString query );
};


#include "ui_facturaslistbase.h"


/// Administra el listado de facturas a clientes.
/** */
class PLUGINBF_CLIENTEFACTURA_EXPORT FacturasList : public BlFormList, public Ui_FacturasListBase
{
    Q_OBJECT

private:
    QString mdb_idfactura;

public:
    FacturasList ( QWidget *parent = 0, Qt::WindowFlags flag = 0, edmode editmodo = BL_EDIT_MODE );
    FacturasList ( BfCompany *, QWidget *parent = 0, Qt::WindowFlags flag = 0, edmode editmodo = BL_EDIT_MODE );
    ~FacturasList();
    void iniciaForm();
    void setMainCompany ( BfCompany *comp );
    QString idfactura();
    void setidcliente ( QString val );
    void setidarticulo ( QString val );
    const QString generaFiltro();
    void presentar();
    void editar ( int );
    void remove();
    void crear();
    void imprimir();

signals:
    void selected ( QString );
};

#endif

