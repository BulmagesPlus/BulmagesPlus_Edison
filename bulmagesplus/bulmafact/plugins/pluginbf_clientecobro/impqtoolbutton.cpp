/***************************************************************************
 *   Copyright (C) 2006 by Arturo Martin Llado                             *
 *   amartin@conetxia.com                                                  *
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
#include "impqtoolbutton.h"
#include "blfunctions.h"

/// Necesarios para importacion de efactura
#include <QtXml/QDomDocument>
#include <QtXml/QDomNodeList>
#include <QtXml/QDomNode>
#include <QString>
#include <QFileDialog>
#include <QMap>
#include <QList>
#include "facturaview.h"
#include "albaranclienteview.h"
#include "pedidoclienteview.h"
#include "blfixed.h"
#include "bfcompany.h"
#include "bldb.h"
#include "cobroview.h"

///
/**
\param pres
\param ped
\param alb
\param fac
\param parent
**/
GenCobroQToolButton::GenCobroQToolButton ( QWidget *fac , QWidget *parent ) : QToolButton ( parent )
{
    BL_FUNC_DEBUG
    m_object = fac;
    setBoton();
    
}


///
/**
**/
GenCobroQToolButton::~GenCobroQToolButton()
{
    BL_FUNC_DEBUG
    
}


///
/**
**/
void GenCobroQToolButton::setBoton()
{
    BL_FUNC_DEBUG
    connect ( this, SIGNAL ( clicked() ), this, SLOT ( click() ) );
    setObjectName ( QString::fromUtf8 ( "exporta" ) );
    setStatusTip ( "Generar cobro" );
    setToolTip ( "Generar cobro" );
    setMinimumSize ( QSize ( 32, 32 ) );
    setMaximumSize ( QSize ( 32, 32 ) );
    setIcon ( QIcon ( QString::fromUtf8 ( ":/Images/receive.png" ) ) );
    setIconSize ( QSize ( 22, 22 ) );
    setContentsMargins ( 0, 0, 0, 0 );
    
}


