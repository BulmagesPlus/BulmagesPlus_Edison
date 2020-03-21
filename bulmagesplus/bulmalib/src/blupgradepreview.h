/***************************************************************************
 *   Copyright (C) 2012 by Daniel Ripoll Osma.                             *
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

#ifndef BLUPGRADEPREVIEW_H
#define BLUPGRADEPREVIEW_H

#include <QDialog>
#include <QWidget>
#include <QString>
#include <QMenu>
#include <QtWebKitWidgets>
#include <QLocale>
#include <QUrl>
#include <QList>
#include "blfunctions.h"
#include "blconfiguration.h"
#include "blsubform.h"
#include "bltreewidget.h"
#include "bldefs.h"


class BL_EXPORT BlUpgradePreviewView : public QDialog
{
  Q_OBJECT

  public:
    explicit BlUpgradePreviewView(QWidget * parent = nullptr);
    ~BlUpgradePreviewView();
    QWebView *g_view{nullptr};

  private:
    QWidget *m_parent{nullptr};

  public slots:
    void setHtml(QString content);

  private slots:
    void detectUrl(const QUrl & url);
};


class BL_EXPORT BlUpgradePreview: public QObject
{
  Q_OBJECT

  private:
    int m_iter;
    QList<QAction *> m_listActions;

  public:
    BlUpgradePreview();
    explicit BlUpgradePreview(BlSubForm *);
    explicit BlUpgradePreview(BlTreeWidget *);
    ~BlUpgradePreview();
    int BlSubForm_Post(BlSubForm *);
    int BlTreeWidget_Post(BlTreeWidget *);
    int BlSubForm_preparaMenu(BlSubForm *);

  public slots:
    void showUpgradePreviewInfo(const QString & parameters = QString{""}, const QString &templateName = QString{"index"}, const QString &zoneName = QString{"upgrade_preview"});
    void launchODSPreview(bool released);
    void launchODSPreview(QAction *);
    bool unloadPreview(const QString &theplugin);
    bool detectPlugins(const QString &theplugin);
    virtual void paintMenu(QMenu *);
};

#endif
