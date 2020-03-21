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

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QToolBar>
#include <QUrl>
#include <QtWebKitWidgets>
#include <QTimer>
#include "blbannerbar.h"
#include "blfunctions.h"
#include "blconfiguration.h"
#include "blclickablelabel.h"
#include "blupdate.h"
#include "../../bulmagesplus_version.h"


BlBannerBar::BlBannerBar(QToolBar * parent) : m_parent{parent}
{
  BL_FUNC_DEBUG

  m_banner = new QWidget;
  m_webview = new QWebView(m_banner);
  m_url = QString("http://www.bulmagesplus.com"); /// TODO https
}


BlBannerBar::~BlBannerBar()
{
  BL_FUNC_DEBUG

  delete m_banner;
  delete m_webview;
}


void BlBannerBar::setProgramName(const QString & programName)
{
  BL_FUNC_DEBUG

  m_programRunning = programName;
}


void BlBannerBar::detectUrl(const QUrl & url)
{
  BL_FUNC_DEBUG

  if (url.isValid())
  {
    blWebBrowser(url.toString());
  } // end if
}


void BlBannerBar::launchBrowser()
{
  BL_FUNC_DEBUG

  blWebBrowser(m_url);
}


void BlBannerBar::updateBanner()
{
  BL_FUNC_DEBUG

  QLabel *picture{m_parent->findChild<QLabel*>("mui_labelBanner")};
  m_webview->show();

  if (picture)
  {
    picture->hide();
    m_webview->show();
  } // end if
}


void BlBannerBar::replyFinished(QNetworkReply * reply)
{
  BL_FUNC_DEBUG

  if (reply->error() == QNetworkReply::NoError)
  {
    QTimer::singleShot(5000, this, SLOT(updateBanner()));

    m_webview->setContextMenuPolicy(Qt::NoContextMenu);
    m_webview->setMinimumSize(200,47);
    m_webview->setMaximumSize(200,47);
    m_webview->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    m_webview->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

    BlUpdate *update{new BlUpdate{}};
    QString stringUrl{m_url + QString("/banners/ads.php")};
    QUrl url{};

    if (update->isUserRegistered())
    {
      url = QUrl (stringUrl + "?" + "tokena=" + update->tokenA() + "&" + "product_auth_id=" + QString(PRODUCT_AUTH_ID) + "&" + "os=" + QString(OPERATING_SYSTEM) + "&" + "arch=" + QString(ARCH) + "&" + "build_version=" + QString(BUILD_VERSION) + "&" + "caller=" + m_programRunning);

    } else
    {
      url = QUrl (stringUrl + "?" + "product_auth_id=" + QString(PRODUCT_AUTH_ID) + "&" + "os=" + QString(OPERATING_SYSTEM) + "&" + "arch=" + QString(ARCH) + "&" + "build_version=" + QString(BUILD_VERSION) + "&" + "caller=" + m_programRunning);
    } // end if

    m_webview->load(url);

    QPalette palette = m_webview->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);

    m_webview->page()->setPalette(palette);

    m_webview->setStyleSheet("background-color:transparent;");
    m_webview->setAttribute(Qt::WA_TranslucentBackground, true);
    m_webview->setAttribute(Qt::WA_OpaquePaintEvent, false);
    m_webview->setWindowFlags(Qt::FramelessWindowHint);

    m_webview->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
    connect(m_webview,SIGNAL(linkClicked(const QUrl)),this,SLOT(detectUrl(const QUrl)),Qt::DirectConnection);
  } // end if
}


void BlBannerBar::initBar()
{
  BL_FUNC_DEBUG

  m_banner->setMinimumSize(201,47);
  m_banner->setMaximumSize(201,47);

  m_banner->setStyleSheet("background:transparent;");
  m_banner->setAttribute(Qt::WA_TranslucentBackground);
  m_banner->setWindowFlags(Qt::FramelessWindowHint);

  m_webview->hide();

  BlClickableLabel *picture{new BlClickableLabel{"", m_banner}};
  QPixmap pixmap{":/imagesplus/banner.png"};
  picture->setPixmap(pixmap);
  picture->setObjectName("mui_labelBanner");
  picture->setMinimumSize(200,40);
  picture->setMaximumSize(200,40);
  picture->setAlignment(Qt::AlignVCenter);
  connect(picture, SIGNAL(clicked()), this, SLOT(launchBrowser()));

  m_parent->addWidget(m_banner);

  QNetworkAccessManager* manager{new QNetworkAccessManager{this}};
  connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
  manager->get(QNetworkRequest(QUrl(m_url)));
}
