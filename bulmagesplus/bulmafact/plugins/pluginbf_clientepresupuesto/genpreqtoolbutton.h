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

#ifndef GENPREQTOOLBUTTON_H
#define GENPREQTOOLBUTTON_H

#include <QtXml/QDomDocument>
#include <QMap>
#include <QString>
#include <QWidget>
#include <QToolButton>
#include <QString>

#include "bfcompany.h"
#include "blfunctions.h"
#include "pdefs_pluginbf_clientepresupuesto.h"

/*
class GenPreQToolButton : public QToolButton
{
    Q_OBJECT

private:
    BfCompany *m_companyact;
    QWidget *m_object;

public:
    GenPreQToolButton ( QWidget *fac,   QWidget *parent = NULL );
    ~GenPreQToolButton();
    void setBoton();
    void generarFactura();
    void generarFactura1();
    void generarFactura2();

public slots:
    virtual void click();

};
*/

class PLUGINBF_CLIENTEPRESUPUESTO_EXPORT VerPreQToolButton : public QToolButton
{
    Q_OBJECT

private:
    BfCompany *m_companyact;
    QWidget *m_object;

public:
    VerPreQToolButton ( QWidget *fac,   QWidget *parent = NULL );
    ~VerPreQToolButton();
    void setBoton();
    void verPresupuesto();

public slots:
    virtual void click();

};

#endif

