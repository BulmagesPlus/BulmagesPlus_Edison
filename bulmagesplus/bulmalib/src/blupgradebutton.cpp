/***************************************************************************
 *   Copyright (C) 2006 by Arturo Martín Lladó.                            *
 *   Copyright (C) 2007 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2012 by Daniel Ripoll Osma.                             *
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

#include "blupgradebutton.h"
#include "blupgradepreview.h"


QString g_zoneName{};
QString g_templateName{};

///
/**
\param parent
**/
BlUpgradeButton::BlUpgradeButton(QWidget *parent) : QToolButton{parent}
{
  BL_FUNC_DEBUG
}


///
/**
**/
BlUpgradeButton::~BlUpgradeButton()
{
  BL_FUNC_DEBUG
}


///
/**
**/
void BlUpgradeButton::setButton(const char * theobjectName, QString statusTip, QString toolTip, QSize minSize, QSize maxSize, QIcon icon, QSize iconSize, const char * htdoc, const char * htplace)
{
  BL_FUNC_DEBUG

  connect(this, SIGNAL(clicked()), this, SLOT(click()));
  setObjectName(QString::fromUtf8(theobjectName));
  setStatusTip(statusTip);
  setToolTip(toolTip);
  setMinimumSize(minSize);
  setMaximumSize(maxSize);
  setIcon(icon);
  setIconSize(iconSize);

  g_templateName = QString::fromLatin1(htdoc);
  g_zoneName = QString::fromLatin1(htplace);
}


///
/**
**/
void BlUpgradeButton::click()
{
  BL_FUNC_DEBUG

  BlUpgradePreview *m_upgradePreview{new BlUpgradePreview{}};
  m_upgradePreview->showUpgradePreviewInfo(g_templateName, g_zoneName);
}

