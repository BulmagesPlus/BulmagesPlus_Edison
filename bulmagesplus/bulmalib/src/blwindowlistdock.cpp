/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borrás Riera.                             *
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

#include <typeinfo>
#include <map>
#include <QObject>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QVBoxLayout>
#include "blwindowlistdock.h"
#include "local_blI18n.h"

///
/**
\param parent
**/
BlListWidget::BlListWidget(QWidget *parent) : QListWidget{parent}
{
  BL_FUNC_DEBUG
}


///
/**
**/
BlListWidget::~BlListWidget()
{
  BL_FUNC_DEBUG
}


///
/**
\param listWidget
\param pixmap
**/
BlListWidgetItem::BlListWidgetItem(BlListWidget *listWidget, QPixmap &pixmap) : QListWidgetItem{listWidget}
{
  BL_FUNC_DEBUG

  setIcon(QIcon(pixmap));
  m_list = listWidget;
}


///
/**
**/
BlListWidgetItem::~BlListWidgetItem()
{
  BL_FUNC_DEBUG
}


///
/**
\param object
**/
void BlListWidgetItem::setObject(QObject *object)
{
  BL_FUNC_DEBUG

  m_obj = object;
}


///
/**
\param name
**/
void BlListWidgetItem::setName(QString name)
{
  BL_FUNC_DEBUG

  m_name = name;
}


///
/**
\param title
**/
void BlListWidgetItem::setTitle(QString title)
{
  BL_FUNC_DEBUG

  m_title = title;
  setText(title);
}


///
/**
\return
**/
QObject *BlListWidgetItem::object()
{
  BL_FUNC_DEBUG

  return m_obj;
}


///
/**
\return
**/
QString BlListWidgetItem::name()
{
  BL_FUNC_DEBUG

  return m_name;
}


///
/**
\param workspace
**/
void BlWindowListDock::setWorkspace(BlWorkspace *workspace)
{
  BL_FUNC_DEBUG

  m_pWorkspace = workspace;
}


///
/**
\param widget
**/
BlWindowListDock::BlWindowListDock(QWidget *widget) : QDockWidget{widget}
{
  BL_FUNC_DEBUG

  m_listBox = new BlListWidget{this};
  m_listBox->setObjectName("indexador");
  m_listBox->setIconSize(QSize(32, 32));
  m_listBox->setContentsMargins(0, 0, 0, 0);
  m_listBox->setSpacing(0);
  m_listBox->setWindowTitle(_("Indexador"));
  m_listBox->setMinimumSize(260, 220);
  setWindowTitle(_("Indexador"));
  setFocusPolicy(Qt::StrongFocus);
  setWidget(m_listBox);
  setObjectName("Indexador");
  connect(m_listBox, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(dclicked()));
  connect(m_listBox, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(clicked()));
}


///
/**
**/
void BlWindowListDock::dclicked()
{
  BL_FUNC_DEBUG

  QWidget *widget{(QWidget *)((BlListWidgetItem *) m_listBox->currentItem())->object()};
  if (widget)
  {
    if (widget->isMaximized())
    {
      widget->showNormal();

    } else
    {
      widget->showMaximized();
    } // end if

    widget->parentWidget()->activateWindow();
  } // end if
}


///
/**
**/
void BlWindowListDock::clicked()
{
  BL_FUNC_DEBUG

  QWidget *widget{(QWidget *)((BlListWidgetItem *) m_listBox->currentItem())->object()};
  if (widget)
  {
    widget->show();
    if (widget->parentWidget()) widget->parentWidget()->raise();
    if (widget->parentWidget()) widget->parentWidget()->activateWindow();
  } // end if
}


///
/**
**/
BlWindowListDock::~BlWindowListDock()
{
  BL_FUNC_DEBUG

  delete m_listBox;
}


///
/**
**/
void BlWindowListDock::vaciar()
{
  BL_FUNC_DEBUG

  /// Buscamos la ventana correspondiente y la borramos.
  int i{0};

  while (i < m_listBox->count())
  {
    BlListWidgetItem * m{(BlListWidgetItem *) m_listBox->item(i)};
    if (((QWidget *) m->object())->testAttribute(Qt::WA_DeleteOnClose))
    {
      delete m->object();

    } else
    {
      i++;
    } // end if
  } // end while
}


///
/**
\return
**/
int BlWindowListDock::numVentanas()
{
  BL_FUNC_DEBUG

  return m_listBox->count();
}


///
/**
\param index
\return
**/
QObject *BlWindowListDock::ventana(int index)
{
  BL_FUNC_DEBUG

  BlListWidgetItem *listWidgetItem{( BlListWidgetItem *) m_listBox->item(index)};

  return listWidgetItem->object();
}


