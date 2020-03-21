/***************************************************************************
 *   Copyright (C) 2007 by Tomeu Borras Riera                              *
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

#ifndef BFBUSCARBANCO_H
#define BFBUSCARBANCO_H

#include "blcombobox.h"

#include "blfunctions.h"


/// Permite buscar y seleccionar un trabajador.
/** Este comboBox permite buscar y seleccionar un
    trabajador. Aparece en forma de desplegable.
    Antes de utilizarlo debe ser inicializado con setcompany().
    Cuando se cambia el valor del Widget se emite un SIGNAL valueChanged().
*/
class BF_EXPORT BfBuscarBanco : public BlComboBox
{
    Q_OBJECT

private:
    /// Cursor que almacena el listado de trabajadores y sobre el que se construye el comboBox.
    BlDbRecordSet *m_cursorcombo;

public:
    explicit BfBuscarBanco(QWidget *parent = 0);
    ~BfBuscarBanco();
    void setId(QString idbanco) override;
    virtual void setFieldValue(QString idbanco);
    QString id() const override;
    virtual QString fieldValue() const;

public slots:
    void activatedItem(int index);

signals:
    void valueChanged(QString);
};

#endif
