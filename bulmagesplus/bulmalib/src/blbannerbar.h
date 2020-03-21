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

#ifndef BLBANNERBAR_H
#define BLBANNERBAR_H

#include <QWidget>
#include "bldefs.h"

QT_FORWARD_DECLARE_CLASS(QString);
QT_FORWARD_DECLARE_CLASS(QWebView);
QT_FORWARD_DECLARE_CLASS(QToolBar);
QT_FORWARD_DECLARE_CLASS(QUrl);
QT_FORWARD_DECLARE_CLASS(QNetworkReply);


class BL_EXPORT BlBannerBar : public QWidget
{
  Q_OBJECT

  private:
    QWebView *m_webview = nullptr;
    QWidget *m_banner = nullptr;
    QToolBar *m_parent = nullptr;
    QString m_url{};
    QString m_programRunning{};

  public:
    explicit BlBannerBar(QToolBar * parent);
    ~BlBannerBar();
    void initBar();

  public slots:
    void updateBanner();
    void detectUrl(const QUrl & url);
    void replyFinished(QNetworkReply * reply);
    void launchBrowser();
    void setProgramName(const QString & programName);
};

#endif
