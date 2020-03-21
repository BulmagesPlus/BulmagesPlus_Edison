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

#ifndef BLFORMLIST_H
#define BLFORMLIST_H

#include <QWidget>
#include <QCloseEvent>
#include <QtScript>
#include <QScriptEngine>

#include "bldialogchanges.h"
#include "blfunctions.h"
#include "blmaincompany.h"
#include "blsubform.h"
#include "blwidget.h"


class BL_EXPORT BlFormList : public BlWidget
{
    Q_OBJECT

    BlSubForm *m_listado;

private:
    /// Indica si es modo consulta o modo edicion. (altera el comportamiento del
    /// doble click sobre la lista)
    edmode m_modo;
    QScriptEngine m_myEngine;
    const QString nameFileConfig(); 
    const QString nameFileDefaultConfig(); 
    
protected:
    void setSubForm ( BlSubForm * );

public:
    BlFormList ( QWidget *parent = nullptr, Qt::WindowFlags f = 0, edmode modo = BL_EDIT_MODE );
    BlFormList ( BlMainCompany *emp = NULL, QWidget *parent = nullptr, Qt::WindowFlags f = 0, edmode modo = BL_EDIT_MODE );
    virtual void editar ( int );
    virtual void imprimir();
    virtual void presentar();
    virtual ~BlFormList();
    virtual int removeWindow();
    virtual int insertWindow ( QString title );
    virtual void submenu ( const QPoint & );
    virtual void crear();
    virtual void remove();
    void hideBotonera();
    void showBotonera();
    void hideBusqueda();
    void showBusqueda();
    void setSelectMode();
    void setEditMode();
    bool editMode();
    bool selectMode();
    void trataPermisos ( QString nomtabla );
    BlSubForm *subForm();
    void blScript(QObject * obj);
    void guardaFiltrosXML();
    void cargaFiltrosXML();
    virtual const QString generaFiltro();


public slots:
    virtual void on_mui_list_customContextMenuRequested ( const QPoint & );
    virtual void on_mui_list_cellDoubleClicked ( int, int );
    virtual void on_mui_list_toogledConfig ( bool check );
    virtual void on_m_filtro_textChanged ( const QString &text );
    virtual void on_m_filtro_editingFinished();
    virtual void on_m_filtro_returnPressed();
    virtual void on_mui_actualizar_clicked();
    virtual void on_mui_imprimir_clicked();
    virtual void on_mui_editar_clicked();
    virtual void on_mui_configurar_toggled ( bool checked );
    virtual void on_mui_borrar_clicked();
    virtual void on_mui_crear_clicked();
    virtual void contextMenuEvent ( QContextMenuEvent * );

signals:
    void selected ( QString );
};

#endif

