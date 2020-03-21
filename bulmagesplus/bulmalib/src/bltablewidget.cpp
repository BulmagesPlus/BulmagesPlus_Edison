/***************************************************************************
 *   Copyright (C) 2003 by Tomeu Borrás Riera.                             *
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

#include <QDebug>
#include <QKeyEvent>
#include <QEvent>
#include <QLineEdit>
#include <QTextEdit>
#include <QBrush>
#include <QColor>

#include "bltablewidget.h"
#include "blconfiguration.h"


/** Constructor de BlTableWidget clase derivada de QTableWidget con
un eventHandler especifico
*/
/**
\param parent
**/
BlTableWidget::BlTableWidget ( QWidget *parent ) : QTableWidget ( parent )
{
    BL_FUNC_DEBUG

    setColumnOrder ( -1 );
    setOrderType ( -1 );
    installEventFilter ( this );
    connect ( this, SIGNAL ( itemChanged ( QTableWidgetItem * ) ), this, SLOT ( sitemChanged ( QTableWidgetItem * ) ) );
}


void BlTableWidget::moveRow ( int oldRow, int newRow )
{
    BL_FUNC_DEBUG

    int column;
    QList<QTableWidgetItem *> rowItemsOld;

    /// Captura los items de las fila
    for ( column = 0; column < columnCount(); ++column ) {
        rowItemsOld << takeItem ( oldRow, column );
    } // end for

    removeRow ( oldRow );
    insertRow ( newRow );

    /// Aqui habria que averiguar si esta activado la ordenacion
    /// automatica para deshabilitarla antes y habilitarla despues.

    /// Establece los items de la fila
    for ( column = 0; column < columnCount(); ++column ) {
        setItem ( newRow, column, rowItemsOld.at ( column ) );
    } // end for

}


/** Este m&eacute;todo no hace nada.
*/
/**
**/
void BlTableWidget::editItem ( QTableWidgetItem * )
{
    BL_FUNC_DEBUG
    
}


///
/**
**/
void BlTableWidget::sitemChanged ( QTableWidgetItem * )
{
    BL_FUNC_DEBUG
}


///
/**
\param type
\param mode
**/
BlTableWidgetItem::BlTableWidgetItem ( int type, int mode ) : QTableWidgetItem(type), QWidget(0)
{
    BL_FUNC_DEBUG
    modo = mode;
    setType(type);
}

///
/**
\param text
\param type
\param mode
**/
BlTableWidgetItem::BlTableWidgetItem ( const QString &text, int type, int mode ) : QTableWidgetItem(text, type), QWidget(0)
{
    BL_FUNC_DEBUG

    modo = mode;
    setType(type);
}


///
/**
**/
BlTableWidgetItem::~BlTableWidgetItem()
{
    BL_FUNC_DEBUG
}


void BlTableWidgetItem::setType(int type)
{
  BL_FUNC_DEBUG

  /// Solo se puede establecer una vez al crear el QTableWidgetItem.
  /// Type >= 1000 para usuario. Menor reservada a Qt.
  if (type == QTableWidgetItem::UserType)
  {
    /// Tipo editable
    setIcon(QIcon(":/Images/edit-find.png"));
  } // end if
}


///
/**
\param other
\return
**/
bool BlTableWidgetItem::operator< ( const QTableWidgetItem & other ) const
{
    BL_FUNC_DEBUG

    BlDebug::blDebug ( "BlTableWidgetItem::operator<", 0, text() + " < " + other.text() );
    bool oknumero;
    bool oknumero1;
    QString cad = text();
    QString cad1 = other.text();

    if ( cad != "" && cad1 != "" ) {
        /// Comprobamos si es un n&uacute;mero.
        double ncad = cad.toDouble ( &oknumero );
        double ncad1 = cad1.toDouble ( &oknumero1 );
        if ( oknumero && oknumero1 ) {
            
            return ncad < ncad1;
        } // end if
        /// Comprobamos si es una fecha
        if ( cad[2] == '/' && cad[5] == '/' ) {
            QDate fcad = blNormalizeDate ( cad );
            QString acad = fcad.toString ( Qt::ISODate );
            QDate fcad1 = blNormalizeDate ( cad1 );
            QString acad1 = fcad1.toString ( Qt::ISODate );
            if ( acad[4] == '-' && acad1[4] == '-' && acad[7] == '-' && acad1[7] == '-' ) {
                
                return fcad < fcad1;
            } // end if
        } // end if
    } // end if
    
    return cad < cad1;
}


