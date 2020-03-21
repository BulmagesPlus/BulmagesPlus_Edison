/***************************************************************************
 *   Copyright (C) 2007 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2010 by Rubén Cabezas Sigüenza. (QScript support).      *
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
//#include <QTextDocument>

#include "blformconfig.h"


///  Inicializa el objeto y hace todas las conexiones necesarias.
/**
\param emp
\param parent
\param f
**/
BlFormConfig::BlFormConfig ( BlMainCompany *emp, BlForm *parent, Qt::WindowFlags f ) : BlWidget ( emp, parent, f )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    setAttribute ( Qt::WA_DeleteOnClose );
    m_BlForm = parent;
    setGeometry ( 0, 0, m_BlForm->width(), m_BlForm->height() );
    showMaximized();
    mui_tablename->setText ( m_BlForm->tableName() );

    /// Listamos los atributos de la tabla.
    QString fields = "SELECT a.attnum AS ordinal_position, a.attname AS column_name, t.typname AS data_type, a.attlen AS character_maximum_length, a.atttypmod AS modifier, a.attnotnull AS notnull, a.atthasdef AS hasdefault FROM pg_class c, pg_attribute a, pg_type t WHERE c.relname = '" + m_BlForm->tableName() + "' AND a.attnum > 0 AND a.attrelid = c.oid AND a.atttypid = t.oid ORDER BY a.attnum;";
    BlDbRecordSet *cur = mainCompany() ->loadQuery ( fields );

    mui_databasetable->setRowCount ( cur->numregistros() );
    mui_databasetable->setColumnCount ( cur->numcampos() );
    int i = 0;
    while ( !cur->eof() ) {
        for ( int j = 0 ; j < cur->numcampos() ; j++ ) {
            QTableWidgetItem *newItem = new QTableWidgetItem ( cur->value( j, i ) );
            mui_databasetable->setItem ( i, j, newItem );
        } // end for
        cur->nextRecord();
        i++;
    } // end while
    delete cur;

    /// Miramos los temas de BlDbRecord.
    mui_formtablename->setText ( m_BlForm->tableName() );
    mui_formcampoindice->setText ( m_BlForm->fieldId() );

    QList<BlDbField *> *lista = m_BlForm->lista();

    mui_formelements->setRowCount ( lista->size() );
    mui_formelements->setColumnCount ( 3 );

    /// Impresion de la tabla de contenidos.
    for ( int j = 0; j < lista->size(); ++j ) {
        BlDbField *campo = lista->at ( j );
        QTableWidgetItem *newItem = new QTableWidgetItem ( campo->fieldName() );
        mui_formelements->setItem ( j, 0, newItem );

        QTableWidgetItem *newItem1 = new QTableWidgetItem ( campo->fieldTitle() );
        mui_formelements->setItem ( j, 1, newItem1 );

        QTableWidgetItem *newItem2 = new QTableWidgetItem ( campo->fieldValue() );
        mui_formelements->setItem ( j, 2, newItem2 );
    } // end for

    /// Privilegios de usuario.
    QString queryusers = "SELECT usename, has_table_privilege(pg_user.usename, '" + m_BlForm->tableName() + "', 'SELECT') AS pselect" \
                         ", has_table_privilege(pg_user.usename, '" + m_BlForm->tableName() + "', 'INSERT') AS pinsert" \
                         ", has_table_privilege(pg_user.usename, '" + m_BlForm->tableName() + "', 'UPDATE') AS pupdate" \
                         ", has_table_privilege(pg_user.usename, '" + m_BlForm->tableName() + "', 'RULE') AS prules  FROM pg_user";
    cur = mainCompany() ->loadQuery ( queryusers );

    mui_usertable->setRowCount ( cur->numregistros() );
    mui_usertable->setColumnCount ( cur->numcampos() );
    i = 0;
    while ( !cur->eof() ) {
        for ( int j = 0 ; j < cur->numcampos() ; j++ ) {
            QTableWidgetItem *newItem = new QTableWidgetItem ( cur->value( j, i ) );
            mui_usertable->setItem ( i, j, newItem );
        } // end for
        cur->nextRecord();
        i++;
    } // end while
    delete cur;

    cur = mainCompany() ->loadQuery ( "SELECT user" );
    if ( !cur ) return;

    mui_currentuser->setText ( cur->value( "current_user" ) );
    delete cur;
    
    archivoScript->setText(g_confpr->value( CONF_DIR_OPENREPORTS ) + "blform_"+parent->metaObject()->className()+".qs");
    
    if (!QFile::exists(g_confpr->value( CONF_DIR_OPENREPORTS ) + "blform_"+parent->metaObject()->className()+".qs")) {
      editarScript->setText(_("Crear"));
    } // end if
  
    /// Rellenamos los campos de DatabaseScript y QtScript con los algoritmos que encontramos.
    //    QString queryTriggers = "select tgname, prosrc from pg_trigger,pg_proc where  pg_proc.oid=pg_trigger.tgfoid  and pg_trigger.tgname LIKE '%"+m_BlForm->tableName()+"%'";
    QString queryTriggers = "select proname, prosrc from pg_proc where proname LIKE '%"+m_BlForm->tableName()+"%'";
    
    cur = mainCompany() ->loadQuery ( queryTriggers );
    QString texto = "";
    while ( !cur->eof() ) {
	texto += cur->value("proname") + " \n==================================\n";
	texto += cur->value("prosrc") + "\n\n==================================\n\n";
        cur->nextRecord();
    } // end while
    delete cur;
    databaseScript->setPlainText(texto);
    
    
}

void BlFormConfig::on_editarScript_released() {
  QString cadena = "touch " +g_confpr->value( CONF_DIR_OPENREPORTS ) + "blform_"+parent()->metaObject()->className()+".qs";
  system ( cadena.toLatin1().data() );
  cadena = g_confpr->value(CONF_EDITOR) +" "+ g_confpr->value( CONF_DIR_OPENREPORTS ) + "blform_"+parent()->metaObject()->className()+".qs &";
  int result = system ( cadena.toLatin1().data() );
}

/// No requiere de acciones especiales en el destructor.
/**
**/
BlFormConfig::~BlFormConfig()
{
    BL_FUNC_DEBUG
    
}

