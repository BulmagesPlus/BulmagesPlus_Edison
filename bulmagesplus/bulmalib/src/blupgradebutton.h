/***************************************************************************
 *   Copyright (C) 2006 by Arturo Martín Lladó.                            *
 *   Copyright (C) 2007 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2012 by Daniel Ripoll Osma.                             *
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

#ifndef BLUPGRADEBUTTON_H
#define BLUPGRADEBUTTON_H

#include <QWidget>
#include <QToolButton>
#include "bldefs.h"


class BL_EXPORT BlUpgradeButton : public QToolButton
{
  Q_OBJECT

  public:
    explicit BlUpgradeButton(QWidget *parent = nullptr);
    ~BlUpgradeButton();
    void setButton(const char * theobjectName, QString statusTip, QString toolTip, QSize minSize, QSize maxSize, QIcon icon, QSize iconSize, const char *htdoc = "index", const char *htplace = "upgrade_preview");

  public slots:
    virtual void click();
};

#endif
