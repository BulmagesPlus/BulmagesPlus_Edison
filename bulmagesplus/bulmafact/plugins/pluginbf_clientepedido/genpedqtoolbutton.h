/***************************************************************************
 *   Copyright (C) 2007 by Tomeu Borras Riera                              *
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
 
#include <QtXml/QDomDocument>
#include <QMap>
#include <QString>
#include <QPair>
#include <QWidget>
#include <QToolButton>
#include <QString>

#include "bfcompany.h"
#include "blfunctions.h"
#include "pdefs_pluginbf_clientepedido.h"


class PLUGINBF_CLIENTEPEDIDO_EXPORT GenPedQToolButton : public QToolButton
{
    Q_OBJECT

private:
    BfCompany *m_companyact;
    QWidget *m_object;

public:
    QList< QPair<QString, QString> > *m_fieldListToTransfer_form;
    QList< QPair<QString, QString> > *m_fieldListToTransfer_subForm;

    GenPedQToolButton ( QWidget *fac, QWidget *parent = NULL );
    ~GenPedQToolButton();
    void setBoton();
    void generarFactura1();

public slots:
    virtual void click();

};


