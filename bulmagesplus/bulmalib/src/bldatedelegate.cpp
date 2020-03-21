/***************************************************************************
 *   Copyright (C) 2010 by Aron Galdón.                                    *
 *   Copyright (C) 2012 by Fco. Javier Miró Carmona.                       *
 *                                                                         *
 *   https://bulmagesplus.com                                              *
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

#include "bldatedelegate.h"
#include "bldatesearch.h"


/// Constructor
/**
   \param company
   \param parent
   \param allowNull
**/
BlDateDelegate::BlDateDelegate(BlMainCompany * company, QObject * parent, bool allowNull)
               : QStyledItemDelegate{parent},
                 m_company{company},
                 m_allowNull{allowNull}
{
   BL_FUNC_DEBUG
}


/// Crear el calendario que servirá para editar el campo.
/**
   \param parent
   \param vis Parámetros de visualización para el elemento en la vista.
   \param index Índice en el modelo del dato a editar.
**/
QWidget * BlDateDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & vis, const QModelIndex & index) const
{
  BL_FUNC_DEBUG

  BlDateSearch *dateSearch{new BlDateSearch{parent}};
  dateSearch->setMainCompany(m_company);
  return dateSearch;
}


/// Poner el buscador de fecha en la posición indicada por el valor del campo a editar.
/**
   \param editor
   \param index Índice en el modelo del dato a editar
**/
void BlDateDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
  BL_FUNC_DEBUG

  BlDateSearch *dateSearch{dynamic_cast<BlDateSearch *>(editor)};
  if (dateSearch)
  {
    QString date{index.model()->data(index).toString()};

    /// Si no hay fecha previa, usar la de hoy si no se permite el valor nulo.
    if (date.isEmpty() && !m_allowNull)
    {
      date = QDate::currentDate().toString("dd/MM/yyyy");
    } // end if

    dateSearch->setDate(date);
  } // end if
}


/// Guarda los cambios realizados en el widget sobre los datos asociados en la columna.
/**
   \param editor
   \param model
   \param index Índice en el modelo del dato a editar.
**/
void BlDateDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
  BL_FUNC_DEBUG

  BlDateSearch *dateSearch{dynamic_cast<BlDateSearch *>(editor)};
  if (dateSearch)
  {
    QString date{dateSearch->text()};
    model->setData(index, date);
  } // end if
}


/// Establece el tamaño y posición del widget editor.
/**
   \param editor
   \param vis Parámetros de visualización para el elemento en la vista.
   \param index Índice en el modelo del dato a editar.
**/
void BlDateDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & vis, const QModelIndex & index) const
{
  BL_FUNC_DEBUG

  editor->setGeometry(vis.rect);
}


///
/**
  \param allowNull
**/
void BlDateDelegate::setAllowNull(bool allowNull)
{
  BL_FUNC_DEBUG

  m_allowNull = allowNull;
}
