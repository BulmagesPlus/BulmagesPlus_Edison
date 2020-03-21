/***************************************************************************
 *   Copyright (C) 2013 by Daniel Ripoll Osma.                             *
 *   Copyright (C) 2013 by Fco. Javier Miró Carmona.                       *
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

#ifndef BLSUPPORTEMAIL_H
#define BLSUPPORTEMAIL_H

#include <QDockWidget>
#include <QtNetwork/QNetworkReply>

#include "blfunctions.h"
#include "blconfiguration.h"
#include "blmainwindow.h"

#include "ui_blsupportemail.h"
#include "bldefs.h"


class BL_EXPORT BlSupportEmail : public QDockWidget, public Ui_BlSupportEmail
{
    Q_OBJECT

public:
    explicit BlSupportEmail(BlMainWindow *parent = nullptr);
    ~BlSupportEmail();

private:
    BlMainWindow *m_parent;
    int m_dockHeight;
    int m_dockWidth;
    

public slots:
    void on_mui_sendEmail_clicked();
    void replyFinished(QNetworkReply *reply);
    void resizeDockWidget();
};

#endif
