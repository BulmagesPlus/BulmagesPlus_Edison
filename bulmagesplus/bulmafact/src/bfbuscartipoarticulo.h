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

#ifndef BFBUSCARTIPOARTICULO_H
#define BFBUSCARTIPOARTICULO_H

#include <QLineEdit>
#include <QLabel>

#include "blwidget.h"
#include "blsearchwidget.h"


class BfCompany;

/// Permite buscar y seleccionar un tipo de art&iacute;culo.
/** Este Widget permite buscar y seleccionar un
    tipo de art&iacute;culo. */
class BF_EXPORT BfBuscarTipoArticulo : public BlSearchWidget
{
    Q_OBJECT

public:
    explicit BfBuscarTipoArticulo(QWidget *parent = 0);
    ~BfBuscarTipoArticulo();
    virtual QString codtipo_articulo() const;
    virtual QString idtipo_articulo() const;
    virtual QString desctipo_articulo() const;
    virtual void setidtipo_articulo(QString val);
    virtual void setcodtipo_articulo(QString val);

};

#endif

