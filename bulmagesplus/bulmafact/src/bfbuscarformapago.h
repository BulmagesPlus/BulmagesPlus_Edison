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

#ifndef BFBUSCARFORMAPAGO_H
#define BFBUSCARFORMAPAGO_H

#include "blfunctions.h"
#include "blcombobox.h"
#include "bfcompany.h"
#include "blpostgresqlclient.h"
#include "blfunctions.h"
#include "blwidget.h"


/// Permite buscar y seleccionar una forma de pago.
/** Widget que permite buscar y seleccionar una
    forma de pago. Aparece en forma de comboBox. */
class BF_EXPORT BfBuscarFormaPago : public BlComboBox
{
    Q_OBJECT

public:
    explicit BfBuscarFormaPago(QWidget *parent = 0);
    ~BfBuscarFormaPago();
    QString id() const;
    virtual void setId(QString idforma_pago) override;
    virtual void setIdCliente(QString);
    virtual void setIdProveedor(QString);
//    virtual void m_activated(int index);
};

#endif
