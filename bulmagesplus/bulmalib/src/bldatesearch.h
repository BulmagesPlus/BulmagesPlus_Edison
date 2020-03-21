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

#ifndef BLDATESEARCH_H
#define BLDATESEARCH_H

#include "blwidget.h"
#include "bldefs.h"
#include "ui_bldatesearchbase.h"

QT_FORWARD_DECLARE_CLASS(QWidget);
QT_FORWARD_DECLARE_CLASS(QString);
QT_FORWARD_DECLARE_CLASS(QDate);


class BL_EXPORT BlDateSearch : public BlWidget, public Ui_BlDateSearchBase
{
  Q_OBJECT

  public:
    explicit BlDateSearch(QWidget * parent = nullptr);
    ~BlDateSearch();
    virtual void setDate(QString date);
    virtual void setText(QString text);
    virtual void setFieldValue(QString fieldValue) override;
    virtual QString fecha();
    virtual QString text();
    virtual QString fieldValue();
    virtual QDate date() const;

  public slots:
    virtual void s_searchFecha();
    virtual void s_fechatextChanged(const QString &);
    virtual void s_fechalostFocus();
    virtual void s_returnPressed();
    virtual void selectAll();

  signals:
    /// Esta señal se emite siempre que el valor de la fecha sea modificada,
    /// ya sea programadamente (setText) o por el usuario.
    void valueChanged(QString);
    void returnPressed();

    /// Esta señal se emite cuando el usuario cambia la fecha ya sea
    /// escribiendo o seleccionando en el calendario.
    /// No se activará si el cambio se realizó programadamente.
    void editingFinished();
};

#endif
