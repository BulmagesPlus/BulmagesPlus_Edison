/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borras Riera                              *
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

#ifndef BFSUBFORM_H
#define BFSUBFORM_H

#include <QEvent>

#include "bfcompany.h"
#include "bltablewidget.h"
#include "bldbsubform.h"
#include "blsubform.h"
#include "bldoublespinbox.h"


class BfSubForm;


/// Clase BfSubFormDelegate
/** Se encarga del control de los 'Widgets' de edici&oacute;n del sistema.*/
class BF_EXPORT BfSubFormDelegate : public BlSubFormDelegate
{
    Q_OBJECT

public:
    BfSubFormDelegate ( QObject * );
    virtual ~BfSubFormDelegate();
    virtual void setEditorData ( QWidget *, const QModelIndex &index ) const override;
    virtual void setModelData ( QWidget *editor,  QAbstractItemModel *model, const QModelIndex &index ) const override;
    virtual QWidget *createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
    virtual void updateEditorGeometry ( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
};


/// Clase BfSubForm
/** Es la primera abstraccion de los subformularios que incluye todas las caracter&iacute;sticas
    comunes de la facturaci&oacute;n.
*/
class BF_EXPORT BfSubForm : public BlSubForm
{
    Q_OBJECT

public:
    BfSubFormDelegate *m_delegate;

protected:
    QString mdb_idcliente;
    QString mdb_idproveedor;
    QString m_idArticulo;
    QString m_idTarifa;
    QString m_idAlmacen;

public:
    explicit BfSubForm(QWidget *parent = 0);
    virtual ~BfSubForm();
    virtual void load ( QString query );
    virtual void setMainCompany ( BlMainCompany * );
    virtual int cerrarEditor();
    void setIdCliente ( QString id );
    void setIdProveedor ( QString id );
    void setIdAlmacen ( QString id );
    QString idcliente();
    QString idproveedor();
    QString idArticulo();
    QString idTarifa();
    QString idAlmacen();

public slots:
    virtual void editFinished ( int row, int col, BlDbSubFormRecord *rec, BlDbSubFormField *camp ) override;
    virtual void pressedAsterisk ( int row, int col, BlDbSubFormRecord *rec, BlDbSubFormField *camp ) override;
    virtual void pressedMinus ( int row, int col, BlDbSubFormRecord *rec, BlDbSubFormField *camp ) override;
    virtual void pressedSlash ( int row, int col, BlDbSubFormRecord *rec, BlDbSubFormField *camp ) override;

    virtual void calculaPVP(BlDbSubFormRecord *rec);
};

#endif