///
/**
**/
void BlWindowListDock::vaciarCompleto()
{
  BL_FUNC_DEBUG

  /// Buscamos la ventana correspondiente y la borramos.
  int i{0};

  while (i < m_listBox->count())
  {
    BlListWidgetItem *listWidgetItem{(BlListWidgetItem *) m_listBox->item(i)};
    delete listWidgetItem->object();
  } // end while
}


/// This function provides the ability of push a window in the dock window
/// name This QString is the name of the window that was shown in the listbox
/// object This QObject * contains the pointer of the window for furtner reference.
/**
\param name
\param object
\param checkDuplication
\return
**/
int BlWindowListDock::insertWindow(QString name, QObject *object, bool checkDuplication, QString title)
{
  BL_FUNC_DEBUG

  BlDebug::blDebug("BlWindowListDock::insertWindow", 0, name);
  try
  {
    /// Comprobamos que haya un título y de no ser el caso se pone el nombre.
    if (title.isEmpty()) title = name;

    int i{0};

    while (i < m_listBox->count())
    {
      BlListWidgetItem * listWidgetItem{( BlListWidgetItem *) m_listBox->item(i)};
      /// Si la ventana ya esta en la lista.
      if (listWidgetItem->object() == object)
      {
        listWidgetItem->setName(name);
        listWidgetItem->setTitle(title);
        return 0;
      } // end if

      /// Comprobamos ventanas duplicadas.
      if (listWidgetItem->name() == name && checkDuplication)
      {
        ((QWidget *) listWidgetItem->object())->hide();
        ((QWidget *) listWidgetItem->object())->show();
        name = name + "(i)";
      } // end if

      i++;
    } // end while

    if (i >= m_listBox->count())
    {
      QPixmap icon{((QWidget *) object)->windowIcon().pixmap(32, 32)};
      BlListWidgetItem *listWidgetItem{new BlListWidgetItem{m_listBox, icon}};
      listWidgetItem->setObject(object);
      listWidgetItem->setName(name);
      listWidgetItem->setTitle(title);
    } // end if
  } catch ( ... )
  {
    return - 1;
  } // end try

  return 0;
}


/// Sirve para seleccionar una ventana listada en el Indexador.
/**
\param name
\param object
\return
**/
int BlWindowListDock::selectWindow(QString name, QMdiSubWindow *object)
{
  BL_FUNC_DEBUG

  try
  {
    int i{0};
    while (i < m_listBox->count())
    {
      BlListWidgetItem *listWidgetItem{(BlListWidgetItem *) m_listBox->item(i)};
      /// Encuentra la ventana en la lista.
      if (listWidgetItem->object() == object->widget())
      {
        m_listBox->setCurrentItem(listWidgetItem);
        return 0;
      } // end if

      i++;
    } // end while
  } catch ( ... )
  {
    throw - 1;
  } // end try

  return 0;
}


/// Deselecciona todas las entradas del Indexador.
/**
\return
**/
int BlWindowListDock::deselectWindow()
{
  BL_FUNC_DEBUG

  try
  {
    m_listBox->clearSelection();
  } catch (...)
  {
    BlDebug::blDebug("BlWindowListDock::deselectWindow", 2, "Error en la Seleccion");
    throw - 1;
  } // end try

  return 0;
}


///
/**
\param object
**/
void BlWindowListDock::removeWindow(QObject *object)
{
  BL_FUNC_DEBUG

  /// Buscamos la entrada correspondiente dentro del Indexador y la borramos.
  int i{0};

  while (i < m_listBox->count())
  {
    BlListWidgetItem *listWidgetItem{(BlListWidgetItem *) m_listBox->item(i)};
    if (listWidgetItem->object() == object)
    {
      BlDebug::blDebug("Ventana encontrada y vamos a sacarla", 0, listWidgetItem->name());
      m_listBox->takeItem(i);
      delete listWidgetItem;
      break;
    } // end if

    i++;
  } // end while

  /// Deseleccionamos cualquier elemento del listado para que no se quede marcado si
  /// ninguna otra ventana recoge el foco.
  deselectWindow();
}


///
/**
\param visible
**/
void BlWindowListDock::setVisibilityState(bool visible)
{
  BL_FUNC_DEBUG

  if (visible)
  {
    this->show();

  } else
  {
    this->hide();
  } // end if
}


///
/**
**/
void BlWindowListDock::closeEvent(QCloseEvent *)
{
  BL_FUNC_DEBUG

  emit(visibilityStateChanged(false));
}

