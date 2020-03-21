/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2006 by Fco. Javier Miró Carmona.                       *
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

#ifndef BLSPLASHSCREEN_H
#define BLSPLASHSCREEN_H

#include <QDialog>
#include <QPixmap>
#include <QLabel>
#include <QTimer>
#include <QTextBrowser>
#include <QPalette>
#include <QProgressBar>
#include <QDesktopWidget>
#include <QFile>
#include <QSplashScreen>

#include <stdlib.h>
#include <stdio.h>

#include "blfunctions.h"


/// Muestra y administra la ventana de bienvenida al programa.
/** */
class BL_EXPORT BlSplashScreen : public QSplashScreen
{
    Q_OBJECT

private:
    QProgressBar *m_barra;
    QLabel *m_label, *m_label1, *l0;
    QPixmap *m_image0;
    QBrush  *m_brush;

public:
    BlSplashScreen ( QString appSplash, QString appName, QString appVersion );
    ~BlSplashScreen();
    void setMessage ( QString );
    void setProgressBar ( int );
};

#endif

