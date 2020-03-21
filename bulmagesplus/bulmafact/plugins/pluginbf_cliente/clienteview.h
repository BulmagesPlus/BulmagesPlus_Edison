/***************************************************************************
 *   Copyright (C) 2004 by Marcos Mezo                                     *
 *   mmezo@selexco.net                                                     *
 *   http://www.iglues.org                                                 *
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

#ifndef CLIENTEVIEW_H
#define CLIENTEVIEW_H

#include "bfform.h"
#include "ui_clientebase.h"
#include "pdefs_pluginbf_cliente.h"

/** Clase que se encarga de la pantalla de cliente.
    Deriva de la clase BfForm para la estandarizacion de pantallas y
    de la clase Cliente para el manejo con la base de datos.
    Se encarga de recibir y lanzar eventos.
*/
class PLUGINBF_CLIENTE_EXPORT ClienteView : public BfForm, public Ui_ClienteBase
{
    Q_OBJECT

public:
    ClienteView ( BfCompany *emp, QWidget *parent = 0 );
    ~ClienteView();

public:
    int cargarPost ( QString client );
    void activateDocuments();
    void deactivateDocuments();
    virtual int beforeSave();
    virtual int afterSave();

signals:
    void activatedDocuments(ClienteView *, bool enabled);
    void deactivatedDocuments(ClienteView *, bool enabled);

public slots:
    /// \TODO: Muchos metodos aqui implementados deberian estar en Ficha.
    virtual void on_mui_informe_clicked();
    virtual void on_mui_cifcliente_lostFocus();
};

#endif

