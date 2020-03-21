/***************************************************************************
 *   Copyright (C) 2004 by J. M. Torres Rigo                               *
 *   joanmi@bulma.net                                                      *
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

#include "stdio.h"

#include <Qt>
#include <QObject>
#include <qnamespace.h>
#include <QFile>
#include <QTextStream>
#include <QDomDocument>

#include "blcompanydialog.h"
#include "blfunctions.h"
#include "blplugins.h"
#include "bfcompany.h"
#include "bfconfiguracionview.h"
#include "bfbulmafact.h"
#include "../../bulmagesplus_version.h"

#include "blsupportbutton.h"


/** No precisa de operaciones en su construccion.
*/
/**
\param bges
**/
BfCompany::BfCompany ( BfBulmaFact *bges ) : BlMainCompany()
{
    BL_FUNC_DEBUG
    m_bulmafact = bges;
    
}


/// El destructor de la clase BfCompany borra toda la memoria almacenada.
/**
**/
BfCompany::~BfCompany()
{
    BL_FUNC_DEBUG

    /// Guardamos la configuracion.
    guardaConf();

    /// Borramos las ventanas flotantes antes de eliminar esta clase ya que se produciria un segfault
    m_windowListDock->vaciarCompleto();
    
}


/** Crea todas las ventanas que aparecen creadas al inicio del programa.
    Todas estas ventanas son principalmente los listados mas utilizados a partir de los
    cuales se pueden crear nuevas fichas y manejar todo.
*/
/**
\param splash
\return
**/
void BfCompany::createMainWindows ( BlSplashScreen *splash )
{
    BL_FUNC_DEBUG

    /// Comprobamos que tengamos permisos para trabajar con 'Paises'.
    m_bulmafact->actionPaises->setEnabled ( false );
    if ( hasTablePrivilege ( "pais", "SELECT" ) ) {
        m_bulmafact->actionPaises->setEnabled ( true );
    } // end if


    /// Disparamos los plugins.
    int res = g_plugins->run ( "BfCompany_createMainWindows_Post", this );
    if ( res != 0 ) {
        return;
    } // end if

    cargaConf();

    /// pb = 100%
    m_progressbar->setValue ( 100 );

    /// Ponemos el titulo de la ventana
    m_bulmafact->statusBar() ->showMessage ( dbName(), 2000 );
    
    
}



/** Crea y Presenta la ventana de formas de pago.
*/
/// \TODO: Deberia dividirse en dos y deberia pasar por sistema de plugins.
/**
**/
void BfCompany::s_FPago()
{
    BL_FUNC_DEBUG
    /*
       FPagoView *f = new FPagoView ( this, NULL );
       m_pWorkspace->addSubWindow ( f );
       f->show();
    */
    
}


/** Crea y Presenta la ventana de trabajadores
*/
/// \TODO: Deberia dividirse en dos y deberia pasar por sistema de plugins.
/**
**/
void BfCompany::s_trabajadores()
{
    BL_FUNC_DEBUG  
}


/** Crea y Presenta la ventana de bancos
*/
/// \TODO: Deberia dividirse en dos y deberia pasar por sistema de plugins.
/**
**/
void BfCompany::s_bancos()
{
    BL_FUNC_DEBUG
}


/** Crea y Presenta la ventana de Series de Factura
*/
/// \TODO: Deberia dividirse en dos y deberia pasar por sistema de plugins.
/**
**/
void BfCompany::s_seriesFactura()
{
    BL_FUNC_DEBUG
   
}





/** Crea una instancia de ListAlmacenView y la presenta.
*/
/**
\return
**/
void BfCompany::s_almacenes()
{
    BL_FUNC_DEBUG
}


/** Crea una instancia de BfConfiguracionView y la presenta.
*/
/**
**/
void BfCompany::s_newBfConfiguracionView()
{
    BL_FUNC_DEBUG
    BfConfiguracionView *lser = new BfConfiguracionView ( this, 0 );
    m_pWorkspace->addSubWindow ( lser );
    lser->show();
    
}


