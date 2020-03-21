/***************************************************************************
 *   Copyright (C) 2009 by Tomeu Borrás Riera.                             *
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


#ifndef BL_DEFS
#define BL_DEFS

/// Creo las macros de exportacion/importacion para windows
/// En windows una libreria dinamica (DLL) requiere una
/// exportacion de las funciones al ser compilada y una
/// importacion de las mismas al ser utilizada. De ahi las macros

/// NOTA: Tal vez estas definiciones deberian ir en cada uno
/// de los programas pero de momento es mas facil juntarlos aqui.
#ifdef Q_OS_WIN32

#ifdef BL_BULMALIB
#define BL_EXPORT __declspec(dllexport)
#else
#define BL_EXPORT __declspec(dllimport)
#endif

#ifdef BF_BULMAFACT
#define BF_EXPORT __declspec(dllexport)
#else
#define BF_EXPORT __declspec(dllimport)
#endif

#ifdef BC_BULMACONT
#define BC_EXPORT __declspec(dllexport)
#else
#define BC_EXPORT __declspec(dllimport)
#endif

#ifdef BT_BULMATPV
#define BT_EXPORT __declspec(dllexport)
#else
#define BT_EXPORT __declspec(dllimport)
#endif


#ifdef BA_BULMAADMIN
#define BA_EXPORT __declspec(dllexport)
#else
#define BA_EXPORT __declspec(dllimport)
#endif
#else


#define BL_EXPORT
#define BF_EXPORT
#define BC_EXPORT
#define BT_EXPORT
#define BA_EXPORT

#endif


/// Trasladamos los modos edicion o seleccion a defines ya
/// que son utilizados en muchas clases y ademas no hay
/// una superclase que los englobe a todos.
#define BL_EDIT_MODE   0
#define BL_SELECT_MODE 1
typedef unsigned int edmode;
#endif



