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

#ifndef BLI18N_H
#define BLI18N_H

#include <locale.h>

#include <QString>

#include "bldefs.h"


QString BL_EXPORT blI18n(const char *, int = 0);

/// Gettext con dominio
QString BL_EXPORT blI18n(const char *, const char *);


char* BL_EXPORT blBindTextDomain(const char *, const char *);
char* BL_EXPORT blTextDomain(const char *);


#endif

