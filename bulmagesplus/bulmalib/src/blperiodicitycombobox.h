/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borrás Riera.                             *
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

#ifndef BLPERIODICITYCOMBOBOX_H
#define BLPERIODICITYCOMBOBOX_H

#include "blcombobox.h"
#include "blfunctions.h"


class BL_EXPORT BlPeriodicityComboBox : public QComboBox
{
    Q_OBJECT

private:
    QString m_textos[10];
    QString m_valores[10];

public:
    explicit BlPeriodicityComboBox(QWidget *parent = nullptr);
    ~BlPeriodicityComboBox();
    virtual void setperiodo ( QString periodo );
    virtual void setFieldValue ( QString periodo );
    QString periodo() const;
    virtual QString fieldValue() const;

public slots:
    void s_activated ( int index );

signals:
    void valueChanged ( QString );
};

#endif

