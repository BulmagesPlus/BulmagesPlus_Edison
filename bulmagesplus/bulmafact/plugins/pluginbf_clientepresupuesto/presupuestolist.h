/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borras Riera                              *
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

#ifndef BUDGETSLIST_H
#define BUDGETSLIST_H

#include <QLineEdit>

#include "bfcompany.h"
#include "bfbuscararticulo.h"
#include "bfsubform.h"
#include "blformlist.h"
#include "pdefs_pluginbf_clientepresupuesto.h"


/// Administra el listado de presupuestos.
/** */
class PLUGINBF_CLIENTEPRESUPUESTO_EXPORT PresupuestoListSubForm : public BfSubForm
{
    Q_OBJECT

public:
    PresupuestoListSubForm ( QWidget *parent = 0, const char *name = 0 );
    ~PresupuestoListSubForm() {};

public slots:
    virtual void load();
    virtual void load ( QString query );
};


#include "ui_presupuestoslistbase.h"


/// Muestra y administra la ventana con el listado de presupuestos.
/** */
class PLUGINBF_CLIENTEPRESUPUESTO_EXPORT PresupuestoList : public BlFormList, public Ui_PresupuestosListBase
{
    Q_OBJECT

private:
    QString m_idpresupuesto;

public:
    PresupuestoList ( QWidget *parent = 0, Qt::WindowFlags flag = 0, edmode editmodo = BL_EDIT_MODE );
    PresupuestoList ( BfCompany *comp = NULL, QWidget *parent = 0, Qt::WindowFlags flag = 0, edmode editmodo = BL_EDIT_MODE );
    ~PresupuestoList();
    void presentar();
    void editar ( int );
    void remove();
    void crear();
    QString idpresupuesto();
    void setMainCompany ( BfCompany *comp );
    void imprimir();
    void setidcliente ( QString val );
    void setidarticulo ( QString val );
    const QString generaFiltro();
    void iniciaForm();


signals:
    void selected ( QString );
};

#endif

