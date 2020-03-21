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

#include <QString>
#include <QLibrary>
#include <QStringList>

#include "blplugins.h"
#include "blconfiguration.h"
#include "local_blI18n.h"

typedef int ( *MyPrototype ) ( void * );
MyPrototype myFunction;

typedef int ( *MyPrototype1 ) ( void *, void ** );
MyPrototype1 myFunction1;

BlPlugins BL_EXPORT *g_plugins = NULL;
void BL_EXPORT *g_plugParams = NULL;


void initPlugins()
{
    g_plugins = new BlPlugins();
}

///
/**
**/
BlPlugins::BlPlugins()
{
    BL_FUNC_DEBUG
    
}


///
/**
**/
BlPlugins::~BlPlugins()
{
    BL_FUNC_DEBUG
    /// Liberamos la memoria.
    while (!m_plugins.isEmpty())
       delete m_plugins.takeFirst();
}


///
/**
\param libs
**/
void BlPlugins::loadLibs ( const QString &libs )
{
    BL_FUNC_DEBUG

    bool cargado = false;
    /// Hacemos la carga de los plugins.
    QString cad = libs;

    if ( cad == "" )   return;

    QStringList dirs = g_confpr->value( CONF_DIR_PLUGINS ).split ( ";" );

    QStringList plugins = cad.split ( ";" );
    QString libErrorString;
    for ( QStringList::Iterator it = plugins.begin(); it != plugins.end(); ++it ) {
        cargado = false;
        for ( QStringList::Iterator ot = dirs.begin(); ot != dirs.end(); ++ot ) {
            QString file = *ot + *it;
            QLibrary *lib = new QLibrary ( file );
            lib->load();
            libErrorString += lib->errorString();
            if ( lib->isLoaded() ) {
                cargado = true;
                m_plugins.append ( lib );
	    } else {
	        delete lib;
            } // end if
        } // end for
        if ( ! cargado ) {
            blMsgInfo ( _ ("No se ha podido cargar la libreria: " ) + *it + "\n" + libErrorString );
	} else {
	    libErrorString = "";
        } // end if
    } // end for
    
}

///
/**
\param func
\param clase
\return
**/
int BlPlugins::run ( const char *func, void *clase )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlPlugins::run", 0, func );
    int a = 0;
    QList<MyPrototype> funcAddressList;
    
    /// Recorre todos los plugins buscando la funcion 'func'.
    for ( int i = 0; i < m_plugins.size(); ++i ) {
        myFunction = ( MyPrototype ) m_plugins.at ( i ) ->resolve ( func );
	
	/// Anyade direccion a la lista si no esta en la lista.
	/// Esto evita que se pueda ejecutar varias veces la misma funcion
	/// desde 2 plugins diferentes. Como se utilizan para plugins librerias
	/// que pueden depender de otras librerias si no hacemos esto entonces
	/// la funcion 'func' puede aparecer en una dependencia del plugin lo
	/// que la haria ejecutar varias veces.
	bool encontrado = false;

	for (int j = 0; j < funcAddressList.size(); ++j) {
	    if (funcAddressList.at(j) == myFunction) {
		encontrado = true;
		break;
	    } // end if
	} // end for
	
	if (!encontrado) {
	  funcAddressList.push_back(myFunction);
	} // end if
	
    } // end for
	
    /// Ejecuta todas las funciones 'func' encontradas en los plugins.
    for (int i = 0; i < funcAddressList.size(); ++i) {
	myFunction = funcAddressList.at(i);

	if ( myFunction && a == 0 ) {
            a = myFunction ( clase );
        } // end if
    
    } // end for

    
    return a;
}


///
/**
\return
**/
int BlPlugins::run ( const char *func, void *clase, void **ret )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlPlugins::run", 0, func );
    int a = 0;
    QList<MyPrototype1> funcAddressList;
    
    /// Recorre todos los plugins buscando la funcion 'func'.
    for ( int i = 0; i < m_plugins.size(); ++i ) {
        myFunction1 = ( MyPrototype1 ) m_plugins.at ( i ) ->resolve ( func );
	
	/// Anyade direccion a la lista si no esta en la lista.
	/// Esto evita que se pueda ejecutar varias veces la misma funcion
	/// desde 2 plugins diferentes. Como se utilizan para plugins librerias
	/// que pueden depender de otras librerias si no hacemos esto entonces
	/// la funcion 'func' puede aparecer en una dependencia del plugin lo
	/// que la haria ejecutar varias veces.
	bool encontrado = false;

	for (int j = 0; j < funcAddressList.size(); ++j) {
	    if (funcAddressList.at(j) == myFunction1) {
		encontrado = true;
		break;
	    } // end if
	} // end for
	
	if (!encontrado) {
	  funcAddressList.push_back(myFunction1);
	} // end if
	
    } // end for
	
    /// Ejecuta todas las funciones 'func' encontradas en los plugins.
    for (int i = 0; i < funcAddressList.size(); ++i) {
	myFunction1 = funcAddressList.at(i);

	if ( myFunction1 && a == 0 ) {
            a = myFunction1 ( clase, ret );
        } // end if
    
    } // end for

    
    return a;
}


///
/**
\return
**/
QList<QLibrary *> BlPlugins::pluginsLoaded()
{
    BL_FUNC_DEBUG
    return m_plugins;
}

