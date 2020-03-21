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

#ifndef BLDOCKWIDGET_H
#define BLDOCKWIDGET_H

#include <QDockWidget>

#include "blworkspace.h"
#include "blfunctions.h"
#include "blmaincompanypointer.h"


/// Provides the dock window to list all windows in BulmaFact
/// \param m_listBox this variable is the listBox that contains all
/// titles of the diferent windows.
class BL_EXPORT BlDockWidget : public QDockWidget, public BlMainCompanyPointer
{
    Q_OBJECT

private:
    BlWorkspace *m_pWorkspace;
    Qt::DockWidgetArea m_area;
    QString m_name;
    BlMainCompany *m_company;
    void init(const QString & title, QWidget * parent = nullptr, const QString &name = "",  Qt::WindowFlags flags = 0);

public:
    void setWorkspace ( BlWorkspace *w );
    BlDockWidget(BlMainCompany *company, const QString & title, QWidget * parent = nullptr, const QString &name = "",  Qt::WindowFlags flags = 0);
    BlDockWidget(const QString & title, QWidget * parent = nullptr, const QString &name = "",  Qt::WindowFlags flags = 0);
    ~BlDockWidget();
    void saveConfig();
    void loadConfig();

public slots:
    void setVisibilityState ( bool );
    virtual void mi_dockLocationChanged ( Qt::DockWidgetArea area );

protected:
    void closeEvent ( QCloseEvent *event );

signals:
    void visibilityStateChanged ( bool );
};

#endif

