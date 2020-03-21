/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borrás Riera.                             *
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

#include "blsplashscreen.h"
#include "blconfiguration.h"


///
/**
\param appSplash
\param appName
\param appVersion
**/
BlSplashScreen::BlSplashScreen ( QString appSplash, QString appName, QString appVersion ) : QSplashScreen ( QPixmap(), Qt::FramelessWindowHint )
{
    BL_FUNC_DEBUG

    QFile archivo ( appSplash );
    if ( !archivo.exists() ) {
        QString mensaje = "BlSplashScreen: No existe el archivo " + appSplash + "\n";
        fprintf ( stderr, "%s", mensaje.toLatin1().constData() );
        m_image0 = new QPixmap ( 350, 263 );
        m_image0->fill ( QColor ( QColor ( 150, 150, 200 ) ) );
    } else {
        m_image0 = new QPixmap();
        m_image0->load ( appSplash );
    }// end if

    /// Se modifica la paleta para que utilize la imagen como fondo.
    QPalette p = this->palette();
    m_brush = new QBrush();
    m_brush->setTexture(*m_image0);
    p.setBrush ( QPalette::Window, *m_brush  );
    this->setPalette ( p );

    /// Centramos la ventana en la pantalla.
    QDesktopWidget *pantalla = new QDesktopWidget();
    setGeometry ( ( pantalla->screenGeometry().width() / 2 ) - ( m_image0->width() / 2 ), ( pantalla->screenGeometry().height() / 2 ) - ( ( m_image0->height() + 58 ) / 2 ), m_image0->width(), m_image0->height() );
    delete pantalla;

#ifndef Q_OS_WIN32
    l0 = new QLabel ( this );
    l0->hide();
    l0->setTextFormat ( Qt::RichText );
    l0->setGeometry ( 0, 0, m_image0->width(), m_image0->height() );
    l0->setAlignment ( Qt::AlignTop );
    l0->setFont ( QFont ( "Arial", 20, QFont::Bold ) );
    l0->setText ( "<center><font size=+1 color=\"#a3ffa3\">" + appName + "</font><br/><font color=\"#0000ff\">" + appVersion + "</font></center>" );
#endif
	
    m_label = new QLabel ( this );
    m_label->setTextFormat ( Qt::RichText );
    m_label->setGeometry ( 0, m_image0->height() - 80, m_image0->width(), 15 );
    m_label->setAlignment ( Qt::AlignTop );
    m_label->setFont ( QFont ( "helvetica", 8, QFont::Bold ) );
    //m_label->show();
    m_label->hide();

    m_label1 = new QLabel ( this );
    m_label1->setAlignment ( Qt::AlignBottom );
    m_label1->setFont ( QFont ( "helvetica", 9, QFont::Normal ) );
    m_label1->setGeometry ( 0, m_image0->height() - 50, m_image0->width(), 50 );
    m_label1->hide();

    m_barra = new QProgressBar ( this );
    m_barra->setTextVisible ( false );
    m_barra->setVisible(false);
    /// Poniendo el minimo y maximo a 0 hace el efecto especial.
    m_barra->setRange ( 0, 100 );
    m_barra->setGeometry ( 0, m_image0->height() - 65, m_image0->width(), 15 );
    QPalette pbarra = m_barra->palette();
    QColor colorfondobarra = QColor ( "#000000" );
    colorfondobarra.setAlpha ( 100 );
    pbarra.setBrush ( QPalette::Base, colorfondobarra );
    m_barra->setPalette ( pbarra );
    
}


///
/**
**/
BlSplashScreen::~BlSplashScreen()
{
    BL_FUNC_DEBUG
#ifndef Q_OS_WIN32
    delete l0;
#endif
    delete m_label;
    delete m_label1;
    delete m_image0;
    delete m_brush;
    delete m_barra;
    
}


///
/**
\param mens
**/
void BlSplashScreen::setMessage ( QString mens )
{
    BL_FUNC_DEBUG

/*
    BlDebug::blDebug ( "BlSplashScreen::setMessage", 0, mens );
    static int a = 0;
    static QString cadant = "";
    QString cad = "";
    QString cad1 = "";
    if ( a != 0 )
        cad1 = "<FONT SIZE=\"-3\" COLOR='#FF0000'>&nbsp;.......&nbsp;<B>OK</B></FONT><BR>";
    cad = cad + "<FONT SIZE=\"-3\" COLOR='#000066'>" + mens + "</FONT>";
    a++;
    m_label1->setText ( cadant + cad1 + cad );
    //m_label1->repaint();
    m_label1->update();
    m_label->setText ( mens );
    //m_label->repaint();
    m_label->update();
    update();
    //repaint();
    cadant = cad;

    */
    
    repaint();
}


///
/**
\param progreso
**/
void BlSplashScreen::setProgressBar ( int progreso )
{
    BL_FUNC_DEBUG

    //m_barra->setValue ( progreso );
    
    repaint();
}


