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

#ifndef BLBANKEDIT_H
#define BLBANKEDIT_H

#include "blwidget.h"
#include "bldefs.h"
#include "ui_blbankeditbase.h"

QT_BEGIN_NAMESPACE
class QString;
class QWidget;
QT_END_NAMESPACE


class BL_EXPORT BlBankEdit : public BlWidget, public Ui_BlBankEditBase
{
  Q_OBJECT

  public:
    explicit BlBankEdit(QWidget *parent = nullptr);
    ~BlBankEdit();
    virtual void setText(QString text);
    virtual void setFieldValue(QString fieldValue) override;
    virtual QString text();
    virtual QString fieldValue();
    void checkControlDigit();
    void checkIBAN();

  public slots:
    virtual void bankAccountChanged(const QString &);
    virtual void bankAccountLostFocus();
    virtual void returnKeyPressed();
    virtual void selectAll();
    virtual void setFocus();

  signals:
    void valueChanged(QString);
    void returnPressed();
    void editingFinished();
};

#endif