///
/**
\return
**/
int BlTableWidget::orderType()
{
    BL_FUNC_DEBUG
    
    return m_orderType;
}


///
/**
\return
**/
int BlTableWidget::columnOrder()
{
    BL_FUNC_DEBUG
    
    return m_columnOrder;
}


///
/**
\param t
**/
void BlTableWidget::setOrderType ( int t )
{
    BL_FUNC_DEBUG
    m_orderType = t;
    
}


///
/**
\param t
**/
void BlTableWidget::setColumnOrder ( int t )
{
    BL_FUNC_DEBUG
    m_columnOrder = t;
    
}

///
/**
\param column
\param oldIndex
\param newIndex
**/
void BlTableWidget::columnMoved ( int column, int oldIndex, int newIndex )
{
    BL_FUNC_DEBUG
    QTableWidget::columnMoved ( column, oldIndex, newIndex );
    
}


///
/**
**/
BlTableWidget::~BlTableWidget()
{
    BL_FUNC_DEBUG
    
}

/// Habilita o inhabilita el ordenado de columnas mediante el pulsar sobre ellas.
/**
\param sorting
**/
void BlTableWidget::setSortingEnabled ( bool sorting )
{
    BL_FUNC_DEBUG
    QTableWidget::setSortingEnabled ( sorting );
    
}


/** EventFilter para BlTableWidget, procesa los eventos recibidos por la
tabla y emite signals si lo considera adecuado.
*/
/**
\param obj
\param event
\return
**/
bool BlTableWidget::eventFilter ( QObject *obj, QEvent *event )
{
//   BL_FUNC_DEBUG  No hacemos debug por no saturar el fichero
    if ( event->type() == QEvent::KeyPress ) {
//        BlDebug::blDebug ( "BlTableWidget::eventFilter() :" + QString::number ( event->type() ), 1 );
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> ( event );
        int key = keyEvent->key();
        int row = currentRow();
        /// Algunas veces se produce un eventfilter pero la fila no existe (-1) en esos
        /// casos abortamos la ejecucion del eventFilter para que no de fallos en la
        /// busqueda de que celda es.
        if ( row < 0 ) {
            return true;
        } // end if
        Qt::KeyboardModifiers mod = keyEvent->modifiers();
        switch ( key ) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
	  case Qt::Key_Backtab:
        case Qt::Key_Tab:
            m_teclasalida = key;
            return true;
        } // end switch
    } // end if

    /// Si es un release de tecla se hace la funcionalidad especificada.
    if ( event->type() == QEvent::KeyRelease ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> ( event );
        int key = keyEvent->key();
        
        /// Visualmente las coordinadas son diferentes a la tabla real y para recalcular
        /// la nueva posicion hay que utilizar las coordenadas reales. visualColumn de un visualColumn devuelve
        /// la columna real.
        int col = visualColumn(currentColumn());
        int row = visualRow(currentRow());

        m_teclasalida = 0;
        /// Algunas veces se produce un eventfilter pero la fila no existe (-1) en esos
        /// casos abortamos la ejecucion del eventFilter para que no de fallos en la
        /// busqueda de que celda es.
        if ( row < 0 ) {
            return true;
        } // end if
        Qt::KeyboardModifiers mod = keyEvent->modifiers();
        switch ( key ) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
	  case Qt::Key_Backtab:
        case Qt::Key_Tab:
//		if ( ! ( mod & Qt::ShiftModifier ) ) {
                m_teclasalida = key;
                emit cellRePosition ( row, col );
                return true;
                break;
//		}// end if
        case Qt::Key_Slash:
            if ( ( mod & Qt::ControlModifier ) || ( mod & Qt::AltModifier ) ) {
                emit pressedSlash ( row, col );
                return true;
            } // end if
            break;
        case Qt::Key_Minus:
            if ( ( mod & Qt::ControlModifier ) || ( mod & Qt::AltModifier ) ) {
                emit pressedMinus ( row, col );
                return true;
            } // end if
            break;
        case Qt::Key_Plus:
            if ( ( mod & Qt::ControlModifier ) || ( mod & Qt::AltModifier ) ) {
                emit pressedPlus ( row, col );
                return true;
            } // end if
            break;
        case Qt::Key_Asterisk:
            if ( ( mod & Qt::ControlModifier ) || ( mod & Qt::AltModifier ) ) {
                emit pressedAsterisk ( row, col );
                return true;
            } // end if
            break;
        case Qt::Key_F2:
            emit pressedAsterisk ( row, col );
            return true;
            break;
        case Qt::Key_Up:
            if ( ( mod & Qt::ControlModifier ) || ( mod & Qt::AltModifier ) ) {
                emit ctrlUp ( row, col );
                return true;
            } // end if
            /// Al pulsar la tecla 'arriba' se considera que es el fin de la edici&oacute;n de la celda.
        case Qt::Key_Down:
            if ( ( mod & Qt::ControlModifier ) || ( mod & Qt::AltModifier ) ) {
                emit ctrlDown ( row, col );
                return true;
            } // end if
            /// Al pulsar la tecla 'abajo' se considera que es el fin de la edici&oacute; de la celda.
            return true;
        case Qt::Key_Left:
            if ( ( mod & Qt::ControlModifier ) || ( mod & Qt::AltModifier ) ) {
                emit ctrlLeft ( row, col );
                return true;
            } // end if
        case Qt::Key_Right:
            if ( ( mod & Qt::ControlModifier ) || ( mod & Qt::AltModifier ) ) {
                emit ctrlRight ( row, col );
                return true;
            } // end if
        } // end switch
    } // end if

    return QTableWidget::eventFilter ( obj, event );
}


