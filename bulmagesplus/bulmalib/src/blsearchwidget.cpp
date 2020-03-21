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

#include <QMenu>
#include <QDebug>

#include "blsearchwidget.h"
#include "blmaincompany.h"
#include "blplugins.h"


/** Inicializa la clase poniendo a NULL todos los valores para que no haya confusion
    en el hecho de que la clase aun no ha sido completamente inicializada.
*/
/**
\param parent
**/
BlSearchWidget::BlSearchWidget ( QWidget *parent )
        : BlWidget ( parent )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    mui_create->setVisible(false);
    m_textBusqueda->setText ( "" );
    mdb_id = "";
    m_campoid = "";

    /// Inicializamos los valores de vuelta a ""
    QMapIterator<QString, QString> i ( m_valores );
    while ( i.hasNext() ) {
        i.next();
        m_valores.insert ( i.key(), "" );
    } // end while

    m_semaforo = false;
    m_mask = "";
    
    /// Establecemos la delegacion del foco en el texto
    setFocusProxy(m_textBusqueda);
    
}


/** No requiere de acciones especiales en el destructor de clase.
*/
/**
**/
BlSearchWidget::~BlSearchWidget()
{
    BL_FUNC_DEBUG
    
}


/** Se encarga de presentar en el Widget los valores seleccionados.
*/
/** Esta funcion lanza un signal avisando de que se ha cambiado el id. Y debe tenerse en cuenta que el
    id puede estar vacio ya que tambien se puede haber borrado el que estaba puesto.
**/
void BlSearchWidget::pinta()
{
    BL_FUNC_DEBUG
    m_semaforo = true;
    QString cad = "";

    if (m_mask == "") {
      
	/// Iteramos y concatenamos"
	QMapIterator<QString, QString> i ( m_valores );
	if ( i.hasNext() ) {
	    i.next();
	    QString valor = m_valores[i.key()];
	    if (!valor.isEmpty()) m_inputBusqueda->setText(valor);
	} // end if
	while ( i.hasNext() ) {
	    i.next();
	    cad = cad + " " + m_valores.value ( i.key() );
	}

    } else {
      
	cad = m_mask;
	/// Iteramos y reemplazamos
	QMapIterator<QString, QString> i ( m_valores );
	while ( i.hasNext() ) {
	    i.next();
	    cad.replace("["+i.key()+"]", m_valores.value(i.key()));
	}
      
      
    } // end if

    if ( m_encontrado )
    {
      m_textBusqueda->setText ( cad );
      mui_create->setVisible(false);
    } // end if

    m_semaforo = false;
    emit ( valueChanged ( mdb_id ) );
    
}


/** Mediante este metodo indicamos al Widget que cargue un elemento determinado.
    Busca en la base de datos un elemento que coincida con el identificador pasado
    y si lo encuentra lo carga y lo presente.
    Dicho elemento se considera el elemento seleccionado.
    Al finalizar llama al metodo pinta() para que se refelejen visualmente los cambios.
*/
/**
\param val
**/
void BlSearchWidget::setId ( QString val, bool cargarvalores )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSearchWidget::setId", 0, val );
    mdb_id = val;

    if ( m_tabla == "" || !cargarvalores) {
        return;
    } // end if

    if ( val == "" ) {
        m_inputBusqueda->setText ( "" );
        m_textBusqueda->setText ( "" );
        mdb_id = "";
        /// Inicializamos los valores de vuelta a ""
        QMapIterator<QString, QString> i ( m_valores );
        while ( i.hasNext() ) {
            i.next();
            m_valores.insert ( i.key(), "" );
        } // end while
        m_encontrado = false;
    } else {
        QString SQLQuery("");
        SQLQuery = "SELECT * FROM " + m_tabla + " WHERE " + m_campoid + "= $1";
        BlDbRecordSet *cur = mainCompany() ->load( SQLQuery, mdb_id );
        if ( !cur->eof() ) {
            /// Inicializamos los valores de vuelta a ""
            QMapIterator<QString, QString> i ( m_valores );
            while ( i.hasNext() ) {
                i.next();
                m_valores.insert ( i.key(), cur->value( i.key() ) );
            } // end while
            mui_create->setVisible(false);
            m_encontrado = true;
        } // end if
        delete cur;
    } // end if
    
    pinta();
    
}


///
/**
\param val
**/
void BlSearchWidget::setFieldValue(QString value)
{
    BL_FUNC_DEBUG

    setId(value);
}

