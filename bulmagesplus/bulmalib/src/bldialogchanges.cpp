/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borrás Riera.                             *
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

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QTableWidget>
#include <QComboBox>
#include <QCheckBox>

#include "bldatesearch.h"
#include "bldialogchanges.h"

///
/**
\param ob
**/
BlDialogChanges::BlDialogChanges ( QObject *ob )
{
    BL_FUNC_DEBUG

    m_obje = ob;
}


///
/**
**/
BlDialogChanges::~BlDialogChanges()
{
    BL_FUNC_DEBUG
}


///
/**
\param item
\return
**/
bool BlDialogChanges::dialogChanges_isObjectExcluded ( QObject *item )
{
    BL_FUNC_DEBUG
    bool excluido = false;

    QListIterator<QObject *> it_excluidos ( m_listaExcluidos );
    /// Itera por los QObjects que estan excluidos de comprobacion de cambios.
    it_excluidos.toFront();
    while ( it_excluidos.hasNext() ) {
        /// Comparamos
        if ( it_excluidos.next() == ( QObject * ) item ) {
            excluido = true;
        } // end if
    } // end while
    
    return excluido;
}


///
/**
**/
void BlDialogChanges::dialogChanges_readValues()
{
    BL_FUNC_DEBUG

    try
    {
        m_maxQLine = 0;
        m_maxQPlainText = 0;
        m_maxQText = 0;
        m_maxQTable = 0;
        m_maxQComboBox = 0;
        m_maxQCheckBox = 0;
        m_maxBlDateSearch = 0;

        QListIterator<QObject *> it_excluidos ( m_listaExcluidos );

        QList<QComboBox *> l4 = m_obje->findChildren<QComboBox *>();
        QListIterator<QComboBox *> it4 ( l4 );
        while ( it4.hasNext() ) {
            QComboBox * item = it4.next();
            if ( item->objectName().startsWith ( "mui_" ) && !dialogChanges_isObjectExcluded ( item ) )
            {
                m_listaQComboBox[m_maxQComboBox++] = item;
            } // end if
        } // end while

        QList<QPlainTextEdit *> l5 = m_obje->findChildren<QPlainTextEdit *>();
        QListIterator<QPlainTextEdit *> it5 ( l5 );
        while ( it5.hasNext() ) {
            QPlainTextEdit * item = it5.next();
            if ( item->objectName().startsWith ( "mui_" ) && !dialogChanges_isObjectExcluded ( item ) ) {
                m_listaQPlainText[m_maxQPlainText++] = item;
            } // end if
        } // end while

        QList<QTextEdit *> l1 = m_obje->findChildren<QTextEdit *>();
        QListIterator<QTextEdit *> it1 ( l1 );
        while ( it1.hasNext() ) {
            QTextEdit * item = it1.next();
            if ( item->objectName().startsWith ( "mui_" ) && !dialogChanges_isObjectExcluded ( item ) ) {
                m_listaQText[m_maxQText++] = item;
            } // end if
        } // end while

        QList<QLineEdit *> l2 = m_obje->findChildren<QLineEdit *>();
        QListIterator<QLineEdit *> it2 ( l2 );
        while ( it2.hasNext() ) {
            QLineEdit * item = it2.next();
            if ( item->objectName().startsWith ( "mui_" ) && !dialogChanges_isObjectExcluded ( item ) ) {
                m_listaQLine[m_maxQLine++] = item;
            } // end if
        } // end while

        QList<QTableWidget *> l3 = m_obje->findChildren<QTableWidget *>();
        QListIterator<QTableWidget *> it3 ( l3 );
        while ( it3.hasNext() ) {
            QTableWidget * item = it3.next();
            if ( item->objectName().startsWith ( "mui_" ) && !dialogChanges_isObjectExcluded ( item ) ) {
                m_listaQTable[m_maxQTable++] = item;
            } // end if
        } // end while

        QList<QCheckBox *> l6 = m_obje->findChildren<QCheckBox *>();
        QListIterator<QCheckBox *> it6 ( l6 );
        while ( it6.hasNext() ) {
            QCheckBox * item = it6.next();
            if ( item->objectName().startsWith ( "mui_" ) && !dialogChanges_isObjectExcluded ( item ) ) {
                m_listaQCheckBox[m_maxQCheckBox++] = item;
            } // end if
        } // end while

	  QList<BlDateSearch *> l7 = m_obje->findChildren<BlDateSearch *>();
	  QListIterator<BlDateSearch *> it7 ( l7 );
	  while ( it7.hasNext() ) {
		BlDateSearch * item = it7.next();
		if ( item->objectName().startsWith ( "mui_" ) && !dialogChanges_isObjectExcluded ( item ) ) {
		    m_listaBlDateSearch[m_maxBlDateSearch++] = item;
		} // end if
	  } // end while

        m_valorinicial = calculateValues();
        
    } catch ( ... ) {
        
        return;
    } // end try
}


///
/**
\return
**/
bool BlDialogChanges::dialogChanges_isChanged()
{
    BL_FUNC_DEBUG
    QString valorfinal = calculateValues();
    
    return ( m_valorinicial != valorfinal );
}


///
/**
\return
**/
QString BlDialogChanges::calculateValues()
{
    BL_FUNC_DEBUG

    QString values = retrieveValues ( "QTableWidget" );
    values += retrieveValues ( "QLineEdit" );
    values += retrieveValues ( "QPlainTextEdit" );
    values += retrieveValues ( "QTextEdit" );
    values += retrieveValues ( "QComboBox" );
    values += retrieveValues ( "QCheckBox" );
    values += retrieveValues ( "BlDateSearch" );

    return values;
}


