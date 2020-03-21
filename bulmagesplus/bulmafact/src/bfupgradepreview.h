/***************************************************************************
 *   Copyright (C) 2012 by Daniel Ripoll Osma.                             *
 *   info@danielripoll.es                                                  *
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

#ifndef BFUPGRADEPREVIEW_H
#define BFUPGRADEPREVIEW_H

#include <QObject>
#include <QStringList>
#include <QAction>

#include "blfunctions.h"
#include "blconfiguration.h"
#include "bfcompany.h"
#include "blmainwindow.h"
#include "blupgradepreview.h"


class BF_EXPORT BfUpgradePreview: public QObject
{
    Q_OBJECT

private:
    BlMainWindow *m_parent;
    BlUpgradePreview *uP;
    QList<QAction*> m_listActions;

public:
    explicit BfUpgradePreview(BlMainWindow *parent);
    ~BfUpgradePreview();

public slots:
    int loadMenuCompras();
    int loadMenuArticulos();
    void loadActions();
    void unloadActions();
    int packProveedorTriggered(bool triggered);
    int packTriggered(bool triggered);
    void extendido_triggered(bool triggered);
};

#endif
