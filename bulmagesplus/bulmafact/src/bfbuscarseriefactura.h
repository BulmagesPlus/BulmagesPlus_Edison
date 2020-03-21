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

#ifndef BFBUSCARSERIEFACTURA_H
#define BFBUSCARSERIEFACTURA_H

#include "blcombobox.h"
#include "bfcompany.h"
#include "blpostgresqlclient.h"
#include "blwidget.h"


/// Permite  seleccionar una serie de factura.
/** Este Widget permite buscar y seleccionar un
    n&uacute;mero de serie de factura.
    Antes de utilizarlo debe ser inicializa con setcompany.
    Cuando se cambia el elemento seleccionado emite un SIGNAL valueChanged().
*/
class BF_EXPORT BfBuscarSerieFactura : public BlComboBox
{
    Q_OBJECT

private:
    /// Indica cual es el codigo de la serie de factura por defecto.
    QString m_codigoserie_factura;

public:
    explicit BfBuscarSerieFactura(QWidget *parent = 0);
    ~BfBuscarSerieFactura();
    void setMainCompany(BfCompany *company);
    QString id() const;
    virtual void setId(QString);
//    void m_activated(int index);
};

#endif