/** Se ha pulsado sobre las cabeceras en la tabla lo que invoca el
metodo de ordenacion.
*/
/**
**/
void BlTableWidget::ordenar()
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlTableWidget::ordenar", 0, QString::number ( m_columnOrder ) );
    /// Puede ocurrir que el parametro de ordenacion sea invalido por cualquier extranyo motivo.
    if ( m_columnOrder < columnCount() && m_columnOrder >= 0 ) {
        sortByColumn ( m_columnOrder );
    } // end if
    
}


/** Hace una asignacion de un elemento.
    @BUG: Parece que esta creando elementos de memoria sin intentar eliminar los antiguos.
*/
/**
\param x
\param y
\param val
**/
void BlTableWidget::setText ( int x, int y, const QString & val )
{
    BL_FUNC_DEBUG
    fprintf(stderr, "\n\tsetText: " + QString::number(x).toLatin1() + " " + QString::number(y).toLatin1() + " " + val.toLatin1() + "\n" );
    BlTableWidgetItem *newitem = new BlTableWidgetItem ( val );
    setItem ( x, y, newitem );
    fprintf(stderr, "\n\tsetItem: " + QString::number(x).toLatin1() + " " + QString::number(y).toLatin1() + " " + val.toLatin1() + "\n" );
    
}


///
/**
\param column
\param order
**/
void BlTableWidget::sortByColumn ( int column, Qt::SortOrder order )
{
    BL_FUNC_DEBUG
    QTableWidget::sortByColumn ( column, order );
    
}


///
/**
\param column
\param order
**/
void BlTableWidget::sortItems ( int column, Qt::SortOrder order )
{
    BL_FUNC_DEBUG
    QTableWidget::sortItems ( column, order );
    
}


void BlTableWidget::showColumn ( int column )
{
    BL_FUNC_DEBUG
    QTableWidget::showColumn ( column );
    
}


void BlTableWidget::hideColumn ( int column )
{
    BL_FUNC_DEBUG
    QTableWidget::hideColumn ( column );
    
}
/*

void BlTableWidget::removeRow ( int row )
{
    removeRow ( row );
    emit removedRow(row);
}
*/

