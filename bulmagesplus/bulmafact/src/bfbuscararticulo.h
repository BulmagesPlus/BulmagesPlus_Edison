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

#ifndef BUSQUEDAARTICULO_H
#define BUSQUEDAARTICULO_H

#include <QLineEdit>
#include <QLabel>

#include "blcombobox.h"
#include "blpostgresqlclient.h"
#include "blsearchwidget.h"


class BfCompany;

/*

class BF_EXPORT BfBuscarArticuloDelegate : public BlComboBox
{
    Q_OBJECT

private:
    /// Este cursor almacena el listado de series de factura para poder trabajar con ellas.
    BlDbRecordSet *m_cursorcombo;
    /// Texto entrado por el usuario (para uso de los plugins)
    QString m_entrada; 

public:
    BfBuscarArticuloDelegate ( QWidget *parent = 0 );
    ~BfBuscarArticuloDelegate();
    QString entrada();
    QString unicaEleccion(void) ;
    QString eligeUnico(void) ;

public slots:
    virtual void s_editTextChanged ( const QString & );
    virtual void focusOutEvent ( QFocusEvent * event );

};

*/
/// Permite buscar y seleccionar un art&iacute;culo.
/** Muestra la parte del formulario que permite buscar y seleccionar un
    art&iacute;culo. */
class BF_EXPORT BfBuscarArticulo : public BlSearchWidget
{
    Q_OBJECT

public:
    explicit BfBuscarArticulo(QWidget *parent = 0);
    ~BfBuscarArticulo();
    virtual QString codigocompletoarticulo() const;
    virtual QString idarticulo() const;
    virtual QString nomarticulo() const;
    virtual void setidarticulo(QString val);
    virtual void setcodigocompletoarticulo(QString val);
};

#endif
