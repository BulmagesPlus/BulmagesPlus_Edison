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

#ifndef BFBUSCARTIPOIVA_H
#define BFBUSCARTIPOIVA_H

#include <QLineEdit>
#include <QLabel>

#include "blcombobox.h"
#include "blpostgresqlclient.h"
#include "blwidget.h"


class BfCompany;


/// Permite buscar y seleccionar un tipo de IVA.
/** Este 'QComboBox' permite buscar y seleccionar un
    tipo de IVA. Aparece en forma de desplegable.
    Antes de utilizarlo debe ser inicializado con setMainCompany().
    Cuando se cambia el valor del Widget se emite un SIGNAL valueChanged().
*/
class BF_EXPORT BfBuscarTipoIVA : public BlComboBox
{
    Q_OBJECT

public:
    explicit BfBuscarTipoIVA(QWidget *parent = 0);
    ~BfBuscarTipoIVA();
    virtual void setId(QString idtipo_iva);
    QString id() const;
//    void m_activated(int index);
};


/** Convierte en los SubForms los datos del tipo desctipo_iva en selectores del tipo 'QComboBox'
    de esta forma la introduccion de tipos de IVA es sencilla.
    Esta clase se usa conjuntamente con BfSubForm para el cambio del editor
    estandar por un 'QComboBox' */
class BF_EXPORT BfBuscarTipoIVADelegate : public BlComboBox
{
    Q_OBJECT

public:
    explicit BfBuscarTipoIVADelegate(QWidget *parent = 0);
    ~BfBuscarTipoIVADelegate();
    virtual void set(const QString &);
};

#endif
