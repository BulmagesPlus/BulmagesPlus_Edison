/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
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

#include "bfbuscararticulo.h"
#include "bfcompany.h"
#include "blfunctions.h"
#include "blplugins.h"

/** Inicializa todos los componentes a null para que no haya posibles
    errores al introducir el puntero a company. */
/**
\param parent
**/
BfBuscarArticulo::BfBuscarArticulo ( QWidget *parent )
        : BlSearchWidget ( parent )
{
    BL_FUNC_DEBUG

    /// Establecemos los parametros de busqueda del Cliente
    setLabel ( _ ( "Articulo:" ) );
    setTableName ( "articulo" );
    m_valores["codigocompletoarticulo"] = "";
    m_valores["nomarticulo"] = "";

    
}


/** El destructor no requiere de acciones especiales.
*/
/**
**/
BfBuscarArticulo::~BfBuscarArticulo()
{
    BL_FUNC_DEBUG
    
}


/** Con este metodo indicamos al Widget que articulo presentar como cargado.
    Lo busca en la base de datos y muestra el codigocompleto y el nombre.
*/
/**
\param val
**/
void BfBuscarArticulo::setidarticulo ( QString val )
{
    BL_FUNC_DEBUG
    setId ( val );
    
}




/** Con este metodo indicamos cual es el articulo que debe presentar el Widget
    como seleccionado, indicando cual es su codigocompleto ya que es un indice
    muy utilizado con los articulos.
*/
/**
\param val
**/
void BfBuscarArticulo::setcodigocompletoarticulo ( QString val )
{
    BL_FUNC_DEBUG
    QString SQLQuery = "SELECT * FROM articulo WHERE codigocompletoarticulo=$1";
    BlDbRecordSet *cur = mainCompany() ->load ( SQLQuery, val );
    if ( !cur->eof() ) {
        setId ( cur->value( "idarticulo" ) );
    } else {
        setId ( "" );
    } // end if
    delete cur;
    
}



///
/**
\return
**/
QString BfBuscarArticulo::codigocompletoarticulo() const
{
    BL_FUNC_DEBUG

    return m_valores["codigocompletoarticulo"];
}


///
/**
\return
**/
QString BfBuscarArticulo::idarticulo() const
{
    BL_FUNC_DEBUG

    return id();
}

///
/**
\return
**/
QString BfBuscarArticulo::nomarticulo() const
{
    BL_FUNC_DEBUG

    return m_valores["nomarticulo"];
}


#ifdef ESTO_NO_PASARA

/// ===================================================================
/// Busqueda Articulo Delegate para usar con los subforms
/// ===================================================================
/** Inicializa todos los componentes del Widget a NULL para que no haya posibles confusiones
    sobre si un elemento ha sido creado o no.
    Conecta el SIGNAL activated() con m_activated() para tratarlo.
*/
/**
\param parent
**/
BfBuscarArticuloDelegate::BfBuscarArticuloDelegate ( QWidget *parent )
        : BlComboBox ( parent )
{
    BL_FUNC_DEBUG
    m_cursorcombo = NULL;
    setEditable ( true );
//    connect ( this, SIGNAL ( activated ( int ) ), this, SLOT ( m_activated ( int ) ) );
    connect ( this, SIGNAL ( editTextChanged ( const QString & ) ), this, SLOT ( s_editTextChanged ( const QString & ) ) );
    
}


/** Libera la memoria reservada. */
/**
**/
BfBuscarArticuloDelegate::~BfBuscarArticuloDelegate()
{
    BL_FUNC_DEBUG
    
}


