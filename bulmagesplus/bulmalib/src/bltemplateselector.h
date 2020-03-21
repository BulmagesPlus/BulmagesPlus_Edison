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

#ifndef BLTEMPLATESELECTOR_H
#define BLTEMPLATESELECTOR_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include "blwidget.h"
#include "bldefs.h"
#include "blmaincompanypointer.h"
#include "ui_bltemplateselectorbase.h"


class BL_EXPORT BlTemplateSelector : public QDialog, public Ui_BlTemplateSelectorBase
{
    Q_OBJECT
private:
    QString m_value{""};

public:
    explicit BlTemplateSelector(QWidget *parent = nullptr);
    ~BlTemplateSelector();
    QString value();
    void setValues(const QStringList &values);

public slots:
    void on_mui_select_clicked();
    void on_mui_cancel_clicked();
    void templateListItemSelectionChanged();
};

#endif
