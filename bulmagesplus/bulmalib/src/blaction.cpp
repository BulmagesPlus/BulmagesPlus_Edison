/***************************************************************************
 *   Copyright (C) 2010 by Tomeu Borrás Riera.                             *
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

#include "blaction.h"
#include "blfunctions.h"
#include "blplugins.h"


///
/**
\param text
\param parent
**/
BlAction::BlAction(const QString &text, QWidget *parent) : QAction{text, parent}, BlMainCompanyPointer{}
{
    BL_FUNC_DEBUG

    connect(this, SIGNAL(triggered(bool)), this, SLOT(actionTriggered(bool)));
}


///
/**
\param company
\param text
\param parent
**/
BlAction::BlAction(BlMainCompany *company, const QString &text, QWidget *parent) : QAction{text, parent}, BlMainCompanyPointer{company}
{
    BL_FUNC_DEBUG

    connect(this, SIGNAL(triggered(bool)), this, SLOT(actionTriggered(bool)));
}


///
/**
**/
BlAction::~BlAction()
{
    BL_FUNC_DEBUG
}


///
/**
\param triggered
**/
void BlAction::actionTriggered(bool triggered)
{
    BL_FUNC_DEBUG

    g_plugins->run("BlAction_actionTriggered", this);
}

