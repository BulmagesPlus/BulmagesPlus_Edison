/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borrás Riera.                             *
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

#include <QKeyEvent>
#include "bldatelineedit.h"
#include "blfunctions.h"


/// Inicializa el objeto y hace todas las conexiones necesarias.
/**
\param parent
**/
BlDateLineEdit::BlDateLineEdit(QWidget * parent) : QLineEdit{parent}
{
  BL_FUNC_DEBUG

  installEventFilter(this);
  QObject::connect(this, SIGNAL(editingFinished()), this, SLOT(on_mui_editingFinished()));
}


///No requiere de acciones especiales en el destructor.
/**
**/
BlDateLineEdit::~BlDateLineEdit()
{
  BL_FUNC_DEBUG
}


///
/**
\param text
**/
void BlDateLineEdit::setText(QString text)
{
  BL_FUNC_DEBUG

  QLineEdit::setText(text);
}


///
/**
\param value
**/
void BlDateLineEdit::setFieldValue(QString value)
{
  BL_FUNC_DEBUG

  QLineEdit::setText(value);
}


///
/**
**/
void BlDateLineEdit::selectAll()
{
  BL_FUNC_DEBUG
}


///
/**
\return
**/
QString BlDateLineEdit::text() const
{
  BL_FUNC_DEBUG

  return QLineEdit::text();
}


///
/**
\return
**/
QString BlDateLineEdit::fieldValue() const
{
  BL_FUNC_DEBUG

  return QLineEdit::text();
}


///
/**
**/
void BlDateLineEdit::on_mui_editingFinished()
{
  BL_FUNC_DEBUG

  setText(blNormalizeDate(text()).toString("dd/MM/yyyy"));
}


///
/**
\param object
\param event
\return
**/
bool BlDateLineEdit::eventFilter(QObject * object, QEvent * event)
{
  BL_FUNC_DEBUG

  return QLineEdit::eventFilter(object, event);
}