///
/**
\param val
**/
void BlSearchWidget::setText ( const QString &val )
{
    BL_FUNC_DEBUG
    m_inputBusqueda->setText(val);
    
}


/** Mediante este metodo le decimos al Widget cual es el CIF del elemento seleccionado.
    Busca en la base de datos cual el elemento cuyo CIF coincida con el que hemos seleccionado
    y si lo encuentra lo pinta y lo considera el elemento seleccionado.
    Al final llama al metodo pintar() para garantizar la correcta visualizacion del elemento en pantalla.
*/
/**
\param val
**/
void BlSearchWidget::setFieldValue(QString fieldName, QString value)
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSearchWidget::setFieldValue", 0, value );

    QString SQLQuery("");
    SQLQuery = "SELECT * FROM " + m_tabla + " WHERE " + fieldName + " = $1";
    BlDbRecordSet *rs = mainCompany()->load(SQLQuery, value);

    if (!rs->eof())
    {
        /// Inicializamos los valores de vuelta a ""
        QMapIterator<QString, QString> i(m_valores);
        while (i.hasNext())
	{
            i.next();
            m_valores.insert(i.key(), rs->value(i.key()));
        } // end while
    }
    else
    {
        /// Inicializamos los valores de vuelta a ""
        QMapIterator<QString, QString> i(m_valores);
        while (i.hasNext())
        {
            i.next();
            m_valores.insert(i.key(), "");
        } // end while
    } // end if
    delete rs;

    pinta();
}


/** Resetea el valor del 'id' y vacia la informacion del formulario.
*/
void BlSearchWidget::on_m_clearSearchWidget_clicked()
{
    BL_FUNC_DEBUG
    setId ( "" );
    mui_create->setVisible(false);
}


/** SLOT que responde a la accion de buscar.
    Abre el listado en modo Seleccionar y espera a que cerremos
    dicha ventana o que seleccionemos un elemento.
    Una vez determinado el elemento lo considera como el elemento seleccionado
    y lo muestra.
*/
/**
**/
void BlSearchWidget::on_m_buscarWidget_clicked()
{
    BL_FUNC_DEBUG
    /// Disparamos los plugins
    g_plugins->run ( "Busqueda_on_mui_buscar_clicked", this );
    
}


/** SLOT que responde al fin de la edicion del campos de texto.
    Se supone que hemos introducido un elemento identificable por codigo, por nombre o por CIF
    Asi que se encarga de buscarlo y encontrarlo.
*/
/**
**/
void BlSearchWidget::on_m_inputBusqueda_editingFinished()
{
    BL_FUNC_DEBUG
    pinta();
    g_plugins->run ( "Busqueda_on_m_inputBusqueda_editingFinished_Post", this );
    
}

