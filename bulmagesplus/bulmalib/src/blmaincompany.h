/***************************************************************************
 *   Copyright (C) 2006 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2010 by Fco. Javier Miró Carmona.                       *
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

#ifndef BLMAINCOMPANY_H
#define BLMAINCOMPANY_H

#include <QObject>
#include <QProgressBar>
#include <QStringList>

#include "blfunctions.h"
#include "blpostgresqlclient.h"
#include "blwindowlistdock.h"
#include "blworkspace.h"


class BL_EXPORT BlMainCompany : public QObject, public BlPostgreSqlClient
{
    Q_OBJECT

protected:
    /// Puntero al dock que lista las ventanas. Puesto que esta clase hace el paso de
    /// mensajes tiene el control de la lista de ventanas.
    BlWindowListDock *m_windowListDock;

public:
    /// Puntero al BlWorkspace de la aplicacion. Ya que esta clase crea todas las ventanas,
    /// es comodo tener este puntero inicializado en esta clase.
    BlWorkspace *m_pWorkspace;
    /// El indicador de progreso. Para que determinados procesos puedan indicar su evolucion
    /// de forma general en la ventana principal.
    QProgressBar *m_progressbar;

public:
    BlMainCompany();
    virtual ~BlMainCompany();
    virtual void init ( QStringList tipo, QString bd = "" );
    QString searchCompany ( QStringList tipo );
    void setListVentanas ( BlWindowListDock *doc );
    BlWindowListDock *listVentanas();
    BlWorkspace *pWorkspace();
    void setWorkspace ( BlWorkspace *qw );
    void setProgressBar ( QProgressBar *pb );
    int insertWindow ( QString nom, QObject *obj, bool compdup = true, QString titulo = "" );
    bool showWindow (QString objectName);
    int selectWindow ( QString nom, QMdiSubWindow *obj );
    void removeWindow ( QObject *nom );
    void s_indexadorCambiaEstado ( bool );
    void muestraPaises();
    void dbPatchVersionCheck(QString, QString);
    void dbVersionCheck(QString, QString);

public slots:
    int deselectWindow();

};

#endif

