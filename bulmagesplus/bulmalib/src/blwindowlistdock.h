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

#ifndef BLWINDOWLISTDOCK_H
#define BLWINDOWLISTDOCK_H

#include <QDockWidget>
#include <QListWidget>
#include "blfunctions.h"
#include "blworkspace.h"

class BlWorkspace;


class BL_EXPORT BlListWidget : public QListWidget
{
  Q_OBJECT

  public:
    explicit BlListWidget(QWidget *parent = nullptr);
    ~BlListWidget();
};


class BL_EXPORT BlListWidgetItem : public QListWidgetItem
{
  private:
    QObject *m_obj{nullptr};
    BlListWidget *m_list{nullptr};
    QString m_name;
    QString m_title;

  public:
    BlListWidgetItem(BlListWidget *listWidgetItem, QPixmap &pixmap);
    ~BlListWidgetItem();
    void setObject(QObject * object);
    void setName(QString name);
    void setTitle(QString title);
    QObject * object();
    QString name();
};


/// Provides the dock window to list all windows in BulmaFact
/// \param m_listBox this variable is the listBox that contains all
/// titles of the diferent windows.
class BL_EXPORT BlWindowListDock : public QDockWidget
{
  Q_OBJECT

  private:
    BlListWidget *m_listBox = nullptr;
    BlWorkspace *m_pWorkspace = nullptr;

  public:
    explicit BlWindowListDock(QWidget *widget = nullptr);
    ~BlWindowListDock();
    void setWorkspace(BlWorkspace *workspace);

  public:
    int insertWindow(QString name, QObject *object, bool checkDuplication = true, QString title = QString{""});
    int selectWindow(QString nombre, QMdiSubWindow *obj);
    int deselectWindow();
    void removeWindow(QObject *);
    void setVisibilityState(bool);
    void vaciar();
    void vaciarCompleto();
    int numVentanas();
    QObject *ventana(int index);

  protected:
    void closeEvent(QCloseEvent *event);

  public slots:
    virtual void dclicked();
    virtual void clicked();

  signals:
    void visibilityStateChanged(bool);
};

#endif

