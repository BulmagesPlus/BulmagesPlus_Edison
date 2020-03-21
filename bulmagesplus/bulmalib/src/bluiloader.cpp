/***************************************************************************
 *   Copyright (C) 2006 by Tomeu Borrás Riera.                             *
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

#include <QWidget>
#include "bluiloader.h"
#include "blcombobox.h"
#include "blsubform.h"
#include "blsearchwidget.h"
#include "bldatesearch.h"


BlUiLoader::BlUiLoader ( BlMainCompany *company,  QObject *parent ) : QUiLoader(parent) 
{
    BL_FUNC_DEBUG
    m_company = company;
}


BlUiLoader::~BlUiLoader ()
{
    BL_FUNC_DEBUG
}


QWidget *BlUiLoader::createWidget ( const QString &className, QWidget *parent, const QString &name )
{
    BL_FUNC_DEBUG
    QWidget *widget = NULL;
    
    if (className == "BlComboBox") {
	widget = ((QWidget *) new BlComboBox(parent));
	((BlComboBox *) widget)->setMainCompany(m_company);
	widget->setObjectName(name);

	// Para no liarla parda, ponemos provincias y asi no habr&aacute; pedatas de momento.
        ((BlComboBox *) widget)->setQuery ( "SELECT * FROM provincia LEFT JOIN pais ON provincia.idpais = pais.idpais ORDER BY descpais, provincia" );
        ((BlComboBox *) widget)->setTableName ( "provincia" );
        ((BlComboBox *) widget)->setFieldId ( "idprovincia" );
        ((BlComboBox *) widget)->m_valores["provincia"] = "";
        ((BlComboBox *) widget)->m_valores["descpais"] = "";
        ((BlComboBox *) widget)->setAllowNull(true);
        ((BlComboBox *) widget)->setId ( "" );
    } // end if
    
    if (className == "BfSubForm" || className== "BlSubForm" || className == "BcSubForm") {
      BlSubForm *subForm = new BlSubForm(parent);
      subForm->setMainCompany(m_company);
      subForm->setObjectName(name);
      widget = (QWidget *) subForm;
    } // end if

    if (className == "BlSearchWidget") {
      BlSearchWidget *search = new BlSearchWidget(parent);
      search->setMainCompany(m_company);
      search->setObjectName(name);
      widget = (QWidget *) search;
    } // end if

    if (className == "BlDateSearch") {
      BlDateSearch *dateSearch = new BlDateSearch(parent);
      dateSearch->setObjectName(name);
      widget = (QWidget *) dateSearch;
    } // end if

    if (!widget) {
      widget =  QUiLoader::createWidget( className, parent, name );
    } // end if
    
    return widget;
}


