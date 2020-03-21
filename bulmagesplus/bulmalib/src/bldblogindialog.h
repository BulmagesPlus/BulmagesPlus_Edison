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

#ifndef BLDBLOGINDIALOG_H
#define BLDBLOGINDIALOG_H

#include <QDialog>

#include "blfunctions.h"
#include "ui_bldblogindialogbase.h"
#include "blpostgresqlclient.h"


/// Presenta un di&aacute;logo que pregunta el usuario y la contrase&ntilde;a
/// para acceder a la base de datos.
class BL_EXPORT BlDbLoginDialog : public QDialog, public Ui_BlDbLoginDialogBase
{
    Q_OBJECT

private:
    bool m_authOK;

private:
    BlPostgreSqlClient *metabase;

public:
    BlDbLoginDialog ( QWidget *parent = nullptr, const char *name = 0 );
    ~BlDbLoginDialog();
    bool authOK();

public slots:
    virtual void validate();
};

#endif

