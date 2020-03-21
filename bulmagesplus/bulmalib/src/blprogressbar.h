/***************************************************************************
 *   Copyright (C) 2008 by Tomeu Borrás Riera.                             *
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

#ifndef BLPROGRESSBAR_H
#define BLPROGRESSBAR_H

#include <QProgressBar>
#include <QDialog>

#include "blfunctions.h"
#include "ui_blprogressbarbase.h"


class BL_EXPORT BlProgressBar : public QDialog, public Ui_BlProgressBarBase
{
public:
    BlProgressBar();
    virtual ~BlProgressBar();
    void setValue(int);
    int value() const;
    void setRange(int, int);
    void setText(const QString &);
    void show();
};

#endif