/** Crea y muestra el listado de Tipos de IVA
*/
/**
**/
void BfCompany::s_TipoIVAView()
{
    BL_FUNC_DEBUG
    
}


/** Crea y muestra el listado de Tasas de IVA
*/
/**
**/
void BfCompany::s_TasaIVAView()
{
    BL_FUNC_DEBUG
   
}


/// Guarda la configuracion de programa para poder recuperar algunas cosas de presentacion.
/**
**/
void BfCompany::guardaConf()
{
    BL_FUNC_DEBUG
    QFile file ( g_confpr->value( CONF_DIR_USER ) + "bulmafact_" + dbName() + ".cfn" );
    /// Guardado del orden y de configuraciones varias.
    if ( file.open ( QIODevice::WriteOnly ) ) {

	 /// Saber si una ventana est&aacuta; maximizada
	 bool isMaximized = false;
	 if ( m_bulmafact->windowState() == Qt::WindowMaximized
	   || m_bulmafact->windowState() == Qt::WindowFullScreen )
	     isMaximized = true;

        QTextStream stream ( &file );
        stream << "<CONFIG>\n";
        stream << "\t<PRINCIPAL>\n";
        stream << "\t\t\t<X>" + QString::number ( m_bulmafact->geometry().x() ) + "</X>\n";
	  stream << "\t\t\t<Y>" + QString::number ( m_bulmafact->geometry().y() ) + "</Y>\n";
        stream << "\t\t\t<WIDTH>" + QString::number ( m_bulmafact->width() ) + "</WIDTH>\n";
        stream << "\t\t\t<HEIGHT>" + QString::number ( m_bulmafact->height() ) + "</HEIGHT>\n";
	  stream << "\t\t\t<INDEXADOR>" + ( QString ( m_bulmafact->actionIndexador->isChecked() ? "TRUE" : "FALSE" ) ) + "</INDEXADOR>\n";
        stream << "\t\t\t<TOOLBARSDOCKWIDGETS>" + QString ( m_bulmafact->saveState().toBase64() ) + "</TOOLBARSDOCKWIDGETS>\n";
	  stream << "\t\t\t<MAXIMIZED>" + QString ( isMaximized ? "TRUE" : "FALSE" ) + "</MAXIMIZED>\n";
        stream << "\t</PRINCIPAL>\n";

        for ( int i = 0; i < m_windowListDock->numVentanas(); i++ ) {
            QObject *obj = m_windowListDock->ventana ( i );
            QWidget *wid = ( QWidget * ) obj;
            stream << "\t<VENTANA>\n";
            stream << "\t\t<VNAME>" + obj->objectName() + "</VNAME>\n";
            stream << "\t\t<VX>" + QString::number ( wid->parentWidget() ->x() ) + "</VX>\n";
            stream << "\t\t<VY>" + QString::number ( wid->parentWidget() ->y() ) + "</VY>\n";
            stream << "\t\t<VWIDTH>" + QString::number ( wid->width() ) + "</VWIDTH>\n";
            stream << "\t\t<VHEIGHT>" + QString::number ( wid->height() ) + "</VHEIGHT>\n";
            stream << "\t\t<VVISIBLE>" + ( wid->isVisible() ? QString ( "TRUE" ) : QString ( "FALSE" ) ) + "</VVISIBLE>\n";
            stream << "\t\t<VMAXIMIZED>" + ( wid->isMaximized() ? QString ( "TRUE" ) : QString ( "FALSE" ) ) + "</VMAXIMIZED>\n";
            stream << "\t\t<VACTIVEWINDOW>" + ( m_bulmafact->workspace() ->activeWindow() == wid ? QString ( "TRUE" ) : QString ( "FALSE" ) ) + "</VACTIVEWINDOW>\n";
            stream << "\t</VENTANA>\n";
        } // end for

        stream << "</CONFIG>\n";
        file.close();
    } // end if
    
}


