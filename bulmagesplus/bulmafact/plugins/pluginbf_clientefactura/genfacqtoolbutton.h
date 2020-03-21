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

#ifndef GENFACQTOOLBUTTON_H
#define GENFACQTOOLBUTTON_H

#include <QWidget>
#include <QToolButton>
#include <QString>
#include <QtXml/QDomDocument>
#include <QList>
#include <QPair>
#include <QString>

#include "bfcompany.h"
#include "blfunctions.h"
#include "pdefs_pluginbf_clientefactura.h"


class PLUGINBF_CLIENTEFACTURA_EXPORT GenFacQToolButton : public QToolButton
{
    Q_OBJECT

private:
    BfCompany *m_companyact;
    QWidget *m_object;

public:
    QList< QPair<QString, QString> > *m_fieldListToTransfer_form;
    QList< QPair<QString, QString> > *m_fieldListToTransfer_subForm;

    GenFacQToolButton ( QWidget *fac, QWidget *parent = NULL );
    ~GenFacQToolButton();
    void setBoton();
    void generarFactura();
    void generarFactura1();
    void generarFactura2();

public slots:
    virtual void click();

};


class PLUGINBF_CLIENTEFACTURA_EXPORT AgFacQToolButton : public QToolButton
{
    Q_OBJECT

private:
    BfCompany *m_companyact;
    QWidget *m_object;

public:
    QList< QPair<QString, QString> > *m_fieldListToTransfer_form;
    QList< QPair<QString, QString> > *m_fieldListToTransfer_subForm;
  
    AgFacQToolButton ( QWidget *fac,   QWidget *parent = NULL );
    ~AgFacQToolButton();
    void setBoton();
    void generarFactura();

public slots:
    virtual void click();

};

#endif

