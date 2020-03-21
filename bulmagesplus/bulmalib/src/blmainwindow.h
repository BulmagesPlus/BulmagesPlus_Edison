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

#ifndef BLMAINWINDOW_H
#define BLMAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QAction>

#include "bldefs.h"
#include "blaction.h"

class BL_EXPORT BlMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    BlMainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
    ~BlMainWindow();
    QMenu *newMenu(const QString &name,const QString &objname, const QString &before = "");
    QAction *newMenuQAction (const QString &name, const QString &objname, QWidget *before, const QString &helpthis = "", const bool &nulled = false, const char *iconname = "" );
    BlAction *newMenuBlAction (const QString &name, const QString &objname, QWidget *before, const QString &helpthis = "", const bool &nulled = false, const char *iconname = "" );
    bool loadAction (QAction *objname, QWidget *before);
    bool loadAction (BlAction *objname, QWidget *before);
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget, Qt::Orientation orientation);

//    void *m_update2;

};


extern BL_EXPORT BlMainWindow *g_main;

#endif

