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

#include <QMenu>
#include "blfunctions.h"
#include "bltreewidget.h"
#include "blplugins.h"
#include "local_blI18n.h"


BlTreeWidget::BlTreeWidget ( QWidget * parent ) : QTreeWidget(parent) {

    /// Disparamos los plugins.
    g_plugins->run ( "BlTreeWidget_BlTreeWidget_Post", this );
}


BlTreeWidget::~BlTreeWidget () {
}


///
/**
**/
void BlTreeWidget::createMenu ( QMenu * )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubForm:: CreaMenu", 0, "funcion para ser sobreescrita" );
}


///
/**
**/
void BlTreeWidget::execMenuAction ( QAction * )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubForm:: execMenuAction", 0, "funcion para ser sobreescrita" );
}


///
/**
\return
**/
void BlTreeWidget::contextMenuEvent ( QContextMenuEvent * )
{
    BL_FUNC_DEBUG

    int column = currentColumn();
    
    if ( column < 0 ) return;

    QMenu *menu = new QMenu ( this );

    /// Lanzamos el evento para que pueda ser capturado por terceros.
    emit pintaMenu ( menu );

    /// Lanzamos la propagaci&oacute;n del men&uacute; a trav&eacute;s de las clases derivadas.
    createMenu ( menu );

    QAction *adjustColumnSize = menu->addAction ( _ ( "Ajustar columna" ) );
    QAction *adjustAllColumnsSize = menu->addAction ( _ ( "Ajustar columnas" ) );
    QAction *menuOption = menu->exec ( QCursor::pos() );

    /// Si no hay ninguna opci&oacute;n pulsada se sale sin hacer nada.
    if ( !menuOption ) return;

    if ( menuOption == adjustAllColumnsSize ) {

	for (int i = 0; i < columnCount(); i++) {
	    resizeColumnToContents (i);
	} // end for
	
    } else if ( menuOption == adjustColumnSize )  {
        resizeColumnToContents ( column );
    } // end if

    emit trataMenu ( menuOption );

    /// Activamos las herederas.
    execMenuAction ( menuOption );

    delete menu;
}

