/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borrás Riera.                             *
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

#ifndef BLPLUGINS_H
#define BLPLUGINS_H

#include <QList>
#include <QLibrary>

#include "blfunctions.h"


/** Controla el sistema de carga y disparo segun metodo de los plugins
 */
class BL_EXPORT BlPlugins
{
private:
    QList<QLibrary *> m_plugins;

public:
    BlPlugins();
    ~BlPlugins();
    void loadLibs ( const QString &libs );
    int run ( const char *func, void *clase );
    int run ( const char *func, void *clase, void **ret );
    QList<QLibrary *> pluginsLoaded();
};

//extern BL_EXPORT BlPlugins *g_plugins;
//extern BL_EXPORT void *g_plugParams;

extern BlPlugins BL_EXPORT *g_plugins;
extern void BL_EXPORT *g_plugParams;
void BL_EXPORT initPlugins();

#endif

