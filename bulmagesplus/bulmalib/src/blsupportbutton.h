/***************************************************************************
 *   Copyright (C) 2013 by Daniel Ripoll Osma.                             *
 *   Copyright (C) 2013 by Fco. Javier Miró Carmona.                       *
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

#include <QWidget>
#include <QDockWidget>
#include <QToolButton>
#include <QByteArray>

#include "bldefs.h"


class BL_EXPORT BlSupportButton : public QToolButton
{
    Q_OBJECT


public:
    explicit BlSupportButton(QWidget *parent = nullptr);
//    BlUpgradeButton ( FacturaView * , QWidget *parent = NULL );
    ~BlSupportButton();
    void setButton( const char *theobjectName, QString statusTip, QString toolTip, QSize minSize, QSize maxSize, QIcon icon, QSize iconSize );

private:
    QWidget *m_parent;
    QDockWidget *m_dockWidget;
    
public slots:
    virtual void supportWindow();

};

