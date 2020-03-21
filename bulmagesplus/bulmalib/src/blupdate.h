/***************************************************************************
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

#ifndef BLUPDATE_H
#define BLUPDATE_H

#include <QWidget>
#include <QTimer>
#include <QDockWidget>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "blfunctions.h"
#include "blmainwindow.h"


class BL_EXPORT BlUpdate : public QWidget
{
  Q_OBJECT

  private:
    QTimer *m_timer1{nullptr};
    QTimer *m_timer2{nullptr};
    QDockWidget *m_dockWidget{nullptr};
    BlMainWindow *m_parent{nullptr};
    bool m_firstExecution{};
    int m_dockWidth{};
    int m_dockHeight{};
    QString m_urlUpdateServer;
    QString m_programRunningName;
    QString m_programRunningVersion;

    /// Necesario para descargar.
    QNetworkReply *m_reply{nullptr};
    QNetworkAccessManager *m_networkManager{nullptr};
    QNetworkReply::NetworkError m_replyError;

    /// Autenticación de usuario.
    bool m_isUserRegistered{};
    QString m_userEmail;
    QString m_tokenA;
    QString m_tokenB;
    QString m_eliteUntil; /// Fecha formateada como "YYYY-mm-dd".

    public:
      explicit BlUpdate(BlMainWindow * parent = nullptr);
      ~BlUpdate();
      void setProgramRunningName(QString name);

      bool isUserRegistered();

      /// Necesario para descargar.
      QByteArray download(const QString & url);
      QNetworkReply::NetworkError error();

      QString encode(QString dato, QString clave);
      QString userEmail() const;
      QString tokenA() const;
      QString tokenB() const;
      QString eliteUntil() const;
      void executeUpdater();
      QString lastInstalledUpdate();

    public slots:
      void checkUpdates();
      void processAllPendingEvents();

    private slots:
      void checkWindowVisible();
      void resizeDockWidget();
      void anchorClicked(const QUrl & link);

      /// Necesario para descargar.
      void registerError(QNetworkReply::NetworkError code);
};

#endif

