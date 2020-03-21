/***************************************************************************
 *   Copyright (C) 2006 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2010 by Fco. Javier Miró Carmona.                       *
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

#ifndef BLWIDGET_H
#define BLWIDGET_H

#include <QWidget>
#include "blmaincompanypointer.h"

QT_FORWARD_DECLARE_CLASS(QCloseEvent);
QT_FORWARD_DECLARE_CLASS(QPainter);
QT_FORWARD_DECLARE_CLASS(QEvent);

class BlMainCompany;


class BL_EXPORT BlWidget : public QWidget, public BlMainCompanyPointer
{
  Q_OBJECT

  protected:
    virtual void paintEvent(QPaintEvent * event);
    bool event(QEvent * event);

  public:
    BlWidget(QWidget * parent = nullptr, Qt::WindowFlags flags = Qt::Widget);
    explicit BlWidget(BlMainCompany * company, QWidget * parent = nullptr, Qt::WindowFlags flags = Qt::Widget);
    virtual ~BlWidget();

  signals:
    void showed(QObject * object = nullptr);
    void hided(QObject * object = nullptr);
};


#endif