///
/**
\param objetoexcluido
**/
void BlDialogChanges::dialogChanges_setExcludedObject ( QObject *objetoexcluido )
{
    BL_FUNC_DEBUG
    m_listaExcluidos.append ( objetoexcluido );
    
}


///
/**
\param qsWidget
\return
**/
QString BlDialogChanges::retrieveValues ( QString qsWidget )
{
    BL_FUNC_DEBUG

    BlDebug::blDebug ( "BlDialogChanges::retrieveValues", 0, qsWidget );
    try
    {
        QString values = "";
        QListIterator<QObject *> it_excluidos ( m_listaExcluidos );

        if ( qsWidget == "QComboBox" )
        {
            for ( int i = 0; i < m_maxQComboBox; i++ )
            {
                if ( m_listaQComboBox[i] != NULL )
                {
                    if ( ( ( QComboBox* ) m_listaQComboBox[i] ) ->objectName().startsWith ( "mui_" ) )
                    {
                        values += ( ( QComboBox* ) m_listaQComboBox[i] ) ->objectName();
                        values += ( ( QComboBox* ) m_listaQComboBox[i] ) ->currentText();
                    } // end if
                } // end if
            } // end for
        } // end if


        if ( qsWidget == "QLineEdit" ) {
            for ( int i = 0; i < m_maxQLine; i++ ) {
                if ( m_listaQLine[i] != NULL ) {
                    if ( ( ( QLineEdit* ) m_listaQLine[i] ) ->objectName().startsWith ( "mui_" ) ) {
                        values += ( ( QLineEdit* ) m_listaQLine[i] ) ->objectName();
                        values += ( ( QLineEdit* ) m_listaQLine[i] ) ->text();

                    } // end if
                } // end if
            } // end for
        } // end if

        if ( qsWidget == "QPlainTextEdit" ) {
            for ( int i = 0; i < m_maxQPlainText; i++ ) {
                if ( m_listaQPlainText[i] != NULL ) {
                    if ( ( ( QPlainTextEdit* ) m_listaQPlainText[i] ) ->objectName().startsWith ( "mui_" ) )
                        values += ( ( QPlainTextEdit* ) m_listaQPlainText[i] ) ->objectName();
                    values += ( ( QPlainTextEdit* ) m_listaQPlainText[i] ) ->toPlainText();
                } // end if
            } // end for
        } // end if

        if ( qsWidget == "QTextEdit" ) {
            for ( int i = 0; i < m_maxQText; i++ ) {
                if ( m_listaQText[i] != NULL ) {
                    if ( ( ( QTextEdit* ) m_listaQText[i] ) ->objectName().startsWith ( "mui_" ) )
                        values += ( ( QTextEdit* ) m_listaQText[i] ) ->objectName();
                    values += ( ( QTextEdit* ) m_listaQText[i] ) ->toPlainText();
                } // end if
            } // end for
        } // end if


        if ( qsWidget == "QTableWidget" ) {
            for ( int i = 0; i < m_maxQTable; i++ ) {
                if ( m_listaQTable[i] != NULL ) {
//                    if ( ( ( QTableWidget* ) m_listaQTable[i] ) ->objectName().startsWith ( "mui_" ) )
//                    {
                      for ( int k = 0; k < ( ( QTableWidget* ) m_listaQTable[i] ) ->rowCount(); k++ ) {
                        for ( int l = 0; l < ( ( QTableWidget* ) m_listaQTable[i] ) ->columnCount(); l++ ) {
                            values += ( ( ( QTableWidget* ) m_listaQTable[i] ) ) ->objectName();
                            values += ( ( ( QTableWidget* ) m_listaQTable[i] ) ) ->item ( k, l ) ->text();
                        } // end for
                      } // end for
//                    } // end if
                } // end if
            } // end for
        } // end if


        if ( qsWidget == "QCheckBox" ) {
            for ( int i = 0; i < m_maxQCheckBox; i++ ) {
                if ( m_listaQCheckBox[i] != NULL ) {
                    if ( ( ( QCheckBox* ) m_listaQCheckBox[i] ) ->objectName().startsWith ( "mui_" ) )
                        values += ( ( QCheckBox* ) m_listaQCheckBox[i] ) ->objectName();

                    /// Tener en cuenta el caso de los checkbox triestado
                    switch ( ( ( QCheckBox* ) m_listaQCheckBox[i] ) ->checkState() ) {
                        case Qt::PartiallyChecked:
                           values += "";
                           break;
                        case Qt::Checked:
                           values += "TRUE";
                           break;
                        case Qt::Unchecked:
                           values += "FALSE";
                           break;
                    } // end switch
                } // end if
            } // end for
        } // end if

        if ( qsWidget == "BlDateSearch" )
        {
          for ( int i = 0; i < m_maxBlDateSearch; i++ )
          {
            if ( m_listaBlDateSearch[i] != NULL )
            {
              if ( ( ( BlDateSearch* ) m_listaBlDateSearch[i] ) ->objectName().startsWith ( "mui_" ) )
              {
                values += ( ( BlDateSearch* ) m_listaBlDateSearch[i] ) ->objectName();

                /// Tener en cuenta el caso de los checkbox triestado
                values += ( ( BlDateSearch* ) m_listaBlDateSearch[i] ) ->text();
              } // end if
            } // end if
          } // end for
        } // end if

        return values;
    }
    catch ( ... )
    {
        return "";
    } // end try
}

