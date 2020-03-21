/***************************************************************************
 *   Copyright (C) 2006 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
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

#ifndef BFFORM_H
#define BFFORM_H

#include <QHash>
#include <QPair>
#include "blform.h"
#include "bfsubform.h"
#include "bfcompany.h"
#include "blfixed.h"


/// Una factura puede tener multiples bases imponibles. Por eso definimos el tipo base
/// como un QMap.
typedef QMap<QString, BlFixed> Base;


class BF_EXPORT BfForm : public BlForm
{
    Q_OBJECT

private:
    //BlFixed m_totalIvaAdicional;
    //QMultiHash <QString, QHash<QString, QString> > m_multiHashIvaAdicional;//
    
public:
    BfSubForm *m_listalineas;
    BfSubForm *m_listadescuentos;

public:
    BfCompany *mainCompany();
    explicit BfForm(BfCompany *company, QWidget *parent = 0, Qt::WindowFlags f = 0, edmode modo = BL_EDIT_MODE);
    virtual ~BfForm();
    
    //QMultiHash <QString, QHash<QString, QString> > getTotalIvaAdicional ( ); // 0
    //void setTotalIvaAdicional ( QHash<QString, QString> ); // 1
    
    
    
    /// Logica de IVA adicional.
private:
    QHash <QString, QList<QPair<BlFixed, BlFixed> > > m_ivaAdicional;
public:

    void setIvaAdicional(QString origen, BlFixed porcentajeIVA, BlFixed ivaAdicional);
    BlFixed ivaAdicional(BlFixed porcentajeIVA);
    void deleteIvaAdicional(QString origen, BlFixed porcentajeIVA);
    void testIVA();
    void imprimeIvaAdicional();
    /// ------------------- ///
    
    void setListaLineas ( BfSubForm *form );
    void setListaDescuentos ( BfSubForm *form );
    BfSubForm* getlistalineas();
    BfSubForm* getlistadescuentos();
    virtual void imprimir();
    virtual QString templateName(void) ;
    virtual int generateRML ( void );
    virtual int generateRML ( const QString &arch );
    virtual void calculaypintatotales();
    virtual void pintatotales ( BlFixed, BlFixed, BlFixed, BlFixed, BlFixed, BlFixed ) {};
    virtual int parseTags ( QString &buff, int tipoEscape = 0 );
    virtual void parseTagsBf( QString &buff, int tipoEscape = 0 );
    virtual QString trataLineasDetalle ( const QString &det, int tipoEscape = 0 );
    virtual QString trataLineasDescuento ( const QString &det, int tipoEscape = 0 );
    virtual QString trataTotales ( const QString &det, int bimporeq = 1 );
    virtual void recogeValores();
};

#endif
