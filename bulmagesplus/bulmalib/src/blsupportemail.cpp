/***************************************************************************
 *   Copyright (C) 2013 by Daniel Ripoll Osma.                             *
 *   Copyright (C) 2013 by Fco. Javier Miró Carmona.                       *
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

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QPlainTextEdit>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QLabel>
#include <QCheckBox>
#include <QMenuBar>

#include "blsupportemail.h"

BlSupportEmail::BlSupportEmail(BlMainWindow *parent) :
    QDockWidget(parent)
{
    BL_FUNC_DEBUG
    
    try{
        m_parent = parent;
        setupUi( this );
        setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetClosable);


        m_dockHeight = 340;
        m_dockWidth = 340;

        setStyleSheet("* {color: black;" "background: qlineargradient( x1:0 y1:0, x2:1 y2:0, stop:0 #FFF, stop:1 #BBB);}");
        //setFloating(true);

        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(1000);
        animation->setStartValue(QRect(m_parent->width() - m_dockWidth, m_parent->height() - m_parent->statusBar()->height(), m_dockWidth, 0));
        animation->setEndValue(QRect(m_parent->width() - m_dockWidth, m_parent->height() - m_parent->statusBar()->height() - m_dockHeight, m_dockWidth, m_dockHeight));
        connect (animation, SIGNAL(valueChanged(const QVariant &)), this, SLOT(resizeDockWidget()));
        
        animation->start();
        
    } catch ( ... ) {
        blMsgInfo ( _ ( "No se ha podido crear la ventana de soporte por email" ) );
    } // end try;
}

BlSupportEmail::~BlSupportEmail()
{
}


void BlSupportEmail::resizeDockWidget()
{
    setGeometry(m_parent->width() - width(), m_parent->height() - m_parent->statusBar()->height() - height(), m_dockWidth, m_dockHeight);
}


void BlSupportEmail::on_mui_sendEmail_clicked()
{
    BL_FUNC_DEBUG
    
    
    QUrl url = QUrl ("https://bulmagesplus.com/route.php");
    QUrlQuery postData;
    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    
    postData.addQueryItem("action", "email");
    postData.addQueryItem("username", QUrl::toPercentEncoding(mui_userName->text().toUtf8()));
    postData.addQueryItem("useremail",mui_email->text().toUtf8());
    postData.addQueryItem("emailbody",QUrl::toPercentEncoding(mui_body->toPlainText().toUtf8()));
    postData.addQueryItem("emailsubject",QUrl::toPercentEncoding(mui_subject->text().toUtf8()));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=utf-8");
    request.setHeader(QNetworkRequest::ContentLengthHeader, postData.toString().mid(1).length());

    networkManager->post(request, postData.toString().mid(1).toUtf8());

    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    
    this->close();
    
}

void BlSupportEmail::replyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        fprintf (stderr, reply->readAll());
    }
    else
    {
        fprintf(stderr, reply->errorString().toLatin1());
    }
}