///
/**
**/
void GenCobroQToolButton::click()
{
    BL_FUNC_DEBUG

    if ( m_object->objectName() == "FacturaBase" ) {
        FacturaView *fpv = ( FacturaView * ) m_object;
        int nuevo = 1;
        /// Comprobamos que no haya ya un cobro con la misma referencia y lo ponemos
        QString query = "SELECT * FROM cobro WHERE refcobro ='" + fpv->dbValue ( "reffactura" ) + "'";
        BlDbRecordSet *cur = fpv->mainCompany()->loadQuery ( query );
        if ( cur->numregistros() > 0 ) {
            QMessageBox msgBox;
            msgBox.setText ( tr ( "Ya existe un cobro con esta referencia\n" ) );
            msgBox.setInformativeText ( tr ( "Desea abrir el cobro existente, registrar un nuevo cobro o salir?" ) );
            msgBox.addButton ( tr ( "Crear" ), QMessageBox::ActionRole );
            QPushButton *openButton = msgBox.addButton ( QMessageBox::Open );
            QPushButton *abortButton = msgBox.addButton ( QMessageBox::Cancel );
            msgBox.setDefaultButton ( QMessageBox::Cancel );
            msgBox.exec();
            /// Se ha pulsado sobre la opcion abrir
            if ( msgBox.clickedButton() == openButton ) {

                while ( !cur->eof() ) {
                    CobroView *bud = new CobroView ( ( BfCompany * ) fpv->mainCompany(), 0 );
                    fpv->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
                    bud->load ( cur->value( "idcobro" ) );
                    bud->pintar();
                    bud->show();
                    cur->nextRecord();
                } // end while
                nuevo = 0;
            } // end if

            /// Se ha pulsado sobre la opcion cancelar
            if ( msgBox.clickedButton() == abortButton )
                nuevo = 0;
        } // end if
        delete cur;

        /// Creacion de un cobro nuevo a partir de la factura.
        if ( nuevo ) {
            CobroView *bud = new CobroView ( fpv->mainCompany(), 0 );
            fpv->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->setDbValue ( "idcliente", fpv->dbValue ( "idcliente" ) );
            bud->setDbValue ( "cantcobro", fpv->m_totalfactura->text() );
            bud->setDbValue ( "refcobro", fpv->dbValue ( "reffactura" ) );
            bud->setDbValue ( "comentcobro", fpv->dbValue ( "descfactura" ) );
            bud->setDbValue ( "idforma_pago", fpv->dbValue ( "idforma_pago" ) );
            bud->pintar();
            bud->show();
        } // end if

	/// Marcamnos la factura como procesada ya que el cobro es inicialmente por el importe total
	fpv->mui_procesadafactura->setChecked(true);

    }// end if


    if ( m_object->objectName() == "AlbaranClienteBase" ) {
        AlbaranClienteView *fpv = ( AlbaranClienteView * ) m_object;
        int nuevo = 1;
        /// Comprobamos que no haya ya un cobro con la misma referencia y lo ponemos
        QString query = "SELECT * FROM cobro WHERE refcobro ='" + fpv->dbValue ( "refalbaran" ) + "'";
        BlDbRecordSet *cur = fpv->mainCompany()->loadQuery ( query );
        if ( cur->numregistros() > 0 ) {
            QMessageBox msgBox;
            msgBox.setText ( tr ( "Ya existe un cobro con esta referencia\n" ) );
            msgBox.setInformativeText ( tr ( "Desea abrir el cobro existente, registrar un nuevo cobro o salir?" ) );
            msgBox.addButton ( tr ( "Crear" ), QMessageBox::ActionRole );
            QPushButton *openButton = msgBox.addButton ( QMessageBox::Open );
            QPushButton *abortButton = msgBox.addButton ( QMessageBox::Cancel );
            msgBox.setDefaultButton ( QMessageBox::Cancel );
            msgBox.exec();
            /// Se ha pulsado sobre la opcion abrir
            if ( msgBox.clickedButton() == openButton ) {

                while ( !cur->eof() ) {
                    CobroView *bud = new CobroView ( ( BfCompany * ) fpv->mainCompany(), 0 );
                    fpv->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
                    bud->load ( cur->value( "idcobro" ) );
                    bud->pintar();
                    bud->show();
                    cur->nextRecord();
                } // end while
                nuevo = 0;
            } // end if

            /// Se ha pulsado sobre la opcion cancelar
            if ( msgBox.clickedButton() == abortButton )
                nuevo = 0;
        } // end if
        delete cur;

        /// Creacion de un cobro nuevo a partir del albaran
        if ( nuevo ) {
            CobroView *bud = new CobroView ( ( BfCompany * ) fpv->mainCompany(), 0 );
            fpv->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->setDbValue ( "idcliente", fpv->dbValue ( "idcliente" ) );
            bud->setDbValue ( "idforma_pago", fpv->dbValue ( "idforma_pago" ) );
            bud->setDbValue ( "cantcobro", fpv->m_totalalbaran->text() );
            bud->setDbValue ( "refcobro", fpv->dbValue ( "refalbaran" ) );
            bud->setDbValue ( "comentcobro", fpv->dbValue ( "descalbaran" ) );
            bud->pintar();
            bud->show();
        } // end if
    }// end if


    if ( m_object->objectName() == "PedidoClienteBase" ) {
        PedidoClienteView *fpv = ( PedidoClienteView * ) m_object;
        int nuevo = 1;
        /// Comprobamos que no haya ya un cobro con la misma referencia y lo ponemos
        QString query = "SELECT * FROM cobro WHERE refcobro ='" + fpv->dbValue ( "refpedidocliente" ) + "'";
        BlDbRecordSet *cur = fpv->mainCompany()->loadQuery ( query );
        if ( cur->numregistros() > 0 ) {
            QMessageBox msgBox;
            msgBox.setText ( tr ( "Ya existe un cobro con esta referencia\n" ) );
            msgBox.setInformativeText ( tr ( "Desea abrir el cobro existente, registrar un nuevo cobro o salir?" ) );
            msgBox.addButton ( tr ( "Crear" ), QMessageBox::ActionRole );
            QPushButton *openButton = msgBox.addButton ( QMessageBox::Open );
            QPushButton *abortButton = msgBox.addButton ( QMessageBox::Cancel );
            msgBox.setDefaultButton ( QMessageBox::Cancel );
            msgBox.exec();
            /// Se ha pulsado sobre la opcion abrir
            if ( msgBox.clickedButton() == openButton ) {

                while ( !cur->eof() ) {
                    CobroView *bud = new CobroView ( ( BfCompany * ) fpv->mainCompany(), 0 );
                    fpv->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
                    bud->load ( cur->value( "idcobro" ) );
                    bud->pintar();
                    bud->show();
                    cur->nextRecord();
                } // end while
                nuevo = 0;
            } // end if

            /// Se ha pulsado sobre la opcion cancelar
            if ( msgBox.clickedButton() == abortButton )
                nuevo = 0;
        } // end if
        delete cur;

        /// Creacion de un cobro nuevo a partir de la factura.
        if ( nuevo ) {
            CobroView *bud = new CobroView ( ( BfCompany * ) fpv->mainCompany(), 0 );
            fpv->mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->setDbValue ( "idcliente", fpv->dbValue ( "idcliente" ) );
            bud->setDbValue ( "idforma_pago", fpv->dbValue ( "idforma_pago" ) );
            bud->setDbValue ( "cantcobro", fpv->m_totalpedidocliente->text() );
            bud->setDbValue ( "refcobro", fpv->dbValue ( "refpedidocliente" ) );
            bud->setDbValue ( "comentcobro", fpv->dbValue ( "descpedidocliente" ) );
            bud->pintar();
            bud->show();
        } // end if
    }// end if

    
}


