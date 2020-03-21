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

#include "blcomboboxdelegate.h"
#include "blwidget.h"
#include "blform.h"


/** Inicializa todos los componenetes a NULL para que no se produzcan confusiones
    sobre si un elemento ha sido creado o no.
*/
/**
\param parent
**/
BlComboBoxDelegate::BlComboBoxDelegate(QWidget *parent) : QComboBox{parent}, BlMainCompanyPointer{}
{
  BL_FUNC_DEBUG
}


///
/**
**/
BlComboBoxDelegate::~BlComboBoxDelegate()
{
  BL_FUNC_DEBUG

  /// Vaciamos el recordset en caso de que haya sido utilizado.
  if (m_comboRecordSet) delete m_comboRecordSet;
}


/** Este método sirve para indicar al Widget cuál es el item por defecto.
    Recarga todo el 'query' y compone de nuevo el comboBox estableciendo el item
    por defecto a aquel cuyo identificador coincide con el que se ha pasado por parámetro.
*/
/**
\param id
**/
void BlComboBoxDelegate::setId(QString id, QString fieldSearch)
{
  BL_FUNC_DEBUG

  try
  {
    /// Si el recordset ya ha sido utilizado lo borramos
    if (m_comboRecordSet) delete m_comboRecordSet;

    /// Cargamos el recordset
    m_comboRecordSet = mainCompany()->loadQuery(m_query);
    /// Si ha habido un problema con la base de datos salimos.
    if (!m_comboRecordSet) throw -1;

    int i{0};
    int i1{0};

    clear();

    if (m_null)
    {
      addItem(QString("--"));
      i++;
    } // end if

    while (!m_comboRecordSet->eof())
    {
      if (fieldSearch.isEmpty())
      {
        /// Busca en el campo ID establecido la coincidencia.
        if (m_comboRecordSet->value(m_fieldId) == id)
        {
          i1 = i;
        } // end if

      } else {
        /// Busca en un campo diferente al campo ID establecido la coincidencia.
        if (m_comboRecordSet->value(fieldSearch) == id)
        {
          i1 = i;
        } // end if
      } // end if

      /// Inicializamos los valores de vuelta a ""
      QMapIterator<QString, QString> it{m_valores};
      QString cad{""};

      while (it.hasNext())
      {
        it.next();
        if (!cad.isEmpty())
        {
          cad = cad + QString(" ");
        } // end if

        cad = cad  + m_comboRecordSet->value(it.key());
      } // end while

      addItem(cad);
      i++;
      m_comboRecordSet->nextRecord();
    } // end while

    setCurrentIndex(i1);
  } catch(...)
  {
    fprintf(stderr, "Error de Base de Datos en BlComboBoxDelegate::setId\n");
    exit(0);
  } // end try
}


///
/**
\param id
**/
void BlComboBoxDelegate::setFieldValue(QString fieldValue)
{
  BL_FUNC_DEBUG

  setId(fieldValue);
}


///
/**
\return
**/
QString BlComboBoxDelegate::id()
{
  BL_FUNC_DEBUG

  if (currentIndex() >= 0)
  {
    /// Si el campo tiene un elemento nulo, al seleccionarlo hay que devolver una cadena vacía,
    /// y si se selecciona otro se retorna el valor del número de elemento menos uno (sobra el valor nulo).
    if (m_null)
    {
      if (currentIndex() == 0)
      {
        return QString("");
      } else
      {
        return m_comboRecordSet->value(m_fieldId, currentIndex() - 1);
      } // end if

      /// Si el campo no tiene valor nulo, el orden coincide con el número de índice.
    } else
    {
      return m_comboRecordSet->value(m_fieldId, currentIndex());
    } // end if
  } // end if

  return QString("");
}


///
/**
\return
**/
QString BlComboBoxDelegate::id(QString value)
{
  BL_FUNC_DEBUG

  m_comboRecordSet->firstRecord();
  while (!m_comboRecordSet->eof())
  {
    /// Inicializamos los valores de vuelta a ""
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

    if (cad == value) return m_comboRecordSet->value(m_fieldId);

    m_comboRecordSet->nextRecord();
  } // end while

  return QString("");
}


///
/**
\return
**/
QString BlComboBoxDelegate::fieldValue()
{
  BL_FUNC_DEBUG

  return id();
}


///
/**
**/
void BlComboBoxDelegate::setQuery(QString query)
{
  BL_FUNC_DEBUG

  m_query = query;
}


///
/**
**/
void BlComboBoxDelegate::setTableName(QString tableName)
{
  BL_FUNC_DEBUG

  m_table = tableName;
}


///
/**
**/
void BlComboBoxDelegate::setFieldId(QString fieldId)
{
  BL_FUNC_DEBUG

  m_fieldId = fieldId;
}


///
/**
**/
void BlComboBoxDelegate::setAllowNull(bool allowNull)
{
  BL_FUNC_DEBUG

  m_null = allowNull;
}
