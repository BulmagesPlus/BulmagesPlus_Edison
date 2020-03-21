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

#ifndef BFBUSCARALMACEN_H
#define BFBUSCARALMACEN_H

#include "blcombobox.h"
#include "blwidget.h"
#include "bfcompany.h"
#include "blpostgresqlclient.h"


/** Esta clase se usa conjuntamente con BfSubForm para el cambio del editor
    estandar por un ComboBox */
class BF_EXPORT BfBuscarAlmacenDelegate : public BlComboBox
{
    Q_OBJECT

private:
    BlDbRecordSet *m_cursorcombo;

public:
    explicit BfBuscarAlmacenDelegate(QWidget *parent = 0);
    ~BfBuscarAlmacenDelegate();
    virtual void set(const QString &);
};


/// Clase que sirve para seleccionar un almac&eacute;n.
/** Creamos un QComboBox que sirve para presentar la lista de almacenes
    disponibles para poder seleccionar uno de ellos.*/
class BF_EXPORT BfBuscarAlmacen : public BlComboBox
{
    Q_OBJECT

private:
    /// Indica cual es el codigo de almacen por defecto.
    QString m_codigoalmacen;

public:
    explicit BfBuscarAlmacen(QWidget *parent = 0, const char *name = 0);
    ~BfBuscarAlmacen();
    virtual void setId(QString idalmacen);
    QString id() const;
    void setMainCompany(BfCompany *company);

public slots:
    void m_activated(int index);
};

#endif
