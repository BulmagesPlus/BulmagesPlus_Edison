/***************************************************************************
 *   Copyright (C) 2019 by Fco. Javier Miró Carmona.                       *
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

#ifndef BLLABEL_H
#define BLLABEL_H

#include <QLabel>
#include "bldefs.h"

QT_FORWARD_DECLARE_CLASS(QString);
QT_FORWARD_DECLARE_CLASS(QWidget);


class BL_EXPORT BlLabel : public QLabel
{
  Q_OBJECT

  public:
    explicit BlLabel(QWidget * parent = nullptr, Qt::WindowFlags f = 0);
    explicit BlLabel(const QString & text, QWidget * parent = nullptr, Qt::WindowFlags f = 0);
    ~BlLabel();

  signals:
    void clicked();

  protected:
    virtual void mousePressEvent(QMouseEvent *) override;
};

#endif

