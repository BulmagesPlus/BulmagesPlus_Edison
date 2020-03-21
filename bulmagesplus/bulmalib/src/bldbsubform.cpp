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
#include <QEvent>
#include <QFile>
#include <QLocale>
#include <QRegExp>

#include "bldbsubform.h"
#include "blsubform.h"


/// Inicia el constructor
/**
\param con Empresa con la que va a trabajar
\todo Se debe eliminar el contador una vez testeado el consumo de memoria
**/
BlDbSubFormRecord::BlDbSubFormRecord(BlMainCompany *mainCompany) : BlDbRecord(mainCompany)
{
    BL_FUNC_DEBUG
}


/// Destruye la clase
/**
**/
BlDbSubFormRecord::~BlDbSubFormRecord()
{
    BL_FUNC_DEBUG
}


///
/**
\param id Devuelve el identificador del registro guardado
\return Si todo ha ido bien devuelve 0
**/
int BlDbSubFormRecord::dbSave ( QString &id )
{
    BL_FUNC_DEBUG

    refresh();

    return BlDbRecord::dbSave(id);
}


/// Actualiza los contenidos del subformulario acorde a los valores de la tabla
/**
Actualiza los valores del registro acorde a los valores que tiene la tabla que lo contiene
**/
void BlDbSubFormRecord::refresh()
{
    BL_FUNC_DEBUG

    for (int i = 0; i < m_lista.size(); ++i)
    {
        BlDbSubFormField *field = static_cast<BlDbSubFormField *>(m_lista.at(i));
        field->refresh();
    } // end for
}


/// Agrega un campo al registro
/**
\param nom Nombre del campo
\param typ Tipo del campo
\param res Restricciones a aplicar en el campo
\param nomp Nombre a mostrar en caso de error
\return
**/
int BlDbSubFormRecord::addDbField(QString nom, BlDbField::DbType typ, int res, QString nomp, int type)
{
    BL_FUNC_DEBUG

    BlDbSubFormField *field = new BlDbSubFormField(this, m_dbConnection, nom, typ, res, nomp, type);
    field->set("");
    m_lista.append(field);

    return 0;
}


/// Constructor de BlDbSubFormField. Representa, maneja y alberga un campo de un registro de un recordset.
/**
\param par El registro padre del campo
\param com La base de datos con la que esta trabajando
\param nom El nombre del campo
\param typ El tipo de campo
\param res Las restricciones del campo
\param nomp el nombre a presentar en caso de error.
**/
BlDbSubFormField::BlDbSubFormField ( BlDbSubFormRecord *par, BlPostgreSqlClient *com, QString nom, DbType typ, int res, QString nomp, int type)
        : BlTableWidgetItem(type), BlDbField ( com, nom, typ, res, nomp )
{
    BL_FUNC_DEBUG

    m_pare = par;
}


///
/**
**/
BlDbSubFormField::~BlDbSubFormField()
{
    BL_FUNC_DEBUG

    /// Eliminamos el item en la lista del padre ya que al estar eliminado no puede seguir alli.
    if (pare())
    {
      if (pare()->lista())
      {
          pare()->lista()->removeAll(this);
      } // end if
    } // end if
}


/// Refresco de los contenidos de un campo con lo que se esta visualizando
/**
Tiene especial intereses con los campos checkables ya que su tratamiento no es directo
**/
void BlDbSubFormField::refresh()
{
    BL_FUNC_DEBUG

    if ( this->fieldType() == BlDbField::DbBoolean ) {
        switch ( checkState() ) {
            case Qt::Checked:
                BlDbField::set ( "TRUE" );
                break;
            case Qt::Unchecked:
                BlDbField::set ( "FALSE" );
                break;
            case Qt::PartiallyChecked:
            default:
                /// Esta opci&oacute;n se usa si el campo SQL admite el valor nulo y por tanto tenemos un CheckBox triestado
                BlDbField::set ( "" );
                break;
        } // end switch
     } else {
        BlDbField::set ( text() );
     } // end if
}


