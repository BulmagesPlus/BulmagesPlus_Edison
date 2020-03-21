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

#include <QObject>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <map>

#include "bldockwidget.h"
#include "blconfiguration.h"
#include "blmainwindow.h"



///
/**
\param w
**/
void BlDockWidget::setWorkspace ( BlWorkspace *w )
{
    BL_FUNC_DEBUG
    m_pWorkspace = w;
    
}


BlDockWidget::BlDockWidget ( BlMainCompany *company, const QString & title, QWidget * parent, const QString &name , Qt::WindowFlags flags ) : QDockWidget ( title, parent, flags )
{
    BL_FUNC_DEBUG
    m_company = company;
    init ( title, parent, name, flags );
}


///
/**
\param a
**/
BlDockWidget::BlDockWidget ( const QString & title, QWidget * parent, const QString &name , Qt::WindowFlags flags ) : QDockWidget ( title, parent, flags )
{
    BL_FUNC_DEBUG
    m_company = 0;
    
    init ( title, parent, name, flags );
    
}


void BlDockWidget::init ( const QString & title, QWidget * parent, const QString &name , Qt::WindowFlags flags )
{
    BL_FUNC_DEBUG
    setFocusPolicy ( Qt::StrongFocus );
    m_name = name;
    setObjectName ( name );
    connect ( this, SIGNAL ( dockLocationChanged ( Qt::DockWidgetArea ) ), this, SLOT ( mi_dockLocationChanged ( Qt::DockWidgetArea ) ) );
    
}


///
/**
**/
BlDockWidget::~BlDockWidget()
{
    BL_FUNC_DEBUG
    saveConfig();
    
}



///
/**
\param visible
**/
void BlDockWidget::setVisibilityState ( bool visible )
{
    BL_FUNC_DEBUG
    if ( visible == true ) {
        this->show();
    } else {
        this->hide();
    } // end if
    
}


///
/**
**/
void BlDockWidget::closeEvent ( QCloseEvent * )
{
    BL_FUNC_DEBUG
    emit ( visibilityStateChanged ( false ) );
    
}

void BlDockWidget::mi_dockLocationChanged ( Qt::DockWidgetArea area )
{
    BL_FUNC_DEBUG

    m_area = area;
}

void BlDockWidget::loadConfig()
{
    BL_FUNC_DEBUG

    /// Si existe el archivo de configuración lo cargamos y configuramos el aspecto del widget.
    QFile file;

    if (m_company)
    {
      file.setFileName ( g_confpr->value( CONF_DIR_USER ) + m_name + "_" + m_company->dbName() + ".cfn" );
    } else {
      file.setFileName ( g_confpr->value( CONF_DIR_USER ) + m_name + ".cfn" );
    } // end if

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);

        int area = stream.readLine().toInt();
        int width = stream.readLine().toInt();
        int height = stream.readLine().toInt();
        int order = stream.readLine().toInt();

        setProperty("order", QVariant(order));

        g_main->addDockWidget((Qt::DockWidgetArea) area, this);

        resize(width, height);

        file.close();
    } // end if
}


void BlDockWidget::saveConfig()
{
    BL_FUNC_DEBUG

    /// Vamos a probar con un docwindow.
    int lwidth = width();
    int lheight = height();

    QString aux = "";

    QFile file;

    if (m_company)
    {
      file.setFileName ( g_confpr->value( CONF_DIR_USER ) + m_name + "_" + m_company->dbName() + ".cfn" );
    } else {
      file.setFileName ( g_confpr->value( CONF_DIR_USER ) + m_name + ".cfn" );
    } // end if

    /// Guardado del orden y de configuraciones varias.
    if ( file.open ( QIODevice::WriteOnly ) ) {
        QTextStream stream ( &file );
        stream << m_area << "\n";
        stream << lwidth << "\n";
        stream << lheight << "\n";
        file.close();
    } // end if
}



