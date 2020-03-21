/***************************************************************************
 *   Copyright (C) 2008 by Tomeu Borrás Riera.                             *
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

#include <QDesktopWidget>

#include "blprogressbar.h"
#include "blconfiguration.h"


///
/**
**/
BlProgressBar::BlProgressBar (  ) : QDialog ( 0, Qt::SplashScreen )
{
    BL_FUNC_DEBUG
    if ( g_confpr->value( CONF_SHOW_PROGRESS_BAR ) == "TRUE" ) {
        setupUi ( this );
        QDesktopWidget *pantalla = new QDesktopWidget();
        setGeometry ( ( pantalla->screenGeometry().width() / 2 ) - this->width() / 2, ( pantalla->screenGeometry().height() / 2 ) - this->height() / 2, this->width(), this->height() );
        delete pantalla;
    } // end if
    
}


///
/**
**/
void BlProgressBar::setText ( const QString &tex )
{
    BL_FUNC_DEBUG

    if ( g_confpr->value( CONF_SHOW_PROGRESS_BAR ) == "TRUE" ) {
        mui_progressBar_label->setText ( tex );
        /// Si no hacemos una espera no se pinta bien el setText.
        update();
//        for ( int i = 0; i < 20; i++ )
//            repaint();
    } // end if
    
}


///
/**
**/
BlProgressBar::~BlProgressBar()
{
    BL_FUNC_DEBUG
    
}


///
/**
**/
void BlProgressBar::setValue ( int value )
{
    BL_FUNC_DEBUG
    if ( g_confpr->value( CONF_SHOW_PROGRESS_BAR ) == "TRUE" )
        mui_progressBar->setValue ( value );
    

}


///
/**
**/
int BlProgressBar::value() const
{
    BL_FUNC_DEBUG
    
    if ( g_confpr->value(CONF_SHOW_PROGRESS_BAR) == "TRUE" ) {
        return mui_progressBar->value();
    } else {
        return 0;
    } // end if
}


///
/**
**/
void BlProgressBar::setRange ( int range,  int range1 )
{
    BL_FUNC_DEBUG
    if ( g_confpr->value( CONF_SHOW_PROGRESS_BAR ) == "TRUE" )
        mui_progressBar->setRange ( range, range1 );
    
}


///
/**
**/
void BlProgressBar::show()
{
    BL_FUNC_DEBUG
    if ( g_confpr->value( CONF_SHOW_PROGRESS_BAR ) == "TRUE" ) {
        QWidget::show();
    } // end if
    
}