/** Permite indicar al Widget cual es la serie de factura seleccionada por defecto.
    Recarga cursor de serie_factura y cuando encuentra un registro cuyo codigoserie_factura coincide con el pasado
    como parametro lo establece como el registro activo por el comboBox.
*/
/**
\param cod
\return
**/
void BfBuscarArticuloDelegate::s_editTextChanged ( const QString &cod )
{
    BL_FUNC_DEBUG
    static bool semaforo = false;

    if ( semaforo ) {
        
        return;
    } else {
        semaforo = true;
    } // end if
    m_entrada = cod;
    if ( !g_plugins->run ( "BfBuscarArticuloDelegate_textChanged", this ) ) {
        QString codigo = m_entrada;
        if ( codigo.size() >= 3 ) {
            int pos = codigo.indexOf ( ".-" );
            // no se si es el autoComplete o què però em criden a
            // aquesta senyal quan omplo el combo, amb el primer valor
            // i si no m'aturo ara, recalcularia el combo amb nomes
            // aquest valor encara que l'usuari nomes hagi escrit
            // un prefix que permeti mes candidats
            if ( pos < 0 ) {

                m_cursorcombo = mainCompany() ->load( "SELECT codigocompletoarticulo, nomarticulo FROM articulo WHERE codigocompletoarticulo LIKE ($1||'%')::text ORDER BY codigocompletoarticulo", 0, 25 );
                clear();
                while ( !m_cursorcombo->eof() ) {
                    addItem ( m_cursorcombo->value( "codigocompletoarticulo" )
                              + ".-" + m_cursorcombo->value( "nomarticulo" )
                              , QVariant ( m_cursorcombo->value( "codigocompletoarticulo" ) ) );
                    m_cursorcombo->nextRecord();
                } // end while
                delete m_cursorcombo;

            }
        }
    }
    //    showPopup();
    g_plugins->run ( "BfBuscarArticuloDelegate_textChanged_Post", this );
    setEditText ( cod );

    semaforo = false;
    
}

/// Retorna el codi d'article associat a la unica entrada del combo que
/// hagi estat trobada a la BD a partir de l'entrada de l'usuari. Això
/// permet que abans de donar un error per codi d'article incorrecte
/// se li assigni l'unic article trobat per l'entrada (incompleta?) de l'usuari.
/// Retorna NULL si no se n'ha trobat cap o se n'ha trobat mes d'un.
QString BfBuscarArticuloDelegate::unicaEleccion ( void )
{
    BL_FUNC_DEBUG
    int num = 0;
    QString elec = NULL;
    for ( int i = 0; ( num < 2 ) && ( i < count() ); i++ ) {
        BlDebug::blDebug ( Q_FUNC_INFO, 0, QString(_("Item: '%1', num: '%2', itemText: '%3', itemData: '%4'.")).arg(QString::number ( i )).arg(QString::number ( num )).arg(itemText ( i )).arg(itemData ( i ).toString()) );
        if ( itemData ( i ).isValid() ) {
	    BlDebug::blDebug ( Q_FUNC_INFO, 0, _("Este item es un articulo encontrado, no entrado por el usuario.") );
            elec = itemData ( i ).toString();
            num++;
        };
	BlDebug::blDebug ( Q_FUNC_INFO, 0, QString(_("Ultimo item: '%1', num: '%2', itemText: '%3', itemData: '%4'.")).arg(QString::number ( i )).arg(QString::number ( num )).arg(itemText ( i )).arg(itemData ( i ).toString()) );
    }
    return ( num == 1 ? elec : NULL );
}

/// Sii el combo nomes ha trobat un article a la BD per l'entrada de
/// l'usuari substitueix el text entrat per l'entrada del combo de l'article trobat.
QString BfBuscarArticuloDelegate::eligeUnico ( void )
{
    BL_FUNC_DEBUG

    QString elec = unicaEleccion();
    if ( !elec.isNull() ) {
        setEditText ( elec );
    } // end if

    
    return elec;
}

/// quan deixa d'editar el camp substituim el que ha posat
/// per l'article que volia trobar si nomes hi ha un article candidat
void BfBuscarArticuloDelegate::focusOutEvent ( QFocusEvent * event )
{
    BL_FUNC_DEBUG
    eligeUnico();
    BlComboBox::focusOutEvent ( event );
    
}

QString BfBuscarArticuloDelegate::entrada()
{
    return m_entrada;
}
# endif