/** SLOT que responde a la modificacion del campo de texto del Widget.
    A medida que vamos escribiendo en el campo de textos el sistema va buscando
    posibles elementos que cumplan las caracteristicas. Si encuentra solo uno lo
    considera como el elemento seleccionado.
*/
/**
\param val
\return
**/
void BlSearchWidget::on_m_inputBusqueda_textChanged ( const QString &val )
{
    BL_FUNC_DEBUG
    
    /// Si la cadena esta vacia entonces salimos sin hacer nada
    if  (val == "") {
        setId ( "" );
        mui_create->setVisible(false);
        return;
    } // end if
    
    if ( m_semaforo ) {
	
        return;
    } // end if


    if ( g_plugins->run ( "Busqueda_on_m_inputBusqueda_textChanged", this ) ) {
        return;
    } // end if


    QString cadwhere = "";
    /// Inicializamos los valores de vuelta a ""
    QMapIterator<QString, QString> i ( m_valores );
    QString cador = "";
    while ( i.hasNext() ) {
        i.next();
        cadwhere = cadwhere + cador + " " + i.key() + "::varchar = '" + mainCompany()->sanearCadenaUtf8(val) + "'";
        cador = " OR ";
    } // end while


    m_encontrado = false;
    QString SQLQuery = "SELECT * FROM " + m_tabla + " WHERE " + cadwhere;
    BlDbRecordSet *cur = mainCompany() ->loadQuery ( SQLQuery );
    if ( !cur->eof() ) {
        mdb_id = cur->value( m_campoid );
        /// Inicializamos los valores de vuelta a ""
        QMapIterator<QString, QString> i ( m_valores );
        while ( i.hasNext() ) {
            i.next();
            m_valores[i.key() ] = cur->value( i.key() );
        } // end while
        m_encontrado = true;
    } // end if
    delete cur;



    if ( ! m_encontrado ) {
        cadwhere = "";
        /// Inicializamos los valores de vuelta a ""
        QMapIterator<QString, QString> i ( m_valores );
        QString cador = "";
        while ( i.hasNext() ) {
            i.next();
            cadwhere = cadwhere + cador 
                        + " upper(" + i.key() + "::varchar ) LIKE  upper('%" 
                        + mainCompany()->sanearCadenaUtf8(val) + "%')";
            cador = " OR ";
        } // end while

        QString SQLQuery = "SELECT * FROM " + m_tabla + " WHERE " + cadwhere;
        cur = mainCompany() ->loadQuery ( SQLQuery );
        if ( cur->numregistros() == 1 ) {
            mdb_id = cur->value( m_campoid );
            /// Inicializamos los valores de vuelta a ""
            QMapIterator<QString, QString> i ( m_valores );
            while ( i.hasNext() ) {
                i.next();
                m_valores.insert ( i.key(), cur->value( i.key() ) );
            } // end while
            m_encontrado = true;
        } // end if
        delete cur;
    } // end if


    if ( !m_encontrado ) {
        m_textBusqueda->setText ( "" );
        mdb_id = "";
        /// Inicializamos los valores de vuelta a ""
        QMapIterator<QString, QString> i ( m_valores );
        while ( i.hasNext() ) {
            i.next();
            m_valores[i.key() ] = "";
        } // end while
        m_textBusqueda->setText ( _("<Dato no encontrado>") );

        /// tienes permisos para insertar'
        if (!m_tabla.isEmpty())
        {
          if (mainCompany()->hasTablePrivilege(m_tabla, "INSERT") && m_canCreate)
          {
              mui_create->setVisible(true);
          } // end if
        } // end if
    } // end if

    if ( m_encontrado ) {
        QString cad = "";
        QMapIterator<QString, QString> i ( m_valores );
        while ( i.hasNext() ) {
            i.next();
            cad = cad + " " + i.value();
        } // end while
        m_textBusqueda->setText ( cad );
        mui_create->setVisible(false);
    } // end if
    

}


///
/**
\return
**/
QString BlSearchWidget::fieldValue(QString fieldName) const
{
    BL_FUNC_DEBUG
    
    return m_valores[fieldName];
}

///
/**
**/
QString BlSearchWidget::id() const
{
    BL_FUNC_DEBUG
    
    return mdb_id;
}



///
/**
**/
void BlSearchWidget::setFieldId ( const QString &fieldId )
{
    BL_FUNC_DEBUG
    m_campoid= fieldId;
}

///
/**
\return
**/
QString BlSearchWidget::fieldValue() const
{
    BL_FUNC_DEBUG
    
    return mdb_id;
}


void BlSearchWidget::setLabel ( QString label )
{
    mui_labelBusqueda->setText ( label  );
    mui_labelBusqueda->setBuddy ( m_buscarWidget );
}


QString BlSearchWidget::tableName() const
{
    return m_tabla;
}

void BlSearchWidget::setTableName ( QString tableName )
{
    m_tabla = tableName;
    if (m_campoid == "") {
	m_campoid = "id" + m_tabla;
    } // end if
}

void BlSearchWidget::selectAll ( )
{
    m_inputBusqueda->selectAll ( );
}

void BlSearchWidget::setFocus ( )
{
    m_inputBusqueda->setFocus ( );
}

QString BlSearchWidget::text() const
{
    return m_inputBusqueda->text();
}

void BlSearchWidget::hideLabel()
{
  mui_labelBusqueda->hide();
}

void BlSearchWidget::hideDescripcion()
{
  m_textBusqueda->hide();
}

void BlSearchWidget::hideBotonBorrar()
{
  m_clearSearchWidget->hide();
}

void BlSearchWidget::hideBotonBusqueda()
{
  m_buscarWidget->hide();
}

QString BlSearchWidget::fieldId()
{
  return m_campoid;
}

QString BlSearchWidget::mask() const
{
  return m_mask;
}


void BlSearchWidget::setMask(const QString & val){
  m_mask = val;
}


bool BlSearchWidget::canCreate()
{
  return m_canCreate;
}


void BlSearchWidget::setCanCreate(bool canCreate)
{
    m_canCreate = canCreate;
}


void BlSearchWidget::on_mui_create_clicked()
{
    BL_FUNC_DEBUG
    /// Disparamos los plugins
    g_plugins->run ( "Busqueda_on_mui_create_clicked", this );
}


