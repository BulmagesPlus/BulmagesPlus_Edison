/***************************************************************************
 *   Copyright (C) 2006 by Fco. Javier Miró Carmona.                       *
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

#include <QWidget>
#include <QAction>
#include <QList>
#include "blworkspace.h"
#include "local_blI18n.h"


///
/**
**/
BlWorkspace::BlWorkspace(QWidget *)
{
  BL_FUNC_DEBUG
}


///
/**
**/
BlWorkspace::~BlWorkspace()
{
  BL_FUNC_DEBUG
}


///
/**
\param subWindow
**/
void BlWorkspace::addSubWindow(QWidget * subWindow)
{
  BL_FUNC_DEBUG

  int tamdispW{0};
  int tamdispH{0};
  int tamventanadecoW{0};
  int tamventanadecoH{0};
  int tamventanaW{0};
  int tamventanaH{0};
  int margen{10};

  QMdiSubWindow *sw{new QMdiSubWindow{}};
  sw->setWidget(subWindow);
  connect(subWindow, SIGNAL(destroyed(QObject *)), sw, SLOT(close()));
  connect(subWindow, SIGNAL(hided(QObject *)), sw, SLOT(hide()));
  connect(subWindow, SIGNAL(showed(QObject *)), sw, SLOT(show()));

  QMdiArea::addSubWindow(sw);

  /// Se comprueba el tamaño de la ventana que esta diseñada con Designer:
  /// Sólo si la ventana es más grande que el espacio de representación
  /// se ajusta para caber dentro.

  /// Captura el tamaño disponible. Restamos el margen que dejamos a cada lado (x2).
  tamdispW = this->width() - (margen * 2);
  tamdispH = this->height() - (margen * 2);

  /// Captura el tamaño con decoración de la ventana a insertar.
  tamventanadecoW = sw->parentWidget() ->frameGeometry().width();
  tamventanadecoH = sw->parentWidget() ->frameGeometry().height();

  /// Captura el tamaño sin decoración de la ventana a insertar.
  tamventanaW = subWindow->geometry().width();
  tamventanaH = subWindow->geometry().height();

  /// Comprobamos si es necesario cambiar el tamaño a la ventana.
  if (tamventanadecoW > tamdispW) tamventanaW = tamdispW - (tamventanadecoW - tamventanaW);
  if (tamventanadecoH > tamdispH) tamventanaH = tamdispH - (tamventanadecoH - tamventanaH);

  subWindow->setGeometry(0, 0, tamventanaW, tamventanaH);
  subWindow->parentWidget()->move(margen, margen);

  /// Crea un QAction para manejar la tecla rapida ESC para cerrar la ventana.
  QAction *accionEsc{new QAction{subWindow}};
  accionEsc->setShortcut(_("Esc"));
  connect(accionEsc, SIGNAL(triggered()), subWindow, SLOT(close()));
  subWindow->addAction(accionEsc);
}


QWidget *BlWorkspace::activeWindow() const
{
  return QApplication::activeWindow();
}

