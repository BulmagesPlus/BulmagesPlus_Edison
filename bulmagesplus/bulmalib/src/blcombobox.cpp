/***************************************************************************
 *   Copyright (C) 2006 by Tomeu Borrás Riera.                             *
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

#include <QStyledItemDelegate>
#include "blcombobox.h"
#include "blwidget.h"
#include "blform.h"


/** Inicializa todos los componenetes a NULL para que no se produzcan confusiones
    sobre si un elemento ha sido creado o no.
    Conecta el activar un tipo con 'activatedItem'.
*/
/**
\param parent
**/
BlComboBox::BlComboBox(QWidget *parent) : QComboBox{parent}, BlMainCompanyPointer{}
{
  BL_FUNC_DEBUG

  QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
  setItemDelegate(itemDelegate);

  connect(this, SIGNAL(activated(int)), this, SLOT(activatedItem(int)));
  connect(g_theApp, SIGNAL(dbTableChanged(const QString &)), this, SLOT(onDbTableChanged(const QString &)));
  m_allowNull = true;
}


///
/**
**/
BlComboBox::~BlComboBox()
{
  BL_FUNC_DEBUG

  if (m_comboRecordSet) delete m_comboRecordSet;
}


///
/**
**/
void BlComboBox::onDbTableChanged(const QString &tableName)
{
  BL_FUNC_DEBUG

  if (m_tableName == tableName) setId(id());
}


/** Este metodo sirve para indicar al Widget cual es la item por defecto.
    Recarga todo el query y compone de nuevo el comboBox estableciendo el item
    por defecto aquella cuyo identificador coincide con el que se ha pasado por parametro.
*/
/**
\param id
**/
void BlComboBox::setId(QString id)
{
  BL_FUNC_DEBUG

  try
  {
    /// En caso de que el recordset este vacio.
    if (m_comboRecordSet) delete m_comboRecordSet;

    /// Cargamos el recordset con la consulta.
    m_comboRecordSet = mainCompany()->loadQuery(m_query);

    /// Si ha habido un problema con la base de datos generamos una excepcion.
    if (!m_comboRecordSet) throw -1;

    int i{0};
    int i1{0};

    clear();

    if (m_allowNull)
    {
      addItem(QString("--"));
      i++;
    } // end if

    while (!m_comboRecordSet->eof())
    {
      if (m_comboRecordSet->value(m_fieldId) == id)
      {
        i1 = i;
      } // end if

      /// Inicializamos los valores de vuelta a "".
      QMapIterator<QString, QString> it{m_valores};
      QString cad{""};

      while (it.hasNext())
      {
        it.next();
        if (!cad.isEmpty())
        {
          cad = cad + QString(" ");
        } // end if

        cad = cad + m_comboRecordSet->value(it.key());
      } // end while

      addItem(cad);
      i++;
      m_comboRecordSet->nextRecord();
    } // end while

    setCurrentIndex(i1);
  } catch(...)
  {
    fprintf(stderr, _("Error de Base de Datos en BlComboBox::setId.\n").toLatin1());
    exit(0);
  } // end try
}


///
/**
\param id
**/
void BlComboBox::setFieldValue(QString id)
{
  BL_FUNC_DEBUG

  setId(id);
}


/// SLOT que se activa al activar un elemento del desplegable
/**
\param index
**/
void BlComboBox::activatedItem(int index)
{
  BL_FUNC_DEBUG

  if (index > 0)
  {
    if (m_allowNull)
    {
      emit(valueChanged(m_comboRecordSet->value(m_fieldId, index - 1)));

    } else
    {
      emit(valueChanged(m_comboRecordSet->value(m_fieldId, index)));
    } // end if

  } else
  {
    emit(valueChanged(QString("")));
  } // end if
}


///
/**
\return
**/
QString BlComboBox::id() const
{
  BL_FUNC_DEBUG

  QString result {""};

  if (currentIndex() >= 0)
  {
    /// Si el campo tiene un elemento en blanco ("--"), al seleccionarlo hay que devolver una cadena vacía,
    /// y si se selecciona otro se retorna el valor del número de elemento menos uno (sobra el valor en blanco).
    /// Si el campo no tiene elemento en blanco, el orden coincide con el número de índice.
    if (m_allowNull)
    {
      if (currentIndex() > 0)
      {
        if (m_comboRecordSet)
        {
          result = m_comboRecordSet->value(m_fieldId, currentIndex() - 1);
        }
        else
        {
          result = QString::number(currentIndex());
        } // end if
      } // end if
    }
    else
    {
      if (m_comboRecordSet)
      {
        result = m_comboRecordSet->value(m_fieldId, currentIndex());
      }
      else
      {
        result = QString::number(currentIndex());
      } // end if
    } // end if
  } // end if

  return result;
}


///
/**
\return
**/
QString BlComboBox::fieldValue() const
{
  BL_FUNC_DEBUG

  return id();
}


///
/**
**/
void BlComboBox::setQuery(QString query)
{
  BL_FUNC_DEBUG

  m_query = query;
}


///
/**
**/
void BlComboBox::setTableName(QString tableName)
{
  BL_FUNC_DEBUG

  m_tableName = tableName;
}


///
/**
**/
QString BlComboBox::tableName() const
{
  BL_FUNC_DEBUG

  return m_tableName;
}


///
/**
**/
void BlComboBox::setFieldId(QString fieldId)
{
  BL_FUNC_DEBUG

  m_fieldId = fieldId;
}


///
/**
**/
void BlComboBox::setAllowNull(bool allowNull)
{
  BL_FUNC_DEBUG

  m_allowNull = allowNull;
}


///
/**
\return
**/
bool BlComboBox::isAllowNull() const
{
  BL_FUNC_DEBUG

  return m_allowNull;
}


void BlComboBox::focusOutEvent(QFocusEvent * event)
{
  emit focusOut(event);
  QComboBox::focusOutEvent(event);
}


void BlComboBox::focusInEvent(QFocusEvent * event)
{
  emit focusIn(event);
  QComboBox::focusInEvent(event);
}