/// Establece el valor que debe tener un campo
/**
Al introducir un valor en el campo ya se hacen las posibles conversiones que puedan interesar
para la presentacion de valores corregidos a un estandar.
\param val Valor que toma el campo
\return Si no hay errores devuelve 0
**/
int BlDbSubFormField::set ( QString val )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlDbSubFormField::set", 0, fieldName() + " = " + val );

    BlDbField::set ( val );
    QRegExp importe ( "^\\d*\\.\\d{2}$" ); ///< Para emparejar los valores numericos con decimales


    if ( fieldType() == BlDbField::DbBoolean ) {
        if ( fieldRestrictions() == BlSubFormHeader::DbNoWrite ) {
            setFlags ( this->flags() & ( ~Qt::ItemIsUserCheckable ) );
        } // end if

        if ( val == "TRUE" || val == "t" ) {
            setCheckState ( Qt::Checked );
        } else if ( val == "FALSE" || val == "f" ) {
            setCheckState ( Qt::Unchecked );
        } else {
            /// El triestado si usa si el campo booleano admite el valor nulo.
            setCheckState ( Qt::PartiallyChecked );
        } // end if
    } else if ( fieldType() == BlDbField::DbNumeric && importe.exactMatch ( val ) ) {
        setText ( fieldValue() );
    } else if ( fieldType() == BlDbField::DbDate ) {
        setText ( val.left ( 10 ) );
    } else {
	  setText ( fieldValue() );

	  if ( g_confpr->value( CONF_CENTER_TEXT_FIELDS ) == "TRUE"
	  && fieldType() == BlDbField::DbVarChar )
	  {
		setTextAlignment(Qt::AlignCenter);
	  }
    } // end if
    return 0;
}


/// Comparacion entre campos
/**
\param other El campo con el que hay que comprarar
\return true si se considere este el campo mas grande, false en caso contrario.
**/
bool BlDbSubFormField::operator< ( const QTableWidgetItem &other )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlDbSubFormField::operator <", 0, text() );
    BlDbSubFormField *ot = ( BlDbSubFormField * ) & other;
    DbType tip = ot->fieldType();
    if ( tip == this->fieldType() ) {
        QString val = ot->fieldValue();

        if ( this->fieldType() == BlDbField::DbNumeric || this->fieldType() == BlDbField::DbInt ) {
            BlDebug::blDebug ( "BlDbSubFormField::operator < es del tipo numerico:", 0, this->fieldName() + QString::number ( this->fieldType() ) );
            double db1 = this->fieldValue().toDouble();
            double db2 = val.toDouble();
            return ( db1 < db2 );
        } // end if

        if ( this->fieldType() == BlDbField::DbDate ) {
            BlDebug::blDebug ( "BlDbSubFormField::operator < es del tipo fecha:", 0, this->fieldName() + QString::number ( this->fieldType() ) );
		QDate fech = blNormalizeDate ( this->fieldValue() );
            QString db1 = fech.toString ( Qt::ISODate );
		QDate fech1 = blNormalizeDate ( val );
            QString db2 = fech1.toString ( Qt::ISODate );
            return ( db1 < db2 );
        } // end if

        if ( this->fieldType() == BlDbField::DbVarChar ) {
            BlDebug::blDebug ( "BlDbSubFormField::operator < es del tipo varchar:", 0, this->fieldName() + QString::number ( this->fieldType() ) );
            return ( this->fieldValue() < val );
        } // end if

        if ( this->fieldType() == BlDbField::DbTime ) {
    	    // TODO
    	    /// Falta revisar que funcione este comparacion de valores.
            BlDebug::blDebug ( "BlDbSubFormField::operator < es del tipo time:", 0, this->fieldName() + QString::number ( this->fieldType() ) );
            return ( this->fieldValue() < val );
        } // end if
        BlDebug::blDebug ( "tipo desconocido", 0 );
    }
    
    return false;
}


/// Devuelve el registro que alberga este campo
/**
Es util porque se activan signals sobre campos determinados en los que no sabremos a que registro
pertencecen
\return Puntero al registro
**/
BlDbSubFormRecord *BlDbSubFormField::pare()
{
    BL_FUNC_DEBUG
    return m_pare;
}

