/***************************************************************************
 *   Copyright (C) 2007 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2010 by Rubén Cabezas Sigüenza. (QScript support).      *
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

#ifndef BLFORMCONFIG_H
#define BLFORMCONFIG_H

#include "blfunctions.h"
#include "blwidget.h"
#include "ui_blformconfigbase.h"
#include "blform.h"


class BL_EXPORT BlFormConfig : public BlWidget, public Ui_BlFormConfigBase
{
    Q_OBJECT

private:
    BlForm *m_BlForm;

public:
    BlFormConfig ( BlMainCompany *, BlForm *parent = nullptr, Qt::WindowFlags f = 0 );
    ~BlFormConfig();
public slots:
    virtual void on_editarScript_released();
};

#endif

