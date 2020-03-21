/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
 *   Copyright (C) 2006 by Fco. Javier M. C. (Porting to QT4)              *
 *   fcojavmc@todo-redes.com                                               *
 *   http://www.iglues.org                                                 *
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

#ifndef LISTCONFIGURACIONVIEW_H
#define LISTCONFIGURACIONVIEW_H

#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

#include "bfform.h"
#include "blpostgresqlclient.h"
#include "bfsubform.h"


/// Muestra y administra las l&iacute;neas de detalle del listado de configuraci&oacute;nes.
/** */
class BF_EXPORT BfConfiguracionSubForm : public BfSubForm
{
    Q_OBJECT

public:
    explicit BfConfiguracionSubForm(QWidget *parent = 0);
    ~BfConfiguracionSubForm() {}
public slots:
    virtual void load() {
        BL_FUNC_DEBUG
        BlSubForm::load ( "SELECT *, nombre AS nombreorig FROM configuracion" );
    };
};


#include "ui_bfconfiguracionbase.h"


/// Muestra y administra el listado de configuraci&oacute;nes.
/** */
class BF_EXPORT BfConfiguracionView : public BfForm, public Ui_BfConfiguracionBase
{
    Q_OBJECT

public:
    explicit BfConfiguracionView(BfCompany *company, QWidget *parent = 0);
    ~BfConfiguracionView();

    int saveConf();

public slots:
    virtual void on_mui_aceptar_clicked();
    virtual void on_mui_cambiarLogoEmpresa_clicked();
};

#endif
