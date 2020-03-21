/***************************************************************************
 *   Copyright (C) 2009 by Fco. Javier Miró Carmona.                       *
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

#ifdef USE_GETTEXT
	#include <libintl.h>
#endif

#include <QLocale>

#include "bli18n.h"
#include "blfunctions.h"



QString blI18n ( const char *cadena, int )
{
    BL_FUNC_DEBUG
#ifdef USE_GETTEXT
    /// Depuracion:
    QString salida = QString("blI18n (") + QString(blTextDomain(NULL)) + QString(" - ") + QString(gettext(cadena)) + QString(")");
    BlDebug::blDebug(salida, 0);

    return QString ( gettext ( cadena ) );
#else
    return QString ( cadena );
#endif
}


QString blI18n ( const char *domain, const char *cadena )
{
#ifdef USE_GETTEXT
    /// Depuracion:
    QString salida = QString("blI18n (") + QString(domain) + QString(" - ") + QString(dgettext(domain, cadena)) + QString(")");
    BlDebug::blDebug(salida, 0);

    return QString ( dgettext ( domain, cadena ) );
#else
    return QString ( cadena );
#endif
}



char* blBindTextDomain ( const char *t1, const char *t2 )
{
#ifdef USE_GETTEXT
    bind_textdomain_codeset(t1, "UTF-8");
    return bindtextdomain (t1, t2);
#else
    return (char*) "";
#endif
}


char* blTextDomain ( const char *t1 )
{
#ifdef USE_GETTEXT
    bind_textdomain_codeset(t1, "UTF-8");
    return textdomain(t1);
#else
    return (char*) "";
#endif
}

