/***************************************************************************
 *   Copyright (C) 2010 by Tomeu Borrás Riera.                             *
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

#ifndef BLTOOLBUTTON_H
#define BLTOOLBUTTON_H

#include <QToolButton>
#include <QWidget>
#include "blmaincompanypointer.h"
#include "blmaincompany.h"


class BL_EXPORT BlToolButton : public QToolButton, public BlMainCompanyPointer
{
    Q_OBJECT

public:
    explicit BlToolButton(QWidget *parent = nullptr);
    explicit BlToolButton(BlMainCompany *company, QWidget *parent = nullptr);
    virtual ~BlToolButton();

public slots:
    virtual void buttonReleased();
};

#endif

