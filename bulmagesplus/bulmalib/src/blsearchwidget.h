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

#ifndef BLSEARCHWIDGET_H
#define BLSEARCHWIDGET_H

#include <QLineEdit>
#include <QLabel>

#include "blfunctions.h"
#include "blwidget.h"
#include "blcombobox.h"
#include "ui_blsearchwidgetbase.h"


/// Permite buscar y seleccionar un profesor.
/** Implementa un Widget que permite buscar y seleccionar un
    profesor de BulmaFact de forma generica. */
class BL_EXPORT BlSearchWidget : public BlWidget, public Ui_BlSearchWidgetBase
{
    Q_OBJECT

public:
    /// QHash de los valores a comprobar
    QMap <QString, QString> m_valores;

private:
    /// Almacena la tabla sobre la que vamos a buscar.
    QString m_tabla;
    QString m_campoid;
    /// Almacena el id de la tabla seleccionada.
    QString mdb_id;
    /// Impide que se produzca un dead-lock entre pintar y on_mui_text_changed.
    bool m_semaforo;
    /// Es la mascara de presentacion, indica en que forma se presentan los
    /// Campos en el cuadro de texto inferior. Si no esta establecido entonces no se
    /// utiliza
    QString m_mask;
    bool m_canCreate = false;
    bool m_encontrado = false;

public:
    explicit BlSearchWidget(QWidget *parent = nullptr);
    ~BlSearchWidget();
    void pinta();
    virtual QString id() const;
    virtual QString fieldValue() const override;
    virtual QString fieldValue(QString fieldName) const;
    virtual QString tableName() const;
    virtual void setId ( QString val, bool cargarvalores = true );
    virtual void setFieldValue(QString value) override;
    virtual void setFieldValue(QString fieldName, QString value);
    virtual void setLabel(QString label);
    virtual void setTableName(QString tableName);
    virtual void setText(const QString &);
    void setMask(const QString &);
    QString mask() const;
    QString text() const;
    void setFieldId ( const QString & );
    QString fieldId();
    void selectAll();
    void setFocus();
    void hideLabel();
    void hideDescripcion();
    void hideBotonBorrar();
    void hideBotonBusqueda();
    void setCanCreate(bool canCreate);
    bool canCreate();


public slots:
    virtual void on_m_buscarWidget_clicked();
    virtual void on_m_clearSearchWidget_clicked();
    virtual void on_m_inputBusqueda_textChanged ( const QString & );
    virtual void on_m_inputBusqueda_editingFinished();
    virtual void on_mui_create_clicked();

signals:
    void valueChanged(QString);
};


#endif