/// Guarda la configuracion de programa para poder recuperar algunas cosas de presentacion.
/**
\return
**/
void BfCompany::cargaConf()
{
    BL_FUNC_DEBUG
    QFile file ( g_confpr->value( CONF_DIR_USER ) + "bulmafact_" + dbName() + ".cfn" );
    QDomDocument doc ( "mydocument" );
    if ( !file.open ( QIODevice::ReadOnly ) )
        return;
    if ( !doc.setContent ( &file ) ) {
        file.close();
        return;
    }
    file.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = doc.documentElement();
    QDomElement principal = docElem.firstChildElement ( "PRINCIPAL" );
    /// Cogemos la coordenada X
    QString nx = principal.firstChildElement ( "X" ).toElement().text();

    /// Cogemos la coordenada Y
    QString ny = principal.firstChildElement ( "Y" ).toElement().text();

    /// Cogemos el ancho
    QString nwidth = principal.firstChildElement ( "WIDTH" ).toElement().text();

    /// Cogemos el alto
    QString nheight = principal.firstChildElement ( "HEIGHT" ).toElement().text();

    /// Si est&aacute; maximizada, ignoramos las otras dimensiones
    if (principal.firstChildElement ( "MAXIMIZED" ).toElement().text() == "TRUE")
	 m_bulmafact->setWindowState(Qt::WindowMaximized);
    else /// Establecemos la geometria de la ventana principal.
	 m_bulmafact->setGeometry ( nx.toInt(), ny.toInt(), nwidth.toInt(), nheight.toInt() );

    /// Cogemos el indexador
    QString indexador = principal.firstChildElement ( "INDEXADOR" ).toElement().text();
    if ( indexador == "TRUE" ) {
        s_indexadorCambiaEstado ( true );
        m_bulmafact->actionIndexador->setChecked ( true );
    } else {
        s_indexadorCambiaEstado ( false );
        m_bulmafact->actionIndexador->setChecked ( false );
    } // end if

    /// Cogemos el ancho del indexador
    m_bulmafact->restoreState ( QByteArray::fromBase64 ( QByteArray ( principal.firstChildElement ( "TOOLBARSDOCKWIDGETS" ).toElement().text().toLatin1() ) ) );

    /// Tratamos cada ventana
    QWidget *activewindow = NULL;
    QDomNodeList nodos = docElem.elementsByTagName ( "VENTANA" );
    for ( int i = 0; i < nodos.count(); i++ ) {
        QDomNode ventana = nodos.item ( i );
        QDomElement e1 = ventana.toElement(); /// try to convert the node to an element.
        if ( !e1.isNull() ) { /// the node was really an element.
            QString vname = e1.firstChildElement ( "VNAME" ).toElement().text();
            for ( int j = 0; j < m_windowListDock->numVentanas(); j++ ) {
                QObject *obj = m_windowListDock->ventana ( j );
                QWidget *wid = ( QWidget * ) obj;
                if ( obj->objectName() == vname ) {
                    QString vx = e1.firstChildElement ( "VX" ).toElement().text();
                    QString vy = e1.firstChildElement ( "VY" ).toElement().text();
                    QString vwidth = e1.firstChildElement ( "VWIDTH" ).toElement().text();
                    QString vheight = e1.firstChildElement ( "VHEIGHT" ).toElement().text();
                    QString vvisible = e1.firstChildElement ( "VVISIBLE" ).toElement().text();
                    QString vmaximized = e1.firstChildElement ( "VMAXIMIZED" ).toElement().text();
                    QString vactivewindow = e1.firstChildElement ( "VACTIVEWINDOW" ).toElement().text();
                    /// Establecemos la geometria de la ventana principal.
                    wid->resize ( vwidth.toInt(), vheight.toInt() );
                    wid->parentWidget() ->move ( vx.toInt(), vy.toInt() );
                    if ( vvisible == "TRUE" ) {
                        wid->showNormal();
                    } // end if
                    if ( vmaximized == "TRUE" ) {
                        wid->showMaximized();
                    }
                    if ( vactivewindow == "TRUE" ) {
                        activewindow = wid;
                    }
                } // end if
            } // end for
        } // end if
    } // end for
    /// Si hay una ventana activa se pone como activa.
    if ( activewindow )
	activewindow->activateWindow();
    
}
