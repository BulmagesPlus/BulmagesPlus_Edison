/***************************************************************************
 *   Copyright (C) 2007 by Fco. Javier Miró Carmona.                       *
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

#ifndef BLCOUNTRYVIEW_H
#define BLCOUNTRYVIEW_H

#include "blform.h"
#include "bldefs.h"
#include "ui_blcountrybase.h"

QT_FORWARD_DECLARE_CLASS(QString);
QT_FORWARD_DECLARE_CLASS(QWidget);
QT_FORWARD_DECLARE_CLASS(QTableWidgetItem);

class BlMainCompany;


class BL_EXPORT BlCountryView : public BlForm, public Ui_BlCountryBase
{
  Q_OBJECT

  public:
    QString m_countryId; /// Indica cuál es el país que se está visualizando.

  public:
    explicit BlCountryView(BlMainCompany * company, QWidget * parent = nullptr);
    ~BlCountryView();
    void pintar() override;
    void mostrarplantilla();
    virtual void on_mui_borrar_clicked() override;
    virtual int save() override;

  public slots:
    virtual void on_mui_crear_clicked();
    virtual void on_mui_list_itemClicked(QTableWidgetItem * item);
};

#endif
