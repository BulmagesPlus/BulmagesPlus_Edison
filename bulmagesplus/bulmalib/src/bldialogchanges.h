/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borrás Riera.                             *
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

#ifndef BLDIALOGCHANGES_H
#define BLDIALOGCHANGES_H

#include <QObject>

#include "blfunctions.h"


/// Esta clase se encarga de proporcionar algo de infraestructura al control de cambios.
/// La idea original es de Alvaro de Miguel en la que lo que hace la funcion es
/// coger todos los campos de un determinado formulario y formar una cadena.
/// Luego para saber si hay cambios en el formulario vuelve a comparar.
class BL_EXPORT BlDialogChanges
{

private:
    // M&aacute;xima cantidad de objetos a los que se les pueden detectar cambios
    static const int max_quantity = 999;

    int m_maxQLine = 0;
    int m_maxQPlainText = 0;
    int m_maxQText = 0;
    int m_maxQTable = 0;
    int m_maxQComboBox = 0;
    int m_maxQCheckBox = 0;
    int m_maxBlDateSearch = 0;
    QString m_valorinicial;
    QObject *m_obje;
    QObject *m_listaQLine[max_quantity] = {};
    QObject *m_listaQPlainText[max_quantity] = {};
    QObject *m_listaQText[max_quantity] = {};
    QObject *m_listaQTable[max_quantity] = {};
    QObject *m_listaQComboBox[max_quantity] = {};
    QObject *m_listaQCheckBox[max_quantity] = {};
    QObject *m_listaBlDateSearch[max_quantity] = {};
    QList<QObject*> m_listaExcluidos;

public:
    explicit BlDialogChanges(QObject *ob);
    ~BlDialogChanges();
    void dialogChanges_readValues();
    bool dialogChanges_isChanged();
    void dialogChanges_setExcludedObject(QObject *objetoexcluido);
    bool dialogChanges_isObjectExcluded(QObject *item);

private:
    QString calculateValues();
    QString retrieveValues(QString qsWidget);
};

#endif

