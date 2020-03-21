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

#ifndef BLDATELINEEDIT_H
#define BLDATELINEEDIT_H

#include <QLineEdit>
#include "bldefs.h"

QT_FORWARD_DECLARE_CLASS(QString);
QT_FORWARD_DECLARE_CLASS(QWidget);
QT_FORWARD_DECLARE_CLASS(QEvent);
QT_FORWARD_DECLARE_CLASS(QObject);


class BL_EXPORT BlDateLineEdit : public QLineEdit
{
  Q_OBJECT

  public:
    explicit BlDateLineEdit(QWidget * parent = nullptr);
    ~BlDateLineEdit();

    void setText(QString text);
    virtual void setFieldValue(QString value);
    QString text() const;
    virtual QString fieldValue() const;
    void selectAll();
    virtual bool eventFilter(QObject * object, QEvent * event);

  public slots:
    void on_mui_editingFinished();
};

#endif
