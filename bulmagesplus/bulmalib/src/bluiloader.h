/***************************************************************************
 *   Copyright (C) 2011 by Tomeu Borrás Riera.                             *
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

#ifndef BLUILOADER_H
#define BLUILOADER_H

#include <QWidget>
#include <QUiLoader>
#include "blform.h"
#include "blformlist.h"


class BL_EXPORT BlUiLoader : public QUiLoader
{
    Q_OBJECT

private:
    BlMainCompany *m_company;

public:
    explicit BlUiLoader(BlMainCompany *company, QObject *parent = nullptr);
    virtual ~BlUiLoader();
    virtual QWidget *createWidget(const QString &className, QWidget *parent = nullptr, const QString &name = QString());
};

#endif

