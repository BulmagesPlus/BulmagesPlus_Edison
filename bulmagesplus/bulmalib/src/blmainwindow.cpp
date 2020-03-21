/***************************************************************************
 *   Copyright (C) 2009 by Tomeu Borrás Riera.                             *
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
#include <QMenuBar>

#include "blmainwindow.h"
//#include "blupdate.h"


/// Definimos aqui la variable global g_main para que sea accesible desde esta libreria.
BlMainWindow *g_main = NULL;



BlMainWindow::BlMainWindow ( QWidget * parent, Qt::WindowFlags flags )
{
//    BlUpdate *update2 = new BlUpdate(this);
//    update2->setProgramRunningName("factu");
//    m_update2 = update2;

}


BlMainWindow::~BlMainWindow()
{
}


QMenu *BlMainWindow::newMenu ( const QString &name, const QString &objname, const QString &before )
{
    /// Miramos si existe un menu Ventas
    QMenu *pPluginMenu = NULL;
    pPluginMenu = menuBar() ->findChild<QMenu *> ( objname );
    /// Creamos el men&uacute;.
    if ( !pPluginMenu ) {
        QMenu *pPluginMaestro = NULL;
        pPluginMaestro = menuBar() ->findChild<QMenu *> ( before );
        pPluginMenu = new QMenu ( name, menuBar() );
        pPluginMenu->setObjectName (  objname );
        if (pPluginMaestro) {
            menuBar()->insertMenu ( pPluginMaestro->menuAction(), pPluginMenu );
        } else {
            menuBar()->addMenu ( pPluginMenu );
        } // end if
    } // end if
    return pPluginMenu;
}

QAction *BlMainWindow::newMenuQAction (const QString &name, const QString &objname, QWidget *before, const QString &helpthis, const bool &nulled, const char *iconname )
{
    BL_FUNC_DEBUG
    
        QAction *accionA = NULL;
        accionA = before->findChild<QAction *> (objname);
        if  ( ! before->actions().contains( accionA ) )
        {
            accionA = new QAction ( name , before );
            if ( (iconname != "") && ( iconname != NULL  ) )
            {
                accionA->setIcon ( QIcon ( QString::fromUtf8 ( iconname ) ) );
            } // end if
            accionA->setStatusTip ( helpthis );
            accionA->setWhatsThis ( helpthis );
            accionA->setObjectName( objname );
        }
        else
        {
          if (nulled) {
              accionA = NULL;
          }
        }
        return accionA;
 //   } // end if
}

BlAction *BlMainWindow::newMenuBlAction (const QString &name, const QString &objname, QWidget *before, const QString &helpthis, const bool &nulled, const char *iconname )
{
    BL_FUNC_DEBUG

        BlAction *accionA = NULL;
        accionA = before->findChild<BlAction *> (objname);
        if  ( ! before->actions().contains( accionA ) )
        {
            accionA = new BlAction ( name , before );
            if ( (iconname != "") && ( iconname != NULL  ) )
            {
                accionA->setIcon ( QIcon ( QString::fromUtf8 ( iconname ) ) );
            } // end if
            accionA->setStatusTip ( helpthis );
            accionA->setWhatsThis ( helpthis );
            accionA->setObjectName( objname );
        }
        else
        {
          if (nulled) {
              accionA = NULL;
          }
        }
        return accionA;
 //   } // end if
}

bool BlMainWindow::loadAction (QAction *objname, QWidget *before )
{
    BL_FUNC_DEBUG
    //QAction actionName = objname;
    bool result = false;
    if (objname)
    {
      //before->addAction(actionName);
      before->addAction(objname);
      result = true;
    }
    else
    {
      result = false;
    }
    return result;
}

bool BlMainWindow::loadAction (BlAction *objname, QWidget *before )
{
    BL_FUNC_DEBUG
    //QAction actionName = objname;
    bool result = false;
    if (objname)
    {
      //before->addAction(actionName);
      before->addAction(objname);
      result = true;
    }
    else
    {
      result = false;
    }
    return result;
}


void BlMainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget)
{
    QMainWindow::addDockWidget(area, dockwidget);
}


void BlMainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget, Qt::Orientation orientation)
{
    QMainWindow::addDockWidget(area, dockwidget, orientation);
}

