/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borras Riera                              *
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

#ifndef BFCOMPANY_H
#define BFCOMPANY_H

#include <QObject>
#include <QLibraryInfo>

#include "blpostgresqlclient.h"
#include "blwindowlistdock.h"
#include "blworkspace.h"
#include "blmaincompany.h"
#include "blsplashscreen.h"


class BfBulmaFact;


/// Clase company (empresa).
/** Clase principal del programa donde se almacenan y gestionan
    todos los datos de la empresa con la que se est&aacute; trabajando.
    Deriva de BlPostgreSqlClient ya que tiene el tratamiento de la base de datos. */
class BF_EXPORT BfCompany : public BlMainCompany
{
private:
    /// Puntero al mainWindow
    BfBulmaFact *m_bulmafact;

public:
    explicit BfCompany(BfBulmaFact *bges);
    virtual ~BfCompany();
    void createMainWindows(BlSplashScreen *);
    void s_FPago();
    void s_Familias();
    void s_trabajadores();
    void s_bancos();
    void s_seriesFactura();
    void s_newBfConfiguracionView();
    void s_almacenes();
    void viewPagosList();
    void s_TipoIVAView();
    void s_TasaIVAView();
    void guardaConf();
    void cargaConf();
    void *newPagoView();
};

#endif
