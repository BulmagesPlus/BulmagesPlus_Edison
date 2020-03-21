/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
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

#ifndef LISTLINFACTURAVIEW_H
#define LISTLINFACTURAVIEW_H

#include <QEvent>

#include "bfcompany.h"
#include "bfsubform.h"
#include "blfixed.h"
#include "pdefs_pluginbf_clientefactura.h"


/// Muestra y administra las l&iacute;neas de detalle de una factura a un cliente.
/** */
class PLUGINBF_CLIENTEFACTURA_EXPORT ListLinFacturaView : public BfSubForm
{
    Q_OBJECT

public:
    QString mdb_idfactura;
    ListLinFacturaView ( QWidget *parent = 0 );
    ~ListLinFacturaView() {};
    QStringList listRows ( QString column, bool ignoreDups = false );

public slots:
    virtual void load ( QString idfactura );
};

#endif

