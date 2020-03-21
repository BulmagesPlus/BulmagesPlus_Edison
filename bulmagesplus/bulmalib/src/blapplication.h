/***************************************************************************
 *   Copyright (C) 2006 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2018 by Fco. Javier Miró Carmona.                       *
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

#ifndef BLAPPLICATION_H
#define BLAPPLICATION_H

#include <QApplication>
#include "bldefs.h"
#include "blconfig.h"

QT_FORWARD_DECLARE_CLASS(QString);

#if CONFIG_DEBUG == true
QT_FORWARD_DECLARE_CLASS(QEvent);
QT_FORWARD_DECLARE_CLASS(QObject);
#endif


class BL_EXPORT BlApplication : public QApplication
{
    Q_OBJECT

public:
    BlApplication(int &argc, char **argv);
    ~BlApplication();

#if CONFIG_DEBUG == true
    virtual bool notify(QObject *object, QEvent *event);
#endif

    void emitDbTableChanged(const QString &dbTableName);

signals:
    void dbTableChanged(const QString &);
};


/// Declaramos la variable global g_theApp.
extern BL_EXPORT BlApplication *g_theApp;

#endif
