/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2010 by Aron Galdón.                                    *
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

#include <QKeyEvent>
#include <QEvent>
#include <QFile>
#include <QHeaderView>
#include <QTextStream>
#include <QMenu>
#include <QShortcut>
#include <QLocale>
#include <QRegExp>
#include <QDomDocument>
#include <QDomNode>
#include <QSpinBox>
#include <QStringList>
#include <QDir>
#include <QDebug>

#include "blsubform.h"
#include "blprogressbar.h"
#include "bldoublespinbox.h"
#include "bltexteditdelegate.h"
#include "blupgradepreview.h"


// Necesito exportar algunos datos.
QModelIndex BL_EXPORT g_index;
QWidget BL_EXPORT *g_editor;
QAbstractItemModel BL_EXPORT *g_model;
QString BL_EXPORT g_fieldName;


/// Construye una columna de la descripcion del recordset
/**
\param nom Nombre de la columna
\param typ Tipo de los datos que alberga
\param res Restricciones de la columna
\param opt Opciones de presentacion
\param nomp Nombre a presentar en caso necesario para referirse a la columna
**/
BlSubFormHeader::BlSubFormHeader ( QString nom, BlDbField::DbType typ, int res, int options, QString nomp )
{
    BL_FUNC_DEBUG
    m_fieldName = nom;
    m_tipo = typ;
    m_fieldRestrictions = res;
    m_options = options;
    m_fieldTitle = nomp;
    m_numericPrecision = 2;
    
}


///
/**
**/
BlSubFormHeader::~BlSubFormHeader()
{
    BL_FUNC_DEBUG
    
}


void BlSubFormHeader::setOptions(int options)
{
    BL_FUNC_DEBUG
    m_options = options;
}


///
/**
\return
**/
unsigned int BlSubFormHeader::options()
{
    BL_FUNC_DEBUG
    
    return m_options;
}


///
/**
\return
**/
unsigned int BlSubFormHeader::restrictions()
{
    BL_FUNC_DEBUG
    
    return m_fieldRestrictions;
}


///
/**
\return
**/
BlDbField::DbType BlSubFormHeader::fieldType()
{
    BL_FUNC_DEBUG
    
    return m_tipo;

}


///
/**
\return
**/
QString BlSubFormHeader::fieldTitle()
{
    BL_FUNC_DEBUG
    
    return m_fieldTitle;
}


///
/**
\return
**/
int BlSubFormHeader::fieldRestrictions()
{
    BL_FUNC_DEBUG
    
    return m_fieldRestrictions;
}


///
/**
\return
**/
QString BlSubFormHeader::fieldName()
{
    BL_FUNC_DEBUG
    
    return m_fieldName;
}


/// Hace la exportacion del campo a XML
QString BlSubFormHeader::exportXML() {
    BL_FUNC_DEBUG
    QString val;
    int error;

    val = "<BLSUBFORMHEADER>\n";
    val += "\t<NOMCAMPO>" + blXMLEncode(m_fieldName) + "</NOMCAMPO>\n";
    val += "\t<NOMPRESENTACION>"+ blXMLEncode(m_fieldTitle)+"</NOMPRESENTACION>\n";
    val += "\t<RESTRICCIONES>"+ blXMLEncode(QString::number(m_fieldRestrictions))+"</RESTRICCIONES>\n";
    val += "\t<TIPO>"+ blXMLEncode(QString::number(m_tipo))+"</TIPO>\n";
    val += "\t<OPCIONES>"+ blXMLEncode(QString::number(m_options))+"</OPCIONES>\n";
    val += "</BLSUBFORMHEADER>\n";
    return val;
    
}



/// BlSubForm, constructor de la clase base para subformularios.
/**
\param parent
**/
BlSubForm::BlSubForm ( QWidget *parent ) : BlWidget ( parent )
{
    BL_FUNC_DEBUG
    setupUi ( this );

    /// Por defecto es modo edicion
    m_modo = BL_EDIT_MODE;

    /// Inicializamos los valores de columna y fila previas para que no tengan un valor indefinido.
    /// Se inicializan con -1 para considerar que no hay celda previa.
    m_prevCol = -1;
    m_prevRow = -1;

    /// Desactivamos el procesado de cambios.
    m_procesacambios = false;

    m_textoceldaParaRowSpan = "";
    m_filaInicialRowSpan = -1;

    /// Disparamos los plugins.
    int res = g_plugins->run ( "BlSubForm_BlSubForm", this );
    if ( res != 0 ) {
        m_procesacambios = true;
        return;
    } // end if

    mui_list->setSelectionMode ( QAbstractItemView::SingleSelection );
    mui_list->setSelectionBehavior ( QAbstractItemView::SelectRows );
    mui_list->setSortingEnabled ( false );
    /// \TODO Hay un Bug que impide ordenar bien los elementos.
    mui_list->horizontalHeader()->setSectionsMovable ( true );
    /// Valor por defecto en todos los listados.
    setListadoPijama ( true );
    QPalette p;
    p = mui_list->palette();
    m_colorfondo1 = p.color ( QPalette::Normal, QPalette::Base );
    m_colorfondo2 = p.color ( QPalette::Normal, QPalette::AlternateBase );


    QHeaderView *hh = mui_list->horizontalHeader();

    /// Para conocer cuando el usuario altera el orden de las columnas.
    connect ( ( QObject * ) hh, SIGNAL ( sectionMoved ( int, int, int ) ), this, SLOT ( columnMovedByUser ( int, int, int ) ) );


    /// Capturamos la secuencia de teclas para hacer aparecer o desaparecer
    /// el panel de configuracion del BlSubForm.
    QShortcut *shortcut = new QShortcut ( QKeySequence ( "Ctrl+B" ), this );
    connect ( shortcut, SIGNAL ( activated() ), this, SLOT ( toogleConfig() ) );

    m_insercion = false;
    m_primero = true;
    m_sorting = false;
    m_orden = false;
    m_ordenporquery = false;

    /// Para el listado de columnas hacemos una inicializacion.
    QStringList headers;
    headers << _ ( "Nombre de campo" ) << _ ( "Nombre" ) << _ ( "Visible" ) << _ ( "Orden" );
    mui_listcolumnas->setColumnCount ( 4 );
    mui_listcolumnas->setHorizontalHeaderLabels ( headers );
    mui_listcolumnas->setShowGrid ( false );
    mui_listcolumnas->setSelectionBehavior ( QAbstractItemView::SelectRows );
    mui_listcolumnas->verticalHeader() ->hide();
    mui_listcolumnas->setEditTriggers ( QAbstractItemView::NoEditTriggers );
    mui_listcolumnas->horizontalHeader() ->setSectionResizeMode ( 0, QHeaderView::ResizeToContents );
    mui_listcolumnas->setColumnWidth ( 0, 175 );
    mui_listcolumnas->setColumnWidth ( 1, 100 );
    mui_listcolumnas->setColumnWidth ( 2, 0 );
    mui_listcolumnas->setColumnWidth ( 3, 0 );
    mui_listcolumnas->hideColumn ( 2 );
    mui_listcolumnas->hideColumn ( 3 );

    /// Siempre que arrancamos mostramos la pagina 0.
    mui_paginaact->setValue ( 1 );
    /// Ocultamos la configuracion.
    hideConfig();
    mui_menusubform->setVisible(false);
    /// Limpiamos la lista.
    m_lista.clear();
    m_listaborrar.clear();

    /// Inicializamos los valores de columna y fila previas para que no tengan un valor indefinido.
    /// Se inicializan con -1 para considerar que no hay celda previa.
    m_prevCol = -1;
    m_prevRow = -1;
    setDelete ( true );
    
    /// Disparamos los plugins.
    g_plugins->run ( "BlSubForm_BlSubForm_Post", this );
    BlUpgradePreview *m_upgrade = new BlUpgradePreview;
    m_upgrade->BlSubForm_Post(this);
    m_procesacambios = true;
    
   if (g_confpr->value(CONF_MODO_EXPERTO) != "TRUE") {
     mui_selectorblsubform->removeTab(mui_selectorblsubform->indexOf(tab3));
   } // end if
   
   
}


void BlSubForm::columnMovedByUser ( int, int oldIndex, int newIndex )
{
    BL_FUNC_DEBUG
    mui_listcolumnas->moveRow ( oldIndex, newIndex );
}


/// Destructor de Clase que guarda la configuracion.
/**
**/
BlSubForm::~BlSubForm()
{
    BL_FUNC_DEBUG
    /// PAra destruir desactivamos el control de cambios.
    m_procesacambios = false;
    saveConfig();
    /// Liberamos memoria.
    while (!m_lista.isEmpty()) {
	delete m_lista.takeFirst();
    } // end while
    while (!m_lcabecera.isEmpty()) {
	delete m_lcabecera.takeFirst();
    } // end while
    while (!m_listaborrar.isEmpty()) {
	delete m_listaborrar.takeFirst();
    } // end while
}


///
/**
\param emp
**/
void BlSubForm::setMainCompany ( BlMainCompany *emp )
{
    BL_FUNC_DEBUG
    BlMainCompanyPointer::setMainCompany ( emp );


    if ( tableName() != "" && mainCompany() != NULL ) {
      /// Averiguamos de los campos que ya estan introducidos la precision de los de tipo numerico.
      BlSubFormHeader *linea;
      for ( int i = 0; i < m_lcabecera.size(); ++i ) {
          linea = m_lcabecera.at ( i );

          /// Comprobamos la precision del campo (si este es del tipo numerico) y la guardamos.
          /// Lo hacemos aqui y no en el constructor de BlDbField porque precisamos del nombre de la tabla para
          /// poder hacer la consulta sobre la precisión del campo y ese dato solo esta en BlDbRecord.
          /// Buscamos los decimales que tiene el campo y establecemos el numero de decimales a ese valor.
          /// NOTA: En caso de que no haya sido posible calcular la precisión esta se establece por defecto como 2.
          if (linea->fieldType() == BlDbField::DbNumeric ) {

                QString query2 = "SELECT numeric_scale FROM information_schema.columns WHERE table_name = '" + tableName() + "' and column_name = '" + linea->fieldName() + "';";
                BlDbRecordSet *cur = mainCompany() ->loadQuery ( query2 );
                if ( !cur->eof() ) {
                    linea->setNumericPrecision(cur->value("numeric_scale").toInt());
                } // end if
                delete cur;
          } // end if
      } // end for
    } // end if

    loadSpecs();
    
}


///
/**
**/
void BlSubForm::loadSpecs()
{
    BL_FUNC_DEBUG
    
    QString fileToLoad = CONFIG_DIR_CONFIG + m_fileconfig + "_" + mainCompany() ->dbName() + "_specs.spc";
    BlDebug::blDebug("BlSubForm::loadSpecs", 0, fileToLoad);
    
    QFile file ( fileToLoad );
    
    /// If not exists return.
    if ( !file.open ( QIODevice::ReadOnly ) ) {

	BlDebug::blDebug("BlSubForm::loadSpecs", 0, "File NOT found.");
        return;

    } else {

	BlDebug::blDebug("BlSubForm::loadSpecs", 0, "File found.");
    } // end if

    QDomDocument doc ( "mydocument" );
    

    QString errorMsg;
    if ( !doc.setContent ( &file, &errorMsg ) ) {
        file.close();
        
        BlDebug::blDebug("BlSubForm::loadSpecs", 0, QString("Error: ") + errorMsg);
        
        return;
    } // end if
    
    file.close();

    QDomElement docElem = doc.documentElement();
    QDomElement principal = docElem.firstChildElement ( "SUBFORM" );
    /// Cogemos la coordenada X
    QString tablename = principal.firstChildElement ( "TABLENAME" ).toElement().text();
    QString campoid = principal.firstChildElement ( "CAMPOID" ).toElement().text();

    QDomNodeList nodos = docElem.elementsByTagName ( "HEADER" );
    for ( int i = 0; i < nodos.count(); i++ ) {
        QDomNode ventana = nodos.item ( i );
        QDomElement e1 = ventana.toElement(); /// try to convert the node to an element.
        if ( !e1.isNull() ) { /// the node was really an element.
            BlDbField::DbType type = BlDbField::DbVarChar;
            QString nomheader = e1.firstChildElement ( "NOMHEADER" ).toElement().text();
            
            if (existsHeader(nomheader)) {
	        BlDebug::blDebug("BlSubForm::loadSpecs", 0, QString("Error: NOMHEADER = '") + nomheader + QString("' exists. Doing nothing!"));
        	continue;
            } // end if
            
            QString nompheader = e1.firstChildElement ( "NOMPHEADER" ).toElement().text();
            QString typeheader = e1.firstChildElement ( "DBTYPEHEADER" ).toElement().text();
            if ( typeheader == "DBVARCHAR" ) {
                type = BlDbField::DbVarChar;
            } else if ( typeheader == "DBINT" ) {
                type = BlDbField::DbInt;
            } else if ( typeheader == "DBNUMERIC" ) {
                type = BlDbField::DbNumeric;
            } else if ( typeheader == "DBBOOLEAN" ) {
                type = BlDbField::DbBoolean;
            } else if ( typeheader == "DBDATE" ) {
                type = BlDbField::DbDate;
            } else if ( typeheader == "DBTIME" ) {
                type = BlDbField::DbTime;
            } // end if

            int restricciones = ( int ) BlDbField::DbNothing;
            QDomElement restrict = e1.firstChildElement ( "RESTRICTIONSHEADER" );
            while ( !restrict.isNull() ) {
                QString trestrict = restrict.text();
                if ( trestrict == "DBNOTHING" ) {
                    restricciones |= BlDbField::DbNothing;
                } else if ( trestrict == "DBNOTNULL" ) {
                    restricciones |= BlDbField::DbNotNull;
                } else if ( trestrict == "DBPRIMARYKEY" ) {
                    restricciones |= BlDbField::DbPrimaryKey;
                } else if ( trestrict == "DBNOSAVE" ) {
                    restricciones |= BlDbField::DbNoSave;
                } else if ( trestrict == "DBAUTO" ) {
                    restricciones |= BlDbField::DbAuto;
                } else if ( trestrict == "DBDUPPRIMARYKEY" ) {
                    restricciones |= BlDbField::DbDupPrimaryKey;
                } else if ( trestrict == "DBREQUIRED" ) {
                    restricciones |= BlDbField::DbRequired;
                } else if ( trestrict == "DBNOLOAD" ) {
                    restricciones |= BlDbField::DbNoLoad;
                } // end if
                restrict = restrict.nextSiblingElement ( "RESTRICTIONSHEADER" );
            } // end while

            int opciones = ( int ) BlSubFormHeader::DbNone;
            QDomElement opci = e1.firstChildElement ( "OPTIONSHEADER" );
            while ( !opci.isNull() ) {
                QString topci = opci.text();
                if ( topci == "DBNONE" ) {
                    opciones |= BlSubFormHeader::DbNone;
                } else if ( topci == "DBREADONLY" ) {
                    opciones |= BlSubFormHeader::DbReadOnly;
                } else if ( topci == "DBHIDEVIEW" ) {
                    opciones |= BlSubFormHeader::DbHideView;
                } else if ( topci == "DBNOWRITE" ) {
                    opciones |= BlSubFormHeader::DbNoWrite;
                } else if ( topci == "DBDISABLEVIEW" ) {
                    opciones |= BlSubFormHeader::DbDisableView;
                } // end if
                opci = opci.nextSiblingElement ( "OPTIONSHEADER" );
            } // end while

            addSubFormHeader ( nomheader, type, ( BlDbField::DbRestrict ) restricciones, ( BlSubFormHeader::DbOptions ) opciones, nompheader );
        } // end if
    } // end for

}


///
/**
\return
**/
QList<BlDbSubFormRecord *> *BlSubForm::lista()
{
    BL_FUNC_DEBUG
    
    return &m_lista;
}


///
/**
\return
**/
QList<BlSubFormHeader *> *BlSubForm::headerList()
{
    BL_FUNC_DEBUG
    
    return &m_lcabecera;
}


///
/**
\param mode
**/
void BlSubForm::setHorizontalScrollMode ( QAbstractItemView::ScrollMode mode )
{
    BL_FUNC_DEBUG
    mui_list->setHorizontalScrollMode ( mode );
    
}


///
/**
\param f
**/
void BlSubForm::setDelete ( bool f )
{
    BL_FUNC_DEBUG
    m_delete = f;
    
}


///
/**
**/
bool BlSubForm::isDelete() {
    BL_FUNC_DEBUG
    
    return m_delete;
}


///
/**
\param row
\param col
\return
**/
BlDbSubFormField *BlSubForm::item ( int row, int col )
{
    BL_FUNC_DEBUG
    
    return ( BlDbSubFormField * ) mui_list->item ( row, col );
}


///
/**
\param row
\param col
**/
void BlSubForm::setCurrentItem ( int row, int col )
{
    BL_FUNC_DEBUG
    mui_list->setCurrentItem ( item(row, col) );
    
}


///
/**
\return
**/
int BlSubForm::columnCount()
{
    BL_FUNC_DEBUG
    
    return mui_list->columnCount();
}


/// Column position given its name
/**
\return
**/
int BlSubForm::columnNumber ( const QString &headerName )
{
    BL_FUNC_DEBUG

    int i = -1;
    const int total_filas = mui_listcolumnas->rowCount();
    for ( int col = 0; col < total_filas && i == -1; col++ )
        if ( mui_listcolumnas->item ( col, 1 )->text() == headerName )
            i = col;
    return i;
}


///
/**
\param i
**/
void BlSubForm::showColumn ( int i )
{
    BL_FUNC_DEBUG

    QString field = mui_listcolumnas->item ( i, 1)->text();
    mui_listcolumnas->item ( i, 0 )->setCheckState ( Qt::Checked );
    on_mui_confcol_clicked();
}


///
/**
\param i
\return
**/
int BlSubForm::columnWidth ( int i )
{
    BL_FUNC_DEBUG
    
    return mui_list->columnWidth ( i );
}


///
/**
\param i
\param j
**/
void BlSubForm::setColumnWidth ( int i, int j )
{
    BL_FUNC_DEBUG
    mui_list->setColumnWidth ( i, j );
    
}


///
/**
\param i
**/
void BlSubForm::hideColumn ( int i )
{
    BL_FUNC_DEBUG

    QString field = mui_listcolumnas->item ( i, 1)->text();
    mui_listcolumnas->item ( i, 0 )->setCheckState ( Qt::Unchecked );
    on_mui_confcol_clicked();
}


///
/**
\return
**/
int BlSubForm::currentRow()
{
    BL_FUNC_DEBUG
    
    return mui_list->currentRow();
}


///
/**
\return
**/
int BlSubForm::rowCount()
{
    BL_FUNC_DEBUG
    
    return mui_list->rowCount();
}


///
/**
\return
**/
int BlSubForm::currentColumn()
{
    BL_FUNC_DEBUG
    
    return mui_list->currentColumn();
}


///
/**
\param modo
**/
void BlSubForm::setResizeMode ( QHeaderView::ResizeMode modo )
{
    BL_FUNC_DEBUG
    mui_list->horizontalHeader() ->setSectionResizeMode ( modo );
    
}


///
/**
**/
void BlSubForm::resizeColumnsToContents()
{
    BL_FUNC_DEBUG
    mui_list->resizeColumnsToContents();
    
}


///
/**
**/
void BlSubForm::resizeRowsToContents()
{
    BL_FUNC_DEBUG
    mui_list->resizeRowsToContents();
    
}


///
/**
\param i
**/
void BlSubForm::resizeColumnToContents ( int i )
{
    BL_FUNC_DEBUG
    mui_list->resizeColumnToContents ( i );
    
}


///
/**
\param i
**/
void BlSubForm::resizeRowToContents ( int i )
{
    BL_FUNC_DEBUG
    mui_list->resizeRowToContents ( i );
    
}


///
/**
**/
void BlSubForm::hideConfig()
{
    BL_FUNC_DEBUG
    mui_configurador->hide();
    
}


///
/**
**/
void BlSubForm::showConfig()
{
    BL_FUNC_DEBUG
    mui_configurador->show();
    
}


///
/**
\param nom
**/
void BlSubForm::setDbTableName ( QString nom )
{
    BL_FUNC_DEBUG
    m_tablename = nom;
    m_fileconfig = nom;
    
}


///
/**
\return
**/
QString BlSubForm::tableName()
{
    BL_FUNC_DEBUG
    
    return m_tablename;
}


///
/**
\param nom
**/
void BlSubForm::setFileConfig ( QString nom )
{
    BL_FUNC_DEBUG
    m_fileconfig = nom;
    
}


///
/**
\param nom
**/
void BlSubForm::setDbFieldId ( QString nom )
{
    BL_FUNC_DEBUG
    m_campoid = nom;
    
}


///
/**
**/
QString BlSubForm::dbFieldId() {
    BL_FUNC_DEBUG
    
    return m_campoid;
}


///
/**
**/
void BlSubForm::clear()
{
    BL_FUNC_DEBUG
    mui_list->clear();
    
}


///
/**
\param i
**/
void BlSubForm::setRowCount ( int i )
{
    BL_FUNC_DEBUG
    mui_list->setRowCount ( i );
    
}


///
/**
\param i
**/
void BlSubForm::setColumnCount ( int i )
{
    BL_FUNC_DEBUG
    mui_list->setColumnCount ( i );
    
}


///
/**
**/
void BlSubForm::createMenu ( QMenu * )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubForm:: CreaMenu", 0, "funcion para ser sobreescrita" );
}


///
/**
**/
void BlSubForm::execMenuAction ( QAction * )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubForm:: execMenuAction", 0, "funcion para ser sobreescrita" );
}


///
/**
\param pijama
**/
void BlSubForm::setListadoPijama ( bool pijama )
{
    BL_FUNC_DEBUG
    if ( pijama ) {
        mui_list->setAlternatingRowColors ( true );
    } else {
        mui_list->setAlternatingRowColors ( false );
    } // end if
    
}


///
/**
\return
**/
bool BlSubForm::listadoPijama()
{
    BL_FUNC_DEBUG
    return mui_list->alternatingRowColors();
    
}



/// Habilita o inhabilita el ordenado de columnas mediante el pulsar sobre ellas.
/**
\param sorting
**/
void BlSubForm::setSortingEnabled ( bool sorting )
{
    BL_FUNC_DEBUG
    mui_list->setSortingEnabled ( sorting );
    m_sorting = sorting;
    
}


///
/**
\return
**/
bool BlSubForm::sortingEnabled()
{
    BL_FUNC_DEBUG
    
    return m_sorting;
}


/// Establece si el subformulario se ordena mediante un campo orden en la base de datos.
/**
\param sorting
**/
void BlSubForm::setOrdenEnabled ( bool sorting )
{
    BL_FUNC_DEBUG
    m_orden = sorting;
    
}


///
/**
\return
**/
bool BlSubForm::ordenEnabled()
{
    BL_FUNC_DEBUG
    return m_orden;
    
}


///
/**
\param item
**/
void BlSubForm::on_mui_list_itemDoubleClicked ( QTableWidgetItem *item )
{
    BL_FUNC_DEBUG
    emit itemDoubleClicked ( item );
    
}

///
/**
\param item
**/
void BlSubForm::on_mui_list_itemClicked ( QTableWidgetItem *item )
{
    BL_FUNC_DEBUG
    emit itemClicked ( item );
    
}


///
/**
\param row
\param col
**/
void BlSubForm::on_mui_list_cellDoubleClicked ( int row, int col )
{
    BL_FUNC_DEBUG
    emit cellDoubleClicked ( row, col );
    
}


///
/**
\param head
\return
**/
bool BlSubForm::existsHeader ( const QString &head )
{
    BL_FUNC_DEBUG
    BlSubFormHeader *linea;
    for ( int i = 0; i < m_lcabecera.size(); ++i ) {
        linea = m_lcabecera.at ( i );
        if ( linea->fieldName() == head ) {
            
            return true;
        } // end if
    } // end for
    
    return false;
}

///
/**
\param head
\return
**/
BlSubFormHeader * BlSubForm::header ( const QString &head )
{
    BL_FUNC_DEBUG
    BlSubFormHeader *linea = NULL;
    for ( int i = 0; i < m_lcabecera.size(); ++i ) {
        linea = m_lcabecera.at ( i );
        if ( linea->fieldName() == head ) {
            
            return linea;
        } // end if
    } // end for
    
    return NULL;
}




const int BlSubFormHeader::numericPrecision() {
    BL_FUNC_DEBUG
    return m_numericPrecision;
    
}


void BlSubFormHeader::setNumericPrecision(int pres) {
    BL_FUNC_DEBUG
    m_numericPrecision = pres;
    
}



/// Se encarga de crear un nuevo registro (una fila entera) y de inicializarla para que
/// tenga todos los elementos necesarios (columnas).
/// Nota: Esta funcion es de uso interno, no debe ser usada.
/**
\return
**/
BlDbSubFormRecord *BlSubForm::newDbSubFormRecord()
{
    BL_FUNC_DEBUG
    BlDbSubFormRecord *rec = new BlDbSubFormRecord ( mainCompany() );
    rec->setDbTableName ( m_tablename );
    rec->setDbFieldId ( m_campoid );

    BlSubFormHeader *linea;
    for ( int i = 0; i < m_lcabecera.size(); ++i ) {
        linea = m_lcabecera.at ( i );
        
        int type = QTableWidgetItem::Type;
        if (!(linea->options() & BlSubFormHeader::DbNoWrite)) type = QTableWidgetItem::UserType;
        
        rec->addDbField ( linea->fieldName(), linea->fieldType(), linea->restrictions(), linea->fieldTitle(), type );
    } // end for

    BlDbSubFormField *camp;
    for ( int i = 0; i < rec->lista() ->size(); ++i ) {
        camp = ( BlDbSubFormField * ) rec->lista() ->at ( i );
        BlSubFormHeader *head = m_lcabecera.at ( i );
        Qt::ItemFlags flags = 0;
        flags |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;

        if ( ! ( head->options() & BlSubFormHeader::DbNoWrite ) )
            flags |= Qt::ItemIsEditable;
        if ( head->fieldType() == BlDbField::DbBoolean ) {
            flags |= Qt::ItemIsUserCheckable;
        } // end if

        camp->setFlags ( flags );
        
        /// Valor por defecto
        if ( head->fieldType() == BlDbField::DbBoolean )
        {
          camp->setCheckState(Qt::Unchecked);
        } // end if

        /// Tratamos el tema de la alineacion dependiendo del tipo.
        if ( head->fieldType() == BlDbField::DbInt || head->fieldType() == BlDbField::DbNumeric || head->fieldType() == BlDbField::DbDate || head->fieldType() == BlDbField::DbTime ) {
            camp->setTextAlignment ( Qt::AlignRight | Qt::AlignVCenter );
        } else {
            camp->setTextAlignment ( Qt::AlignLeft | Qt::AlignVCenter );
        } // end if
    } // end for
    
    return rec;
}


/// Este metodo crea el registro final cuando se trata de subformularios con la opcion
/// de insertar nuevos registros en el subformulario.
/**
\return
**/
void BlSubForm::newRecord()
{
    BL_FUNC_DEBUG
    if ( !m_insercion ) {
        
        return;
    } // end if

    /// Desactivamos la ordenacion automatica.
    mui_list->setSortingEnabled ( false );

    BlDbSubFormRecord *rec = newDbSubFormRecord();

    m_lista.append ( rec );

    mui_list->insertRow ( m_lista.size() - 1 );
    BlDbSubFormField *camp;
    for ( int i = 0; i < rec->lista() ->size(); ++i ) {
        camp = ( BlDbSubFormField * ) rec->lista() ->at ( i );
        mui_list->setItem ( m_lista.size() - 1, i, camp );
        //mui_list->setCellWidget ( m_lista.size() - 1, i, camp );
    } // end for

    /// Nos posicionamos en una celda del nuevo registro ya que si no un bug no nos permite usar currentRow()
    for (int i = 0; i < mui_list->columnCount(); ++i)
    {
      if (mui_list->isColumnHidden(i)) continue;
      bool finded = false;
      for (int j = 0; j < mui_list->columnCount(); ++j)
      {
        if (mui_list->visualColumn(j) == i)
        {
          mui_list->setCurrentCell ( m_lista.size() - 1, j);
          finded = true;
          break;
        } // end if
      } // end for
      if (finded) break;
    } // end for

    /// Activamos la ordenacion automatica.
    mui_list->setSortingEnabled ( m_sorting );
    g_plugins->run("BlSubForm_newRecord", this);
    
}


/// Pinta las cabeceras en la tabla.
/**
**/
void BlSubForm::pintaCabeceras()
{
    BL_FUNC_DEBUG
    QStringList headers;
    BlSubFormHeader *linea;
    for ( int i = 0; i < m_lcabecera.size(); ++i ) {
        linea = m_lcabecera.at ( i );
        headers << linea->fieldTitle();
        if ( linea->options() & BlSubFormHeader::DbHideView ) {
            mui_list->hideColumn ( i );
        } else {
            mui_list->showColumn ( i );
        } // end if
    } // end for
    mui_list->setHorizontalHeaderLabels ( headers );
    mui_list->horizontalHeader() ->setSectionResizeMode ( 0, QHeaderView::Interactive );
    
    /// Recorre las cabeceras aplicando configuraciones de tamaño
    for (int indexColumna = 0; indexColumna < mui_list->columnCount(); ++indexColumna)
    {
      QTableWidgetItem *item = mui_list->horizontalHeaderItem(indexColumna);
      QFont font = item->font();
      font.setWeight(QFont::Bold);
      font.setPointSize(atoi(g_confpr->value(CONF_FONTSIZE_BULMAGES).toLatin1().constData()));
      QFontMetrics fm(font);
      int minWidth = fm.boundingRect(item->text()).width() + 40; /// 20px de margen para cada lado.
      
      //qDebug() << item->text() << " " << minWidth;
      
      mui_list->setColumnWidth(indexColumna, minWidth);
    } // end for
    
}


/// Se situa en una celda especifica del subformulario.
/**
\param row
\param col
\param back Recorrer las celdas en orden inverso (por defecto, falso)
**/
void BlSubForm::situarse ( unsigned int row, unsigned int col, bool back )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubForm::situarse", 0, QString::number ( row ) + " " + QString::number ( col ) );

    unsigned int nrow = row;
    unsigned int ncol = col;
    BlSubFormHeader *linea = m_lcabecera.at ( ncol );
    if ( !linea ) {
	
        return;
    } // end if
    bool invalido = true;

    /// Mientras no se encuentre un candidato y haya candidatos vamos recorriendo celdas inversamente.
    while ( back && invalido && nrow >= 0 ) {

	  /// Si estamos al principio de la linea cogemos el principio de la linea anterior
	  if ( ncol == 0 ) {
		if ( nrow > 0 ) nrow--;
		else break;

		ncol = ( unsigned int ) m_lcabecera.count() - 1;
	  }
	  else {
		ncol--;
	  } // end if

	  linea = m_lcabecera.at ( ncol );
	  invalido = false;
	  if ( ( linea->options() & BlSubFormHeader::DbHideView )
	  ||   ( linea->options() & BlSubFormHeader::DbNoWrite ) )
		invalido = true;
     } // end while

    /// Mientras no se encuentre un candidato y haya candidatos vamos recorriendo celdas.
    while ( !back && invalido && nrow < row + 2 ) {
	  ncol++;
	  /// Si estamos al final de la linea cogemos el principio de la linea siguiente
	  if ( ncol == ( unsigned int ) m_lcabecera.count() ) {
		ncol = 0;
		nrow++;
	  } // end if
	  linea = m_lcabecera.at ( ncol );
	  invalido = false;
	  if ( ( linea->options() & BlSubFormHeader::DbHideView )
	  ||   ( linea->options() & BlSubFormHeader::DbNoWrite ) )
		 invalido = true;
    } // end while

    if (!invalido) {
       mui_list->setCurrentCell ( mui_list->visualRow(nrow), mui_list->visualColumn(ncol) );
    } else {
       BlDebug::blDebug("No hay mas elementos editables en el subformulario", 2);
    } // end if
    
}


/// Se situa en una celda especifica del subformulario.
/** Se incluye la celda incluye la actual */
/**
\param row
\param col
\return
**/
void BlSubForm::situarse1 ( unsigned int row, unsigned int col )
{
    BlDebug::blDebug ( "BlSubForm::situarse1", 0, QString::number ( row ) + " " + QString::number ( col ) );
    unsigned int nrow = row;
    unsigned int ncol = col;
    BlSubFormHeader *linea = m_lcabecera.at ( ncol );
    if ( !linea ) {
        return;
    } // end if
    bool invalido = true;
    while ( invalido ) {
        if ( ncol == ( unsigned int ) m_lcabecera.count() ) {
            ncol = 0;
            nrow++;
        } // end if
        linea = m_lcabecera.at ( ncol );
        invalido = false;
        if ( linea->options() & BlSubFormHeader::DbHideView )
            invalido = true;
        if ( linea->options() & BlSubFormHeader::DbNoWrite )
            invalido = true;
        if ( invalido )
            ncol++;
    } // end while
    mui_list->setCurrentCell ( nrow, ncol );
    
}


/// Cuando tenemos un registro que no se tiene que load (pq es nuevo o algo
/// asi) de la base de datos, con la funcion pintar lo dejamos en un estado que
/// se podria considerar presentable para poder operar con el subformulario.
/**
**/
void BlSubForm::pintar()
{
    BL_FUNC_DEBUG
    m_procesacambios = false;
    mui_list->setColumnCount ( m_lcabecera.count() );
    pintaCabeceras();
    if ( m_primero ) {
        loadConfig();
	/// Preparamos el menu de subformulario
	preparaMenu();
    } // end if
    newRecord();
    m_procesacambios = true;
    
}


/// Carga una tabla a partir del recordset que se le ha pasado.
/**
\return
**/
int BlSubForm::inicializar()
{
    BL_FUNC_DEBUG
    m_procesacambios = false;
    mui_query->setPlainText ( "" );
    BlDbSubFormRecord *rec;

    int filpag = mui_filaspagina->text().toInt();
    if ( filpag <= 0 )
        filpag = 500;

    int pagact = mui_paginaact->text().toInt();
    if ( pagact <= 0 )
        pagact = 1;

    /// Vaciamos la tabla para que no contenga registros.
    mui_list->clear();
    mui_list->setRowCount ( 0 );

    /// Vaciamos el recordset para que no contenga registros.
    while ( m_lista.count() ) {
        rec = m_lista.takeFirst();
        if ( rec )
            delete rec;
    } // end while

    /// Ponemos los datos sobre el query.
    mui_numfilas->setText ( "0" );
    int numpag = 0;
    mui_numpaginas->setText ( QString::number ( numpag ) );
    /// Inicializamos las columnas y pintamos las cabeceras.
    mui_list->setColumnCount ( m_lcabecera.count() );
    pintaCabeceras();
    if ( m_primero ) {
        loadConfig();
	/// Preparamos el menu de subformulario
	preparaMenu();
    } // end if

    newRecord();
    /// Ordenamos la tabla.
    mui_list->ordenar();
    /// configuramos que registros son visibles y que registros no lo son.
    on_mui_confcol_clicked();
    m_procesacambios = true;
    
    return 0;
}


///
/**
\param campo
**/
void BlSubForm::setColumnToRowSpan ( QString campo )
{
    BL_FUNC_DEBUG
    m_columnaParaRowSpan = campo;
    
}


///
/**
\param color
**/
void BlSubForm::setColorFondo1 ( QString color )
{
    BL_FUNC_DEBUG
    m_colorfondo1 = QColor ( color );
    
}


///
/**
\param color
**/
void BlSubForm::setColorFondo2 ( QString color )
{
    BL_FUNC_DEBUG
    m_colorfondo2 = QColor ( color );
    
}


///
/**
\param twidget
\param filainicial
\param totalfilas
\param colorfondo
**/
void BlSubForm::ponItemColorFondo ( QTableWidget *twidget, int filainicial, int totalfilas, QColor colorfondo )
{
    BlDbSubFormRecord * reg3;
    /// Si hay que poner color al fondo de las filas se pone el que toque.
    if ( listadoPijama() == false ) {
        /// Recorre todos los items de las filas afectadas.
        for ( int k = filainicial; k < filainicial + totalfilas; ++k ) {
            reg3 = m_lista.at ( k );
            for ( int r = 0; r < reg3->lista() ->size(); ++r ) {
                /// Pone el color
                twidget->item ( k, r ) ->setBackground ( QBrush ( colorfondo ) );
            } // end for
        } // end for
    } // end if
}


/// Carga una tabla a partir del recordset que se le ha pasado.
/** Este método genera, a partir del recordset pasado como parámetro,
    el listado y lo muestra. */
/**
\param cur
**/
void BlSubForm::load ( BlDbRecordSet *cur )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubForm::cargar", 0, objectName() );
    m_procesacambios = false;


    BlDbSubFormRecord *reg;
    BlDbSubFormRecord *reg2;
    BlDbSubFormField *camp;
    BlDbSubFormField *camp2;
    QColor colorfondo = m_colorfondo1;
    bool coloraponerfondo = false;

    /// Preparamos la barra de progreso
    BlProgressBar *barra = new BlProgressBar;
    if ( cur->numregistros() > 100 ) {
        barra->setValue ( 0 );
        barra->show();
        barra->setText ( _ ( "Cargando SubFormulario " ) + m_tablename );
    } // end if

    /// Desactivamos el sorting debido a un error en Qt4.
    mui_list->setSortingEnabled ( false );

    /// Reseteamos el "rowSpan" de la tabla antes de borrar las filas.
    for ( int i = 0; i < m_lista.size(); ++i ) {
        reg = m_lista.at ( i );
        for ( int j = 0; j < reg->lista() ->size(); ++j ) {
            BlSubFormHeader *head = m_lcabecera.at ( j );
            if ( head->fieldName() == m_columnaParaRowSpan ) {
                mui_list->setSpan ( i, j, 1, 1 );
            } // end if
        } // end for
    } // end for

    /// Vaciamos la tabla para que no contenga registros.
    mui_list->clear();
    mui_list->setRowCount ( 0 );

    /// Vaciamos el recordset para que no contenga registros.
    BlDbSubFormRecord *rec;
    while ( !m_lista.isEmpty() ) {
        rec = m_lista.takeFirst();
        if ( rec )
            delete rec;
    } // end while

    /// Inicializamos las columnas y pintamos las cabeceras.
    mui_list->setColumnCount ( m_lcabecera.count() );
    pintaCabeceras();

    /// Si hay un problema con el cursor, se sale antes de generar Segmentation Fault.
    if ( cur == NULL ) return;

    /// Ponemos la consulta a la vista para que pueda ser editada.
    mui_query->setPlainText ( cur->pristineQuery() );

    /// Tratamos con la paginacion.
    int filpag = mui_filaspagina->text().toInt();
    if ( filpag <= 0 ) {
        filpag = 500;
    } // end if

    int pagact = mui_paginaact->text().toInt();
    if ( pagact <= 0 ) {
        pagact = 1;
    } // end if

    /// Ponemos los datos sobre la consulta.
    mui_numfilas->setText ( QString::number ( cur->numregistros() ) );
    int numpag = cur->numregistros() / filpag + 1;
    mui_numpaginas->setText ( QString::number ( numpag ) );

    /// Si el numero de elementos es igual al numero de filas por pagina habilitamos la pagina siguiente
    if ( filpag == cur->numregistros() ) {
        mui_pagsiguiente->setEnabled ( true );
    } else {
        mui_pagsiguiente->setDisabled ( true );
    } // end if

    /// Si la pagina actual es 1 deshabilitamos la pagina anterior
    if ( pagact ==  1 ) {
        mui_paganterior->setDisabled ( true );
    } else {
        mui_paganterior->setEnabled ( true );
    } // end if

    /// Recorremos el recordset y ponemos los registros en un orden determinado.
    int porcentajecarga = 0;
    while ( !cur->eof() && m_lista.count() < filpag ) {
        BlDbSubFormRecord * rec = newDbSubFormRecord();
        rec->DBload ( cur );
        m_lista.append ( rec );
        cur->nextRecord();
        porcentajecarga++;
    } // end while

    /// Inicializamos la tabla con las filas necesarias.
    mui_list->setRowCount ( m_lista.count() );

    /// Rendimiento: a partir de aquí m_lista ya no cambia, así que ya podemos usar una constante
    /// para guardar la cantidad de elementos y no volver a invocar al método size()
    const int m_lista_size = m_lista.size();

    /// Rendimiento: estas variables sólo se usan en el bucle, pero no es necesario crearlas en cada iteración
    QRegExp patronFecha ( "^.*00:00:00.*$" ); /// Para emparejar los valores fechas.
    QFont bold;
    bold.setBold ( true );

    barra->setRange ( 0, m_lista_size );
    for ( int i = 0; i < m_lista_size; ++i ) {
        reg = m_lista.at ( i );
        for ( int j = 0; j < reg->lista() ->size(); ++j ) {
           /// Pone en negrita el texto de la cabecera de las columnas editables
           if ( i == 0 && ! ( m_lcabecera [ j ] -> options() & BlSubFormHeader::DbNoWrite ) ) {
              mui_list->horizontalHeaderItem ( j ) -> setFont( bold );
           } // end if
            camp = ( BlDbSubFormField * ) reg->lista() ->at ( j );
            /// Si es una fecha lo truncamos a 10 caracteres para presentar solo la fecha.
            if ( patronFecha.exactMatch ( camp->fieldValue() ) ) {
                camp->set
                ( camp->fieldValue().left ( 10 ) );
            } // end if
            /// Rellena la tabla con los datos.
            mui_list->setItem ( i, j, camp );
        } // end for
        barra->setValue ( i );
    } // end for

    /// Establece el "rowSpan" de la tabla.
    QString textoCeldaAnterior;
    QString textoCeldaActual;

    /// Recorre las filas.
    m_filaInicialRowSpan = -1;

    /// Pone el 'rowSpan' a las filas que son iguales.
    for ( int i = 0; i < m_lista_size; ++i ) {
        reg = m_lista.at ( i );
        for ( int j = 0; j < reg->lista() ->size(); ++j ) {
            BlSubFormHeader *head = m_lcabecera.at ( j );
            if ( head->fieldName() == m_columnaParaRowSpan ) {
                camp = ( BlDbSubFormField * ) reg->lista() ->at ( j );
                textoCeldaActual = camp->fieldValue();
                /// Mira lo que hay en la fila anterior si existe.
                if ( i > 0 ) {
                    reg2 = m_lista.at ( i - 1 );
                    camp2 = ( BlDbSubFormField * ) reg2->lista() ->at ( j );
                    textoCeldaAnterior = camp2->fieldValue();
                    if ( textoCeldaActual == textoCeldaAnterior ) {
                        /// activamos el indice de celdas iguales
                        if ( m_filaInicialRowSpan == -1 ) {
                            m_filaInicialRowSpan = i - 1;
                        } // end if
                        /// hay un registro después. No, dibuja 'rowSpan'.
                        if ( i == ( m_lista_size - 1 ) ) {
                            ponItemColorFondo ( mui_list, m_filaInicialRowSpan, i - m_filaInicialRowSpan + 1, colorfondo );
                            mui_list->setSpan ( m_filaInicialRowSpan, j, i - m_filaInicialRowSpan + 1, 1 );

                            if ( coloraponerfondo == false ) {
                                colorfondo = m_colorfondo2;
                                coloraponerfondo = true;
                            } else {
                                colorfondo = m_colorfondo1;
                                coloraponerfondo = false;
                            } // end if

                        } // end if
                    } else {
                        /// Comprobamos si queda algo pendiente de hacer 'rowSpan'.
                        if ( m_filaInicialRowSpan != -1 ) {
                            /// 'rowSpan' desde inicio iguales hasta fila anterior.
                            ponItemColorFondo ( mui_list, m_filaInicialRowSpan, i - m_filaInicialRowSpan, colorfondo );
                            mui_list->setSpan ( m_filaInicialRowSpan, j, i - m_filaInicialRowSpan, 1 );

                            if ( coloraponerfondo == false ) {
                                colorfondo = m_colorfondo2;
                                coloraponerfondo = true;
                            } else {
                                colorfondo = m_colorfondo1;
                                coloraponerfondo = false;
                            } // end if
                        } else {
                            /// El registro sólo tiene una fila.
                            ponItemColorFondo ( mui_list, i - 1, 1, colorfondo );
                            if ( coloraponerfondo == false ) {
                                colorfondo = m_colorfondo2;
                                coloraponerfondo = true;
                            } else {
                                colorfondo = m_colorfondo1;
                                coloraponerfondo = false;
                            } // end if
                        } // end if
                        m_filaInicialRowSpan = -1;
                    } // end if
                } // end if
            } // end if
        } // end for
    } // end for

    /// Si está definido no aplicamos ninguna ordenación.
    if ( !m_ordenporquery ) {
        if ( m_orden ) {
            /// Si estamos con campos de ordenación ordenamos tras la carga el listado.
            for ( int i = 0; i < m_lcabecera.size(); ++i ) {
                if ( m_lcabecera.at ( i ) ->fieldName() == "orden" + m_tablename )
                    mui_list->sortItems ( i );
            } // end for
        } else {
            /// Si no estamos con campos de ordenación ordenamos por lo que toca.
            /// Ordenamos la tabla.
            mui_list->ordenar();
        } // end if
    } // end if
    /// Generamos el registro de insercion.
    newRecord();

    ///  Cargamos la configuracion previamente almacenada.
    /// Configuramos que registros son visibles y que registros no lo son
    if ( m_primero ) {
        loadConfig();
	/// Preparamos el menu de subformulario
	preparaMenu();
    } else {
        on_mui_confcol_clicked();
    } // end if

    /// Reactivamos el sorting
    mui_list->setSortingEnabled ( m_sorting );

    /// Borramos la barra de progreso liberando memoria.
    delete barra;

    m_procesacambios = true;

    /// Ajustamos las columnas al contenido.
    if (g_confpr->value(CONF_RESIZEROWSTOCONTENTS) == "TRUE") {
      mui_listcolumnas->resizeRowsToContents();
      resizeRowsToContents();
    } // end if
    
    
}


///
/**
\param ordenactivado
**/
void BlSubForm::setOrdenPorQuery ( bool ordenactivado )
{
    BL_FUNC_DEBUG
    m_ordenporquery = ordenactivado;
    
}


void BlSubForm::load ( QString query )
{
    BL_FUNC_DEBUG
    /// Si el query no existe no hacemos nada.
    if ( query == "" ) {
	
        return;
    } // end if

    try {

        /// Guardar los valores necesarios para poder mantener la posici&oaucte;n en la lista tras realizar un cambio o borrado
        int active_col = -1; /// Columna activa antes de ejecutar la recarga
        int active_row = -1; /// Fila activa antes de ejecutar la recarga
        QString active_row_id; /// Identificador que había en dicha fila
        QString prev_active_row_id; /// Identificador que había en la fila anterior a la activa
        QString next_active_row_id; /// Identificador que había en la fila posterior a la activa

        /// Si la tabla no est&aacute; vac&iacute;a y tiene alguna fila activa, guardar los datos necesarios
        if ( rowCount() > 0 && currentRow() > -1 )
        {
            active_row = currentRow();
            active_col = currentColumn();

            /// Tomar el identificador de la fila anterior (si es que la hay)
            if ( active_row > 0 ) {
                prev_active_row_id = dbValue(dbFieldId(), active_row - 1);
            } // end if

            /// Tomar el identificador de la fila actual
            active_row_id = dbValue(dbFieldId());

            /// Tomar el identificador de la fila siguiente (si es que la hay)
            if ( active_row < rowCount() - 1 ) {
                next_active_row_id = dbValue(dbFieldId(), active_row + 1);
            } // end if
        } // end if
        /// Fin de Guardar los valores necesarios para poder mantener la posici&oaucte;n en la lista tras realizar un cambio o borrado

        m_query = query;
        /// Tratramos con la paginacion.
        int limit = mui_filaspagina->text().toInt();
        if ( limit <= 0 ) {
            limit = 500;
        } // end if

        int pagact = mui_paginaact->text().toInt();
        if ( pagact <= 0 ) {
            pagact = 1;
        } // end if
        int offset = limit * ( pagact - 1 );

        BlDbRecordSet *cur = mainCompany() ->loadQuery ( query, "", limit, offset );

        if (!cur) throw -1;

        load ( cur );
        delete cur;

        /// Restaurar la posición anterior a la carga si es posible
        /// La condición "active_row <= rowCount()" sirve cuando un filtrado reduce la cantidad de filas
        if ( rowCount() > 0 && active_row > -1 && active_row <= rowCount() ) {

           int fila_futura = -1;

           /// Si la fila activa no era la &uacute;ltima,
           if ( !next_active_row_id.isEmpty() && active_row < rowCount() ) {

              /// y sigue siendo la misma (no ha sido borrada)...
              if ( ( !active_row_id.isEmpty() && dbValue ( dbFieldId(), active_row ) == active_row_id )

              /// o ha sido borrada pero hay otra despu&eacute;s que ha ocupado su lugar...
                 || ( !next_active_row_id.isEmpty() && dbValue ( dbFieldId(), active_row ) == next_active_row_id ) ) {

                 /// ... usar el mismo n&uacute;mero de fila
                 fila_futura = active_row;
              } // end if
          } // end if

           /// Si la fila era la &uacute;ltima, seguir en la &uacute;ltima, pase lo que pase
           else
           {
               fila_futura = rowCount() - 1;
           } // end if

           if ( fila_futura > -1 ) {
               mui_list->setCurrentCell ( fila_futura, active_col ) ;
               mui_list->scrollToItem ( mui_list->currentItem(), QAbstractItemView::PositionAtCenter ) ;
           } // end if

        } // end if
        /// Fin de Restaurar la posición anterior a la carga si es posible

    } catch ( ... ) {
	
    } // end try
    
}



/// Devuelve la linea que se esta tratando actualmente.
/**
\return
**/
BlDbSubFormRecord *BlSubForm::lineaact()
{
    BL_FUNC_DEBUG
    
    return lineaat ( mui_list->currentRow() );
}


/// Devuelve la linea especificada o NULL si esta no existe.
/**
\param row
\return
**/
BlDbSubFormRecord *BlSubForm::lineaat ( int row )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubForm::lineaat()", 0, QString::number ( row ) );
    BlDbSubFormRecord *rec = NULL;
    try {
        /// Si la lista no tiene suficientes elementos devolvemos NULL
        if ( mui_list->rowCount() <= row || row < 0 ) {
            throw - 1;
        } // end if

        /// Seleccionamos el campo especificado y lo devolvemos.
        BlDbSubFormField *camp = ( BlDbSubFormField* ) mui_list->item ( row, 0 );
        if ( !camp ) {
            throw - 1;
        } // end if
        rec = ( BlDbSubFormRecord * ) camp->pare();

    } catch ( ... ) {
        BlDebug::blDebug ( "BlSubForm::lineaat linea inexistente", 2, QString::number ( row ) );
        rec = NULL;
    }
    
    return rec;
}


/// Devuelve true si el registro ha sido completamente rellenado.
/**
\param row
\return
**/
bool BlSubForm::campoCompleto ( int row )
{
    BL_FUNC_DEBUG
    bool resultat = false;
    bool *pResultat = &resultat;
    
    if ( g_plugins->run ( "BlSubForm_campoCompleto", this, ( void** ) &pResultat ) ) {
	
        return resultat;
    } else {
        resultat = true;
    
        BlDbSubFormField *camp;
        BlSubFormHeader *header;
        /// Sacamos celda a celda toda la fila
        for ( int i = 0; i < mui_list->columnCount(); i++ ) {
            camp = ( BlDbSubFormField * ) mui_list->item ( row, i );

            /// Si el dato no es valido se sale
            if ( !camp ) return false;

            header = m_lcabecera.at ( i );
            
            if (camp->fieldType() == BlDbField::DbBoolean) continue;
            if (camp->fieldRestrictions() & BlDbField::DbNotNullAutoFilled) continue;
            
            /// Campo obligatorio o no puede estar vacio
            if ((camp->fieldRestrictions() & BlDbField::DbNotNull) || (camp->fieldRestrictions() & BlDbField::DbRequired))
            {
                /// Está bloquedado?
                if (header->options() & BlSubFormHeader::DbNoWrite)
                {
                  /// No lo miro más
                  continue;
                } else {
                  /// No está bloquedo y es obligatorio. Tiene datos?
                  if (camp->text().isEmpty()) return false;
                } // end if
            } // end if

        } // end for
    } // end if

    return resultat;
}


/// M&eacute;todo que se dispara cuando se va a reposicionar en el formulario.
/// Se encarga de resituar el cursor al lugar que se haya indicado.
/// Con el fin de evitar problemas de reposicionado le activamos un semaforo y bloqueamos signals.
/**
\param row
\param col
**/
void BlSubForm::on_mui_list_cellRePosition ( int row, int col )
{
    BL_FUNC_DEBUG

    BlDebug::blDebug ( "BlSubForm::on_mui_list_cellReposition", 0, "Row: " + QString::number ( row ) + " col: " + QString::number ( col ) );
    /// Implementacion del semaforo
    static bool semaforo = false;
    if ( semaforo )
        return;
    semaforo = true;

    /// Bloqueamos la emision de signals
    mui_list->blockSignals(true);


    bool creado = false;

    int key = mui_list->m_teclasalida;

    /// Miramos si hemos completado la linea y si es asi creamos una linea nueva
    if ((m_insercion) && (row == mui_list->rowCount() - 1)) {
        if (campoCompleto(row))
        {
            newRecord();
            creado = true;
        } else {
            /// TODO. Sacar el mensaje solo cuando estamos en la última columna visible. Pendiente de calcular esto.
            ///blMsgWarning(_("Faltan datos obligatorios"));
        } // end if
    } // end if

    switch ( key ) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if ( !m_insercion ) {
            /// Se ha hecho un enter sobre una tabla sin insercion con lo que lanzamos un doble click para que sea
            /// La accion simulada.
            QTableWidgetItem * item = mui_list->currentItem();
            /// Desbloqueamos la emision de signals
            mui_list->blockSignals(false);
            /// Emitimos las signals de dobleclick
            emit itemDoubleClicked ( item );
            emit cellDoubleClicked ( row, col );
        } else {
            situarse ( row, col );
        } // end if
        break;
    case Qt::Key_Backtab:
	    situarse ( row, col, true );
	break;
    case Qt::Key_Tab:
	    situarse ( row, col );
	break;
    case Qt::Key_Down:
        situarse ( row, col );
        situarse1 ( row, col );
        if ( creado ) {
            mui_list->setCurrentCell ( row + 1, col );
        } // end if
        break;
    } // end switch

    semaforo = false;

    /// Desbloqueamos la emision de signals
    mui_list->blockSignals(false);
    
}


/// M&eacute;todo que se dispara cuando se termina de editar un campo del Subformulario.
/**
\param row
\param col
**/
void BlSubForm::on_mui_list_cellChanged ( int row, int col )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubForm::on_mui_list_cellChanged", 0, "Row: " + QString::number ( row ) + " col: " + QString::number ( col ) );
    BlDbSubFormRecord *rec = lineaat ( row );
    if ( rec == NULL ) {
        return;
    } // end if

    BlDbSubFormField *camp = ( BlDbSubFormField * ) item ( row, col );
    camp->refresh();

    /// Si existe la restriccion unique la comprobamos y no permitimos establecer el valor saliendo con un aviso.
    if ( camp->fieldRestrictions() & BlDbField::DbUnique && camp->fieldValue() != "") {
	BlDbSubFormRecord *rec1;
	for ( int i = 0; i < mui_list->rowCount(); ++i ) {
	    rec1 =  lineaat ( i );
	    if ( rec1 && rec1 != rec) {
	      if (rec1->dbValue(camp->fieldName()) == camp->fieldValue()) {
		blMsgInfo( _ ("Ya existe un elemento con este valor en el subformulario. Se viola una clave unica."));
		camp->set("");
		return;
	      } // end if
	    } // end if
	} // end for
    } // end if
    
    
    /// Si el campo no ha sido cambiado se sale.
    if ( ! camp->isFieldChanged() ) {
	BlDebug::blDebug ( ( Q_FUNC_INFO), 0, _("Sin cambios"));
        return;
    } // end if

    /// En el caso de tener una fecha directamente la tratamos ya que este claro cual es su tratamiento.
    if (camp->fieldType() == BlDbField::DbDate) {
        camp->set ( blNormalizeDate ( camp->fieldValue() ).toString ( "dd/MM/yyyy" ) );
    } // end if

    if ( m_procesacambios ) {
        m_procesacambios = false;
        m_prevRow = row;
        m_prevCol = col;
        editFinished ( row, col, rec, camp );
        emit editFinish ( row, col );
        m_procesacambios = true;
    } // end if
}


/// M&eacute;todo para agregar cabeceras al listado
/** Cada columna de un subformulario tiene una instancia de la clase BlSubFormHeader. Este m&eacute;todo
    se encarga de crear e inicializar cada una de dichas clases. */
/**
\param nom
\param typ
\param res
\param opt
\param nomp
\return
**/
int BlSubForm::addSubFormHeader ( QString nom, BlDbField::DbType typ, int res, int opt, QString nomp )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubForm::addSubFormHeader", 0,  nom );

    BlSubFormHeader *camp = new BlSubFormHeader ( nom, typ, res, opt, nomp );
    m_lcabecera.append ( camp );
    mui_listcolumnas->insertRow ( mui_listcolumnas->rowCount() );

    BlTableWidgetItem *it = new BlTableWidgetItem (nomp);
    it->setFlags ( Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable );

    if ( opt & BlSubFormHeader::DbHideView ) {
        mui_list->hideColumn ( mui_listcolumnas->rowCount() - 1 );
        it->setCheckState ( Qt::Unchecked );
    } else {
        it->setCheckState ( Qt::Checked );
    } // end if

    if ( opt & BlSubFormHeader::DbDisableView ) {
        it->setFlags ( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable );
    } // end if

    /// Comprobamos la precision del campo (si este es del tipo numerico) y la guardamos.
    /// Lo hacemos aqui y no en el constructor de BlDbField porque precisamos del nombre de la tabla para
    /// poder hacer la consulta sobre la precisión del campo y ese dato solo esta en BlDbRecord.
    /// Buscamos los decimales que tiene el campo y establecemos el numero de decimales a ese valor.

    /// NOTA: En caso de que no haya sido posible calcular la precisión esta se establece por defecto como 2.

    if (typ == BlDbField::DbNumeric ) {
      if ( tableName() != "" && mainCompany() != NULL ) {
          QString query2 = "SELECT numeric_scale FROM information_schema.columns WHERE table_name = '"+tableName()+"' and column_name='"+nom+"';";
          BlDbRecordSet *cur = mainCompany() ->loadQuery ( query2 );
          if ( !cur->eof() ) {
              camp->setNumericPrecision(cur->value("numeric_scale").toInt());
          } // end if
          delete cur;
      } else {
              camp->setNumericPrecision(2);
      } // end if
    } // end if

    mui_listcolumnas->setItem ( mui_listcolumnas->rowCount() - 1, 0, it );
    it = new BlTableWidgetItem ( nom );
    mui_listcolumnas->setItem ( mui_listcolumnas->rowCount() - 1, 1, it );
    it = new BlTableWidgetItem ( "" );
    mui_listcolumnas->setItem ( mui_listcolumnas->rowCount() - 1, 2, it );
    /// Escribe el order de las columnas.
    it = new BlTableWidgetItem ( QString::number ( mui_listcolumnas->rowCount() - 1 ) );
    mui_listcolumnas->setItem ( mui_listcolumnas->rowCount() - 1, 3, it );

    
    return 0;
}


void BlSubForm::removeSubFormHeader ( QString nom )
{
    BL_FUNC_DEBUG

    BlSubFormHeader *headRemove = header(nom);
    
    int colRemoveNumber = columnNumber(nom);
    //int numRemoveHeader = m_lcabecera.indexOf(headRemove);
    
   // fprintf(stderr, "\n" + QString::number(colRemoveNumber).toLatin1()  );


   // fprintf(stderr, "\n" + QString::number(m_lcabecera.indexOf(headRemove)).toLatin1()  );


    //fprintf(stderr, "\n" + headRemove->fieldName().toLatin1() );

    m_lcabecera.removeAt(m_lcabecera.indexOf(headRemove) );
    mui_listcolumnas->removeRow(colRemoveNumber );
    //printf(stderr, "\n 2" );
    mui_list->removeColumn(colRemoveNumber -1 );
//     //fprintf(stderr, "\n 3" );
    //delete headRemove;

}


/** Establece el valor de toda la columna.
*/
/**
\param campo
\param valor
**/
void BlSubForm::setColumnValue ( QString campo, QString valor )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubForm::setColumnValue", 0, campo + " -- " + valor );
    BlDbSubFormRecord *rec;

    for ( int i = 0; i < mui_list->rowCount(); ++i ) {
        rec = lineaat ( i );
        if ( rec ) {
            rec->setDbValue ( campo, valor );
        } // end if
    } // end for
    
}


///
/**
\param campo
\return
**/
BlFixed BlSubForm::sumarCampo(QString campo)
{
    BL_FUNC_DEBUG

    /// TODO. Detectar tipo de campo para saber si se puede sumar.

    BlFixed total{"0"};
    for (int i{0}; i < mui_list->rowCount(); ++i)
    {
        BlDbSubFormRecord *rec = lineaat(i);
        if (rec)
        {
            BlFixed subtotal = BlFixed(rec->dbValue(campo));
            total = total + subtotal;
        } // end if
    } // end for

    return total;
}


///
/**
\param campo
\param row
**/
QString BlSubForm::dbValue ( const QString &campo, int row )
{
    BL_FUNC_DEBUG
    try {
        BlDbSubFormRecord *rec;
        if ( row == -1 )
            rec = lineaact();
        else
            rec = lineaat ( row );
        if ( rec == NULL )
            throw - 1;
        
        return rec->dbValue ( campo );
    } catch ( ... ) {
        blMsgInfo ( _ ("Fila inexistente" ));
        throw - 1;
    }
}


///
/**
\param campo Nombre de la columna que debe cambiarse.
\param row   Fila correspondiente a la casilla
\param valor Valor que tomara la casilla
**/
void BlSubForm::setDbValue ( const QString &campo, int row, const QString &valor )
{
    BL_FUNC_DEBUG
    try {
      
        BlDbSubFormRecord *rec;
        if ( row == -1 )
            rec = lineaact();
        else
            rec = lineaat ( row );
        if ( rec == NULL )
            throw - 1;

        rec->setDbValue ( campo, valor );
        
    } catch ( ... ) {
        blMsgInfo ( _ ("Fila inexistente" ) );
        throw - 1;
    }
}


///
/**
\return
**/
int BlSubForm::save()
{
    BL_FUNC_DEBUG
    try {
        BlDbSubFormRecord *rec;
        /// Borramos los elementos marcados para ser borrados.
        while ( !m_listaborrar.isEmpty() ) {
            rec = m_listaborrar.takeFirst();
            if ( rec ) {
                rec->remove();
            } // end if
        } // end while

        /// Asegura que siempre la ultima linea se valide antes de guardar.
        /// Esto evita que se pueda perder informacion.
        if ( campoCompleto ( mui_list->rowCount() - 1 ) ) {
            newRecord();
        } // end if

        /// Si no hay elementos que guardar salimos.
        if ( mui_list->rowCount() == 0 || ( ( mui_list->rowCount() == 1 ) && m_insercion ) ) {
            return 0;
        } // end if

        /// Hacemos el guardado
        for ( int j = 0; j < mui_list->rowCount() - 1; ++j ) {
            rec = lineaat ( j );
            if ( rec ) {
                /// Si hay ordenacion de campos ahora la establecemos
                if ( m_orden ) {
                    rec->setDbValue ( "orden" + m_tablename, QString::number ( j ) );
                } // end if
                rec->refresh();
                rec->save();
            } // end if
        } // end for

        /// Si no hay modo insercion hacemos el guardado de la ultima linea.
        if ( !m_insercion ) {
            rec = lineaat ( mui_list->rowCount() - 1 );
            if ( m_orden )
                rec->setDbValue ( "orden" + m_tablename, QString::number ( mui_list->rowCount() - 1 ) );
            rec->refresh();
            rec->save();
        } // end if

        /// Liberamos memoria
        while ( !m_listaborrar.isEmpty() ) {
            rec = m_listaborrar.takeFirst();
            if ( rec ) {
                delete rec;
            } // end if
        } // end while

        
        return 0;
    } catch ( int e ) {
        if ( e == 100 ) {
	    
            throw - 1;
        } // end if
    } catch ( ... ) {
	
        blMsgError ( _ ("Error inesperado en el guardado. [BlSubForm::guardar]" ) );
        throw - 1;
    } // end try
    return -1;
}


int BlSubForm::deleteCurrentRow() {
      BL_FUNC_DEBUG
      return remove(currentRow());
}

///
/**
\return
**/
int BlSubForm::remove()
{
    BL_FUNC_DEBUG
    BlDbSubFormRecord *rec;
    int i = 0;
    int error = 0;

    /// Si la lista esta vacia no podemos borrar.
    if ( m_lista.count() <= 0 ) {
        
        return error;
    } // end if

    try {
        for ( rec = m_lista.at ( i++ ); i < m_lista.count(); rec = m_lista.at ( i++ ) ) {
            error = rec->remove();
            if ( error )
                return -1;
        } // end for
        if ( !m_insercion ) {
            rec = m_lista.at ( m_lista.count() - 1 );
            error = rec->remove();
        } // end if
        
        return error;
    } catch ( ... ) {
	
        blMsgError ( _ ("Error al borrar. [BlSubForm::borrar]" ));
        return -1;
    } // end try
}


///
/**
\param row
\return
**/
int BlSubForm::remove ( int row )
{
    BL_FUNC_DEBUG
    try {
        BlDbSubFormRecord *rec, *rac;
        BlDbSubFormField *camp;

        rac = new BlDbSubFormRecord ( mainCompany() );

        /// Cogemos el elemento correspondiente usar lineaat
        rec = lineaat ( row );
        if ( !rec )
            return -1;

        /// Agregamos el elemento a la lista de borrados
        m_listaborrar.append ( rac );
        m_lista.takeAt ( m_lista.indexOf ( rec ) );

        rac->setDbTableName ( rec->tableName() );
        rac->setDbFieldId ( rec->fieldId() );
        rac->setNuevo ( false );

        /// Sacamos celda a celda toda la fila
        for ( int i = 0; i < mui_list->columnCount(); i++ ) {
            camp = ( BlDbSubFormField * ) mui_list->item ( row, i );
            BlDbSubFormField *it = new BlDbSubFormField ( rac, mainCompany(), camp->fieldName(), camp->fieldType(), camp->fieldRestrictions(), camp->fieldTitle() );
            rac->lista() ->append ( it );
            it->set ( camp->fieldValue() );
        } // end for

        /// Nos aseguramos que ningun campo de la fila a borrar este en modo edicion.
        /// Evitamos que falle el programa.
        cerrarEditor();

        mui_list->removeRow ( row );
        g_plugins->run("BlSubForm_rowRemoved", this);
        delete rec;

        /// Comprueba que no haya ninguna linea en el subformulario y crea una en blanco.
        if ( m_insercion == true && rowCount() == 0 ) {
            newRecord();
        } // end if
        
        /// Sirve para que se recalcule los totales después de borrar la línea.
        emit editFinish(0, 0);

        /// Terminamos
        
        return 0;

    } catch ( ... ) {
	
        blMsgInfo ( _ ("Error al intentar borrar" ) );
        throw - 1;
    } // end try
}


/// Metodo para ser derivado.
/**
\return
**/
int BlSubForm::cerrarEditor()
{
    
    return 0;
}


/// Sacamos cual es el archivo en el que guardar/cargar configuraciones
const QString BlSubForm::nameFileConfig() 
{
   QString directorio = g_confpr->value(CONF_DIR_USER);
   if (g_confpr->value(CONF_GLOBAL_CONFIG_USER) == "TRUE") {
      directorio = g_confpr->value(CONF_DIR_CONFIG);
   } // end if

   QString empresa = mainCompany()->dbName();
   if (g_confpr->value(CONF_GLOBAL_CONFIG_COMPANY) == "TRUE") {
      empresa  = "";
   } // end if

  QString nombre = directorio + m_fileconfig + "_" + empresa + "_" + QString::number ( m_modo ) + "_tablecfn.cfn" ;
  return nombre;
}

/// Sacamos cual es el archivo de configuraciones por defecto en caso de que no exista un archivo de configuraciones.
/// Lo usamos para mejorar la presentacion en la primera ejecucion ya que la primera impresion es la que queda.
const QString BlSubForm::nameFileDefaultConfig() 
{
   QString directorio = g_confpr->value(CONF_DIR_DEFAULT_CONFS);

  QString nombre = directorio + m_fileconfig + "_" + QString::number ( m_modo ) + "_tablecfn.cfn" ;
  return nombre;
}


/// Guardamos el archivo de configuracion.
/**
**/
void BlSubForm::saveConfig()
{
    BL_FUNC_DEBUG
    saveConfigXML();
    
}


/// Guardamos el archivo de configuracion.
/**
**/
void BlSubForm::saveConfigXML()
{
    BL_FUNC_DEBUG

    /// Si el subformulario no esta inicializado no hacemos el guardado.
    if ( ! mainCompany() )
        return;

    QString aux = "";
    QFile file ( nameFileConfig() );
    /// Guardado del orden y de configuraciones varias.
    if ( file.open ( QIODevice::WriteOnly ) ) {
        QTextStream stream ( &file );
	stream << "<DOCUMENT>" << "\n";
        stream << "<COLORDEN>" << mui_list->columnOrder() << "</COLORDEN>\n";
        stream << "<TIPOORDEN>" << mui_list->orderType() << "</TIPOORDEN>\n";
        stream << "<FILASPAGINA>" << mui_filaspagina->text() << "</FILASPAGINA>\n";

        /// Guarda la visibilidad de los elementos consultando la tabla de configuracion.
	stream << "<VISIBILIDAD>\n";
        for ( int i = 0; i < mui_listcolumnas->rowCount(); ++i ) {
            for ( int j = 0; j < mui_listcolumnas->rowCount(); ++j ) {
                if ( mui_listcolumnas->item ( j, 3 )->text().toInt() == i ) {
		    stream << "\t<VISIBLECOLUMNA name=\"" << mui_listcolumnas->item(j,1) -> text() << "\" >";
                    if ( mui_listcolumnas->item ( j, 0 ) ->checkState() == Qt::Checked ) {
                        stream << "1";
                    } else {
                        stream << "0";
                    } // end if
                    stream << "</VISIBLECOLUMNA>\n";
                } // end if
            } // end for
        } // end for
	stream << "</VISIBILIDAD>\n";

        /// Guarda la configuracion de mui_list.
        stream << "<HORIZONTALHEADER>" << QString ( mui_list->horizontalHeader()->saveState().toBase64() ) << "</HORIZONTALHEADER>\n";
        
	/// 12/Junio/2009:
        /// TODO: Se comentan las lineas que guardan el estado vertical porque da fallos en
        /// algunas circunstancias y no tiene en cuenta el registro (BD) concreto al que afecta la
        /// configuracion.
        
        //stream << QString ( mui_list->verticalHeader()->saveState().toBase64() ) << "\n";

        /// Guarda la configuracion de mui_listcolumnas.
        stream << "<COLUMNASHORIZONTALHEADER>"<< QString ( mui_listcolumnas->horizontalHeader()->saveState().toBase64() ) << "</COLUMNASHORIZONTALHEADER>\n";
        //stream << QString ( mui_listcolumnas->verticalHeader()->saveState().toBase64() ) << "\n";
	stream << "<ORDENCOLUMNAS>\n";
        /// Guarda el orden de la lista de elementos en mui_listcolumnas.
        for ( int i = 0; i < mui_listcolumnas->rowCount(); ++i ) {
	    stream << "\t<ORDENCOLUMNA name=\"" << mui_listcolumnas->item(i,1) -> text() << "\" >";
            stream << mui_listcolumnas->item ( i, 3 )->text();
	    stream << "</ORDENCOLUMNA>\n";
        } // end for
	stream << "</ORDENCOLUMNAS>\n";
	stream << "<VISIBILIDADMENU>";
	/// Guardamos el estado del menu de subformulario.
	if (mui_menusubform->isHidden()) {
	  stream << "0";
	} else {
	  stream << "1";
	} // end if
	stream << "</VISIBILIDADMENU>\n";
	stream << "</DOCUMENT>\n";

        file.close();
    } // end if
    
    
}


///
/**
**/
void BlSubForm::loadConfig()
{
    BL_FUNC_DEBUG

    loadConfigXML();

    m_primero = false;

    
}



///
/**
**/
void BlSubForm::loadConfigXML()
{
    BL_FUNC_DEBUG
    QFile file ( nameFileConfig() );
    if(!file.exists())
      file.setFileName(nameFileDefaultConfig());
    
    
    QDomDocument doc ( "mydocument" );
    if ( !file.open ( QIODevice::ReadOnly ) ) {
	
        return;
    }
    if ( !doc.setContent ( &file ) ) {
        file.close();
	
        return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    
    
	/// Establecemos el orden de columna
	QString columorden = docElem.firstChildElement ( "COLORDEN" ).toElement().text();
	if (columorden != "")
	  mui_list->setColumnOrder ( columorden.toInt() );

        /// Establecemos el tipo de ordenaci&oacute;n
	QString orderType = docElem.firstChildElement ( "TIPOORDEN" ).toElement().text();
	if (orderType != "")
	  mui_list->setOrderType ( orderType.toInt() );

        /// Establecemos el n&uacute;mero de filas por p&aacute;gina
	QString filaspagina = docElem.firstChildElement ( "FILASPAGINA" ).toElement().text();
	if (filaspagina.toInt() > 0) {
            mui_filaspagina->setValue ( filaspagina.toInt() );
        } // end if

    /// Leemos la visibilidad de las columnas. Se hace antes de ordenarlas.
    QDomNodeList nodos = docElem.elementsByTagName ( "VISIBLECOLUMNA" );
    for ( int i = 0; i < nodos.count(); i++ ) {
        QDomNode visible = nodos.item ( i );
        QDomElement e1 = visible.toElement(); /// try to convert the node to an element.
        if ( !e1.isNull() ) { /// the node was really an element.
	    QString nombre = e1.attribute("name");
	    QString linea = e1.text();
	    for ( int j = 0; j < mui_listcolumnas->rowCount(); ++j ) {
		if ( mui_listcolumnas->item(j,1) -> text() == nombre) {
		    if ( linea == "1" ) {
			mui_listcolumnas->item ( j, 0 ) ->setCheckState ( Qt::Checked );
		    } else {
			mui_listcolumnas->item ( j, 0 ) ->setCheckState ( Qt::Unchecked );
		    } // end if
		} // end if
	    } // end for
	} // end if
    } // end form

        /// Restaura el estado de mui_list.
	QString horizontalheader = docElem.firstChildElement ( "HORIZONTALHEADER" ).toElement().text();
	if (horizontalheader != "") {
	    /// Numero de secciones (columnas) en la tabla antes de restaurar su estado original.
	    int seccionesInicial = mui_list->horizontalHeader()->count ();
	    
	    mui_list->horizontalHeader()->restoreState ( QByteArray::fromBase64 ( horizontalheader.toLatin1() ) );
	    
	    /// Numero de secciones (columnas) despues de restaurar su estado original.
	    int seccionesFinal = mui_list->horizontalHeader()->count ();
	    
	    /// Si el numero de secciones (columnas) antes y despues de restaurar no son iguales entonces
	    /// la configuracion de columnas ha cambiado o no corresponde con la estructura de la tabla
	    /// y por tanto se tiene que configurar la tabla por defecto.
	    if (seccionesInicial != seccionesFinal) {
		mui_list->horizontalHeader()->reset();
	    } // end if
	    
	} // end if


        /// Restaura el estado de mui_listcolumnas.
        /// Restaura el estado de mui_list.
	QString columnashorizontalheader = docElem.firstChildElement ( "COLUMNASHORIZONTALHEADER" ).toElement().text();
	if (columnashorizontalheader != "") {
	    mui_listcolumnas->horizontalHeader()->restoreState ( QByteArray::fromBase64 ( columnashorizontalheader.toLatin1() ) );
	} // end if

	    
    /// Leemos la visibilidad de las columnas. Se hace antes de ordenarlas.
    nodos = docElem.elementsByTagName ( "ORDENCOLUMNA" );
    for ( int i = 0; i < nodos.count(); i++ ) {
        QDomNode visible = nodos.item ( i );
        QDomElement e1 = visible.toElement(); /// try to convert the node to an element.
        if ( !e1.isNull() ) { /// the node was really an element.
	    QString nombre = e1.attribute("name");
	    QString linea = e1.text();
	    for ( int j = 0; j < mui_listcolumnas->rowCount(); ++j ) {
		if ( mui_listcolumnas->item(j,3) -> text().toInt() == linea.toInt()) {
                    mui_listcolumnas->moveRow ( j, i );
		} // end if
	    } // end for
	} // end if
    } // end for


	/// Guardamos el estado del menu de subformulario.
        /// Restaura el estado de mui_listcolumnas.
        /// Restaura el estado de mui_list.
	QString visibilidadmenu = docElem.firstChildElement ( "VISIBILIDADMENU" ).toElement().text();
	if (visibilidadmenu == "1") {
	  mui_menusubform->setVisible(true);
	} else {
	  mui_menusubform->setVisible(false);
	} // end if

        on_mui_confcol_clicked();

    m_primero = false;
    
}



///
/**
**/
void BlSubForm::on_mui_confcol_clicked()
{
    BL_FUNC_DEBUG
    
    for ( int i = 0; i < mui_listcolumnas->rowCount(); ++i ) {
        if ( mui_listcolumnas->item ( i, 0 ) ->checkState() == Qt::Checked ) {
            mui_list->showColumn ( mui_listcolumnas->item ( i, 3 )->text().toInt() );
            headerList()->at(i)->setOptions( headerList()->at(i)->options() & ~BlSubFormHeader::DbHideView );
        } else {
            /// Coge el valor de la columna de 'order' para ocultarla.
            mui_list->hideColumn ( mui_listcolumnas->item ( i, 3 )->text().toInt() );
            headerList()->at(i)->setOptions( headerList()->at(i)->options() | BlSubFormHeader::DbHideView );
        } // end if

    } // end for
    
}


///
/**
**/
void BlSubForm::bpressedSlash ( ) {
  BL_FUNC_DEBUG
  on_mui_list_pressedSlash(currentRow(), currentColumn());
  
}

///
/**
**/
void BlSubForm::bSubir ( ) {
  BL_FUNC_DEBUG
  on_mui_list_ctrlUp(currentRow(), currentColumn());
  
}

///
/**
**/
void BlSubForm::bBajar ( ) {
  BL_FUNC_DEBUG
  on_mui_list_ctrlDown(currentRow(), currentColumn());
  
}


///
/**
**/
void BlSubForm::on_mui_list_pressedSlash ( int row, int col )
{
    BL_FUNC_DEBUG
    
    BlDbSubFormRecord *rec = lineaat ( row );
    if ( rec == NULL ) {
	
        return;
    } // end if

    BlDbSubFormField *camp = ( BlDbSubFormField * ) item ( row, col );
    camp->refresh();

    if ( m_procesacambios ) {

        m_procesacambios = false;
        m_prevRow = row;
        m_prevCol = col;
        pressedSlash ( row, col, rec, camp );
        emit pushSlash ( row, col );
        m_procesacambios = true;
        /// Invocamos la finalizacion de edicion para que todos los campos se actualicen.
        on_mui_list_cellChanged ( row, col );
    } // end if
    
}


///
/**
**/
void BlSubForm::on_mui_list_pressedAsterisk ( int row, int col )
{
    BL_FUNC_DEBUG
    BlDbSubFormRecord *rec = lineaat ( row );
    if ( rec == NULL ) {
	
        return;
    } // end if

    BlDbSubFormField *camp = ( BlDbSubFormField * ) item ( row, col );
    camp->refresh();

    if ( m_procesacambios ) {
        m_procesacambios = false;
        m_prevRow = row;
        m_prevCol = col;
        pressedAsterisk ( row, col, rec, camp );
        emit pushAsterisk ( row, col );
        m_procesacambios = true;
        /// Invocamos la finalizacion de edicion para que todos los campos se actualicen.
        on_mui_list_cellChanged ( row, col );
    } // end if
    
}


///
/**
**/
void BlSubForm::on_mui_list_pressedPlus ( int row, int col )
{
    BL_FUNC_DEBUG
    BlDbSubFormRecord *rec = lineaat ( row );
    if ( rec == NULL ) {
	
        return;
    } // end if

    BlDbSubFormField *camp = ( BlDbSubFormField * ) item ( row, col );
    camp->refresh();

    if ( m_procesacambios ) {
        m_procesacambios = false;
        m_prevRow = row;
        m_prevCol = col;
        pressedPlus ( row, col, rec, camp );
        emit pushPlus ( row, col );
        m_procesacambios = true;
        /// Invocamos la finalizacion de edicion para que todos los campos se actualicen.
        on_mui_list_cellChanged ( row, col );
    } // end if
    
}


///
/**
**/
void BlSubForm::on_mui_list_pressedMinus ( int row, int col )
{
    BL_FUNC_DEBUG
    BlDbSubFormRecord *rec = lineaat ( row );
    if ( rec == NULL ) {
	
        return;
    } // end if

    BlDbSubFormField *camp = ( BlDbSubFormField * ) item ( row, col );
    camp->refresh();

    if ( m_procesacambios ) {
        m_procesacambios = false;
        m_prevRow = row;
        m_prevCol = col;
        pressedMinus ( row, col, rec, camp );
        emit pushMinus ( row, col );
        m_procesacambios = true;
        /// Invocamos la finalizacion de edicion para que todos los campos se actualicen.
        on_mui_list_cellChanged ( row, col );
    } // end if
    
}


///
/**
\return
**/
QString BlSubForm::imprimir()
{
    BL_FUNC_DEBUG
    BlProgressBar barra;
    barra.show();
    barra.setRange ( 0, mui_listcolumnas->rowCount() + mui_list->columnCount() - 2 );
    barra.setText ( _ ( "Imprimiendo " ) +  m_tablename );
    barra.setValue ( 0 );
    QLocale::setDefault ( QLocale ( QLocale::Spanish, QLocale::Spain ) );
    QLocale spanish;

    QString rowStyle = "";

    QList<QString> headerTitles;
    for ( int h = 0; h < mui_listcolumnas->rowCount(); ++h )
    {
        if ( mui_listcolumnas->item ( h, 0 ) ->checkState() == Qt::Checked ) {
            headerTitles.push_back(mui_listcolumnas->item ( h, 0 ) ->text());
        } // end if
    } // end for


    int maxColumnsPerRow = 7; /// TODO: que se pueda configurar desde una directiva
    bool showNumLines = true;
    bool showInfoColumn = false;
    if (headerTitles.count() > maxColumnsPerRow)
    {
        showInfoColumn = true;
    } // end if


    int counter = 0;
    int totalColumns = 0;
    int totalRows = 0;
    int printedCounter = 0;
    int infoRowCounter = 0;

    /// Cabecera
    QString fitxersortidarml = "<tr style=\"cabecera\">\n";
    bool numLinesState = true;
    for (int i = 0; i < headerTitles.count(); ++i)
    {
        if (counter)
        {
          fitxersortidarml += "<td>";
          counter++;
        } else {
          if (showNumLines)
          {
            if (numLinesState)
            {
              fitxersortidarml += QString("<td>#") + blXMLEncode(_("Fila"))  + QString("</td>");
              numLinesState = false;
            } else {
              fitxersortidarml += "<td></td>";
            } // end if
          } // end if

          if (showInfoColumn) 
          {
            /// Numero de fila de la misma fila.
            fitxersortidarml += QString("<td>## ") + QString::number(infoRowCounter)  + QString("</td><td>");
            infoRowCounter++;
          } else {
            fitxersortidarml += "<td>";
          }

          counter++;
        } // end if

        totalColumns++;
        printedCounter++;
        fitxersortidarml += QString("<para style=\"cabecera_color\">") + blXMLEncode(headerTitles.at(i)) + QString("</para></td>");

        if (printedCounter >= maxColumnsPerRow)
        {
            printedCounter = 0;
            counter = 0;
            fitxersortidarml += QString("</tr><tr style=\"cabecera\">\n");
        } // end if
    } // end for

    fitxersortidarml += QString("</tr>\n");
    totalRows++;
    numLinesState = true;

    
    /// Filas de datos
    for (int i = 0; i < mui_list->rowCount(); ++i)
    {
        barra.setValue (barra.value() + 1);
        qApp->processEvents();

        counter = 0;
        printedCounter = 0;
        totalColumns = 0;
        infoRowCounter = 0;

        if (i % 2)
        {
          rowStyle = "fila_impar";
        } else {
          rowStyle = "fila_par";
        } // end if

        fitxersortidarml += QString("<tr style=\"%1\">\n").arg(rowStyle);
        for (int j = 0; j < mui_listcolumnas->rowCount(); ++j)
        {
            if (mui_listcolumnas->item ( j, 0 )->checkState() == Qt::Checked)
            {
                BlDbSubFormField *valor = ( BlDbSubFormField * ) mui_list->item ( i, mui_listcolumnas->item ( j, 3)->text().toInt() );
                FormatField data = formatFieldTableStory(valor, spanish);

                if (counter)
                {
                    if (data.style.isEmpty())
                    {
                        fitxersortidarml += QString("<td>");
                    } else {
                        fitxersortidarml += QString("<td style=\"%1\">").arg(data.style);
                    } // end if
                    counter++;
                } else {
                    if (showNumLines)
                    {
                        if (numLinesState)
                        {
                            fitxersortidarml += QString("<td>") + QString::number(totalRows)  + QString("</td>");
                            numLinesState = false;
                        } else {
                            fitxersortidarml += QString("<td></td>");
                        } // end if
                    } // end if

                    if (showInfoColumn) 
                    {
                        fitxersortidarml += QString("<td>") + QString::number(infoRowCounter)  + QString("</td>");
                        if (data.style.isEmpty())
                        {
                            fitxersortidarml += QString("<td>");
                        } else {
                            fitxersortidarml += QString("<td style=\"%1\">").arg(data.style);
                        } // end if
                        infoRowCounter++;
                    } else {
                        if (data.style.isEmpty())
                        {
                            fitxersortidarml += QString("<td>");
                        } else {
                            fitxersortidarml += QString("<td style=\"%1\">").arg(data.style);
                        } // end if
                    } // end if

                    counter++;
                } // end if
        
                totalColumns++;
                printedCounter++;
                fitxersortidarml += QString("<para>") + data.text + QString("</para></td>");
        
                if (printedCounter >= maxColumnsPerRow)
                {
                    printedCounter = 0;
                    counter = 0;
                    fitxersortidarml += QString("</tr>\n");
                    fitxersortidarml += QString("<tr style=\"%1\">\n").arg(rowStyle);
                } // end if
            } // end if
        } // end for

        fitxersortidarml += QString("</tr>\n");
        totalRows++;
        numLinesState = true;
    } // end for

    return fitxersortidarml;
}


/// Genera las etiquetas RML necesarias para poder mostrar un dato con el estilo adecuado a su tipo
/// a la hora de imprimirlo como una celda del "BlockTable" ubicado en el "story" del informe "listado"
/**
  \param value
  \param spanish
\return
**/
BlSubForm::FormatField BlSubForm::formatFieldTableStory(BlDbSubFormField *value, QLocale spanish)
{
    BL_FUNC_DEBUG
    
    FormatField formatField;
    
    /// Valor num&eacute;rico o fecha
    if ( ( value->fieldType() == BlDbField::DbInt )
      || ( value->fieldType() == BlDbField::DbNumeric )
      || ( value->fieldType() == BlDbField::DbDate ) )
    {
        formatField.style = "number";

        /// Si es un n&uacute;mero con decimales, tenerlo en cuenta
        if ( value->fieldType() & BlDbField::DbNumeric )
        {
            int prec = value->text().section(",", 1).count();
            formatField.text += spanish.toString ( value->text().toDouble(), 'f', prec );
        }
        else
            formatField.text += blXMLEncode ( value->text() );
    }

    /// Valor booleano
    else if ( value->fieldType() == BlDbField::DbBoolean )
    {
        /// Mostrar un "puntazo" centrado si el valor es verdadero
        if ( value->checkState() == Qt::Checked )
        {
           formatField.style = "checked";
           formatField.vAlign = "middle";
           formatField.text +=  "&#9679;" ;
        }
    }

    /// Texto
    else
    {
        formatField.style = "text";
        formatField.text += blXMLEncode ( value->text() );
    } // end if

    return formatField;
}


///
/**
\return
**/
void BlSubForm::on_mui_confquery_clicked()
{
    BL_FUNC_DEBUG
    if ( mainCompany() == NULL ) {
        blMsgInfo ( _ ("no se ha inicializado bien la clase" ) );
        return;
    } // end if
    mui_paginaact->setValue ( 1 );
    load ( mui_query->toPlainText() );
//  load(m_query);
    
}


///
/**
\return
**/
void BlSubForm::confquery()
{
    BL_FUNC_DEBUG
    if ( mainCompany() == NULL ) {
        blMsgInfo ( _ ("no se ha inicializado bien la clase" ) );
        return;
    } // end if

    load ( m_query );

    
}


/// Disparador que se activa al haber pulsado ctrl+Arriba en la tabla
/// Hace el intercambio con la fila inmediatamente superior.
/**
\param row
\param col
\return
**/
void BlSubForm::on_mui_list_ctrlUp ( int row, int col )
{
    BlDebug::blDebug ( "BlSubForm::on_mui_list_ctrlUp", 0, " (" + QString::number ( row ) + "," + QString::number ( col ) + ")" );
    if ( ! m_orden )
        return;
    if ( row >= mui_list->rowCount() - 1 || row == 0 )
        return;


    /// Desactivamos el sorting debido a un error en las Qt4
    mui_list->setSortingEnabled ( false );
    mui_list->setCurrentCell ( 0, 0 );

    m_procesacambios = false;

    for ( int i = 0; i < mui_list->columnCount(); ++i ) {
        QTableWidgetItem *it = mui_list->takeItem ( row, i );
        QTableWidgetItem *ot = mui_list->takeItem ( row - 1, i );
        mui_list->setItem ( row, i, ot );
        mui_list->setItem ( row - 1, i, it );
    } // end for
    mui_list->setCurrentCell ( row - 1, col );
    /// Desactivamos el sorting debido a un error en las Qt4
    mui_list->setSortingEnabled ( m_sorting );

    m_procesacambios = true;
    
}


/// Disparador que se activa al haber pulsado ctrl+Abajo en la tabla
/// Hace el intercambio con la fila inmediatamente inferior.
/**
\param row
\param col
\return
**/
void BlSubForm::on_mui_list_ctrlDown ( int row, int col )
{
    BL_FUNC_DEBUG
    if ( ! m_orden )
        return;
    if ( row >= mui_list->rowCount() - 2 )
        return;
    /// Desactivamos el sorting debido a un error en las Qt4
    mui_list->setSortingEnabled ( false );
    m_procesacambios = false;

    if ( row != 0 )
        mui_list->setCurrentCell ( 0, 0 );
    else
        mui_list->setCurrentCell ( mui_list->rowCount() - 1, 0 );

    for ( int i = 0; i < mui_list->columnCount(); ++i ) {
        QTableWidgetItem *it = mui_list->takeItem ( row, i );
        mui_list->setItem ( row, i, mui_list->takeItem ( row + 1, i ) );
        mui_list->setItem ( row + 1, i, it );
    } // end for
    mui_list->setCurrentCell ( row + 1, col );
    /// Desactivamos el sorting debido a un error en las Qt4
    mui_list->setSortingEnabled ( m_sorting );
    m_procesacambios = true;

    
}


///
/**
**/
void BlSubForm::on_mui_pagsiguiente_clicked()
{
    BL_FUNC_DEBUG
    int pag = mui_paginaact->text().toInt();
    pag++;
    mui_paginaact->setValue ( pag );
    confquery();
    
}

///
/**
**/
void BlSubForm::on_mui_paganterior_clicked()
{
    BL_FUNC_DEBUG
    int pag = mui_paginaact->text().toInt();
    if ( pag > 1 )
        pag--;
    mui_paginaact->setValue ( pag );
    confquery();
    
}


///
/**
\param titular
**/
void BlSubForm::printPDF ( const QString &titular )
{
    BL_FUNC_DEBUG

    /// Los listados siempre usan la misma plantilla para imprimir listado.
    QString archivo = g_confpr->value( CONF_DIR_OPENREPORTS ) + "listado.rml";
//    QString archivo = g_confpr->value( CONF_DIR_OPENREPORTS ) + titular + ".rml";
    QString archivod = g_confpr->value( CONF_DIR_USER ) + "listado.rml";
//    QString archivod = g_confpr->value( CONF_DIR_USER ) + titular + ".rml";

    /// Copiamos el archivo.
    if(!blCopyFile(archivo, archivod)){
        blMsgError(_("Errr al copiar listado.rml [ BlSubForm->printPDF() ]"));
    } // end if

    /// Copiamos todos los archivos de imagenes para usarlos en la plantilla.
    /// TODO: Esto no discrimina si el archivo será finalmente usado o no. Así es poco eficiente.
    QStringList nameFilter;
    nameFilter << QString("*.jpg");
    nameFilter << QString("*.png");
    QDir directory(g_confpr->value(CONF_DIR_OPENREPORTS));
    QStringList values = directory.entryList(nameFilter, QDir::Files | QDir::Readable);

    for (int i{0}; i < values.size(); ++i)
    {
        QString archivoImagen = values.at(i);
        QString archivoOrigen{g_confpr->value(CONF_DIR_OPENREPORTS) + archivoImagen};
        QString archivoDestino{g_confpr->value(CONF_DIR_USER) + archivoImagen};

        if(!blCopyFile(archivoOrigen, archivoDestino))
        {
            blMsgError(_("[ BlSubForm->printPDF() ] Error al copiar el archivo:") + archivoImagen);
        } // end if
    } // end if

    QFile file;
    file.setFileName ( archivod );
    file.open ( QIODevice::ReadOnly );

    QTextStream stream ( &file );
    QString buff = stream.readAll();
    file.close();
    QString fitxersortidatxt;

    fitxersortidatxt = "<blockTable style=\"tabla\" repeatRows=\"0\">";
    fitxersortidatxt += imprimir();
    fitxersortidatxt += "</blockTable>";

    /// Reemplazamos en la impresion el parametro [story] por el resultado de la impresion
    buff.replace ( "[story]", fitxersortidatxt );
    /// La plantilla de listados admite unicamente el parametro titular que es lo que cambia
    /// en todos los listados
    buff.replace ( "[titulo]", blXMLEncode(titular) );

    /// Ofrecer el uso de unas variables con la fecha y hora de impresi&oacute;n (por ejemplo, en el pie)
    buff.replace ( "[fecha_actual]", QDate::currentDate().toString ( Qt::DefaultLocaleShortDate ) );
    buff.replace ( "[hora_actual]", QTime::currentTime().toString ( "HH:mm:ss" ) );

    if ( file.open ( QIODevice::WriteOnly ) ) {

        QTextStream stream ( &file );
        stream << buff;

        file.close();
    } // end if

    blCreateAndLoadPDF ( "listado" );
    
}

void BlSubForm::preparaMenu() {
    BL_FUNC_DEBUG

    /// Disparamos los plugins.
    g_plugins->run ( "BlSubForm_preparaMenu", this );
    BlUpgradePreview *m_upgrade = new BlUpgradePreview(this);
    m_upgrade->BlSubForm_preparaMenu(this);

    QHBoxLayout *m_hboxLayout1 = mui_menusubform->findChild<QHBoxLayout *> ( "hboxLayout1" );
    if ( !m_hboxLayout1 ) {
        m_hboxLayout1 = new QHBoxLayout ( mui_menusubform );
        m_hboxLayout1->setSpacing (0 );
        m_hboxLayout1->setMargin ( 0 );
        m_hboxLayout1->setObjectName ( QString::fromUtf8 ( "hboxLayout1" ) );
    } // end if
    
    if ( m_delete ) {
      QToolButton *sel = new QToolButton ( mui_menusubform );
      sel->setStatusTip ( "Borrar Linea" );
      sel->setToolTip ( "Borrar Linea" );
      sel->setMinimumSize ( QSize ( 18, 18 ) );
      sel->setIcon ( QIcon ( ":/Images/delete-one-line.png" ) );
      sel->setIconSize ( QSize ( 18, 18 ) );    
      m_hboxLayout1->addWidget ( sel );
      connect (sel, SIGNAL(released()), this, SLOT(deleteCurrentRow()));
    } // end if
    
    if (m_modo == BL_EDIT_MODE)
    {
      /// Editar en menu aparte
      QToolButton *sel9 = new QToolButton ( mui_menusubform );
      sel9->setStatusTip ( "Usar Editor" );
      sel9->setToolTip ( "Usar Editor" );
      sel9->setMinimumSize ( QSize ( 18, 18 ) );
      sel9->setIcon ( QIcon ( ":/Images/edit_edit.png" ) );
      sel9->setIconSize ( QSize ( 18, 18 ) );    
      m_hboxLayout1->addWidget ( sel9 );    
      connect (sel9, SIGNAL(released()), this, SLOT(bpressedSlash ( )));   

      /// Subir registro
      QToolButton *sel90 = new QToolButton ( mui_menusubform );
      sel90->setStatusTip ( "Subir Registro" );
      sel90->setToolTip ( "Subir Registro" );
      sel90->setMinimumSize ( QSize ( 18, 18 ) );
      sel90->setIcon ( QIcon ( ":/Images/edit_up.png" ) );
      sel90->setIconSize ( QSize ( 18, 18 ) );    
      m_hboxLayout1->addWidget ( sel90 );    
      connect (sel90, SIGNAL(released()), this, SLOT(bSubir ( )));   

      /// Subir registro
      QToolButton *sel91 = new QToolButton ( mui_menusubform );
      sel91->setStatusTip ( "Bajar Registro" );
      sel91->setToolTip ( "Bajar Registro" );
      sel91->setMinimumSize ( QSize ( 18, 18 ) );
      sel91->setIcon ( QIcon ( ":/Images/edit_down.png" ) );
      sel91->setIconSize ( QSize ( 18, 18 ) );    
      m_hboxLayout1->addWidget ( sel91 );    
      connect (sel91, SIGNAL(released()), this, SLOT(bBajar ( )));   
    } // end if
    
    /// Ajuste de la columna
    QToolButton *sel1 = new QToolButton ( mui_menusubform );
    sel1->setStatusTip ( "Ajustar Columnas" );
    sel1->setToolTip ( "Ajustar Columnas" );
    sel1->setMinimumSize ( QSize ( 18, 18 ) );
    sel1->setIcon ( QIcon ( ":/Images/adjust-all-rows.png" ) );
    sel1->setIconSize ( QSize ( 18, 18 ) );    
    m_hboxLayout1->addWidget ( sel1 );    
    connect (sel1, SIGNAL(released()), this, SLOT(resizeColumnsToContents()));    

    /// Ajuste de la altura
    QToolButton *sel2 = new QToolButton ( mui_menusubform );
    sel2->setStatusTip ( "Ajustar Filas" );
    sel2->setToolTip ( "Ajustar Filas" );
    sel2->setMinimumSize ( QSize ( 18, 18 ) );
    sel2->setIcon ( QIcon ( ":/Images/adjust-one-line.png" ) );
    sel2->setIconSize ( QSize ( 18, 18 ) );    
    m_hboxLayout1->addWidget ( sel2 );    
    connect (sel2, SIGNAL(released()), this, SLOT(resizeRowsToContents()));   
    
    /// Ver configuracion del subformulario
    QToolButton *sel3 = new QToolButton ( mui_menusubform );
    sel3->setStatusTip ( "Ver/Ocultar configurador de subformulario" );
    sel3->setToolTip ( "Ver/Ocultar configurador de subformulario" );
    sel3->setMinimumSize ( QSize ( 18, 18 ) );
    sel3->setIcon ( QIcon ( ":/Images/toggleconfig.png" ) );
    sel3->setIconSize ( QSize ( 18, 18 ) );    
    m_hboxLayout1->addWidget ( sel3 );    
    connect (sel3, SIGNAL(released()), this, SLOT(toogleConfig()));  
    
    /// Ocultar Menu
    QToolButton *sel4 = new QToolButton ( mui_menusubform );
    sel4->setStatusTip ( "Ver/Ocultar menu de subformulario" );
    sel4->setToolTip ( "Ver/Ocultar menu de subformulario" );
    sel4->setMinimumSize ( QSize ( 18, 18 ) );
    sel4->setIcon ( QIcon ( ":/Images/togglemenu.png" ) );
    sel4->setIconSize ( QSize ( 18, 18 ) );    
    m_hboxLayout1->addWidget ( sel4 );    
    connect (sel4, SIGNAL(released()), this, SLOT(toogleMenuConfig())); 
    
    
}



///
/**
\return
**/
void BlSubForm::contextMenuEvent ( QContextMenuEvent * )
{
    BL_FUNC_DEBUG
    QAction *del = NULL;
    QAction *editup = NULL;
    QAction *editdown = NULL;
    QAction *editinslash = NULL;
    QAction *ajustc = NULL;
    QAction *ajustac = NULL;
    QAction *ajust = NULL;
    QAction *ajusta = NULL;
    QAction *menuconfig = NULL;
    QAction *verconfig = NULL;
    QAction *opcion = NULL;
    
    
    int row = currentRow();
    int col = currentColumn();

    QMenu *popup = new QMenu ( this );

    /// Si estamos en modo experto. Lo primero que hacemos es encabezar el menu con el nombre del objeto para tenerlo bien ubicado.
    if (g_confpr->value(CONF_MODO_EXPERTO) == "TRUE") {
      QAction *nombreobjeto = popup->addAction( objectName() );
      nombreobjeto->setDisabled(true);
    } // end if
    
    /// Lanzamos el evento para que pueda ser capturado por terceros.
    emit pintaMenu ( popup );

    /// Lanzamos la propagacion del menu a traves de las clases derivadas.
    createMenu ( popup );

    if ( (row >= 0) && (col >= 0) ) {
    
	if ( m_delete ) {
	    popup->addSeparator();
	    del = popup->addAction ( _ ( "Borrar registro" ) );
	    del->setIcon ( QIcon ( ":/Images/delete-one-line.png" ) );
	} // end if
	popup->addSeparator();

	editup = popup->addAction ( _ ( "Subir fila") );
	editup->setIcon ( QIcon ( ":Images/edit_up.png"));

	editdown = popup->addAction ( _ ( "Bajar fila") );
	editdown->setIcon ( QIcon ( ":Images/edit_down.png"));
	
	editinslash = popup->addAction ( _ ( "Usar editor") );
	editinslash->setIcon ( QIcon ( ":Images/edit_edit.png"));
	
	ajustc = popup->addAction ( _ ( "Ajustar columna" ) );
	ajustc->setIcon ( QIcon ( ":/Images/adjust-one-row.png" ) );
	
	ajustac = popup->addAction ( _ ( "Ajustar altura" ) );
	ajustac->setIcon ( QIcon ( ":/Images/adjust-one-line.png" ) );
	
	ajust = popup->addAction ( _ ( "Ajustar columnas" ) );
	ajust->setIcon ( QIcon ( ":/Images/adjust-all-rows.png" ) );
	
	ajusta = popup->addAction ( _ ( "Ajustar alturas" ) );
	ajusta->setIcon ( QIcon ( ":/Images/adjust-all-lines.png" ) );

	popup->addSeparator();

    } // end if

    menuconfig = popup->addAction ( _ ( "Ver/Ocultar menu de subformulario" ) );
    menuconfig->setIcon ( QIcon ( ":/Images/togglemenu.png" ) );
    
    verconfig = popup->addAction ( _ ( "Ver/Ocultar configurador de subformulario" ) );
    verconfig->setIcon ( QIcon ( ":/Images/toggleconfig.png" ) );
    
    opcion = popup->exec ( QCursor::pos() );

    /// Si no hay ninguna opcion pulsada se sale sin hacer nada
    if ( !opcion ) {
	return;
	
    } // end if
    if ( opcion == del )
        remove ( row );
    if ( opcion == ajust )
        resizeColumnsToContents();
    if ( opcion == ajusta )
        resizeRowsToContents();
    if ( opcion == ajustc )
        resizeColumnToContents ( col );
    if ( opcion == ajustac )
        resizeRowToContents ( row );
    if ( opcion == verconfig )
        toogleConfig();
    if ( opcion == menuconfig )
        toogleMenuConfig();
    if (opcion == editinslash)
	on_mui_list_pressedSlash ( row, col );
    if (opcion == editup)
	on_mui_list_ctrlUp(row, col);
    if (opcion == editdown)
	on_mui_list_ctrlDown(row, col);
    
    emit trataMenu ( opcion );

    /// Activamos las herederas.
    execMenuAction ( opcion );

    delete popup;
    
}

///
/**
**/
void BlSubForm::toogleConfig()
{
    BL_FUNC_DEBUG
    mui_configurador->setVisible ( mui_configurador->isHidden() );
    emit toogledConfig ( mui_configurador->isVisible() );
    
}

///
/**
**/
void BlSubForm::toogleMenuConfig()
{
    BL_FUNC_DEBUG
    mui_menusubform->setVisible ( mui_menusubform->isHidden() );
    emit toogledMenuConfig ( mui_menusubform->isVisible() );
    
}

///
/**
**/
void BlSubForm::on_mui_botonCerrar_clicked()
{
    BL_FUNC_DEBUG
    toogleConfig();
    
}


///
/**
**/
void BlSubForm::on_mui_list_itemChanged ( QTableWidgetItem * )
{
    BL_FUNC_DEBUG
}


///
/**
\param b
**/
void BlSubForm::setInsert ( bool b )
{
    BL_FUNC_DEBUG
    m_insercion = b;
    
}


///
/**
**/
bool BlSubForm::isInsert() {
    BL_FUNC_DEBUG
    
    return m_insercion;
}


/// Establece si el subformulario debe considerar y tratar los cambios de celdas
/**
\param proc
**/
void BlSubForm::setProcesarCambios ( bool proc )
{
    BL_FUNC_DEBUG
    m_procesacambios = proc;
    
}


/// Indica si el subformulario esta considerando el tratamiento de celdas modificadas
/**
\return
**/
bool BlSubForm::procesaCambios()
{
    BL_FUNC_DEBUG
    return m_procesacambios;
}


/// Devuelve el nombre del campo en la base de datos sabiendo que columna representa ese dato.
/**
\return
**/
QString BlSubForm::dbFieldNameByColumnId ( int columna )
{
    BlSubFormHeader * linea;
    linea = m_lcabecera.at ( columna );
    return linea->fieldName();
}


/// Devuelve el nombre visual de la columna indicada
/**
  No usamos directamente la columna actual, ya que en ocasiones puede no ser correcta.
   \return Nombre visual de la columna
**/
QString BlSubForm::dbFieldViewNameByColumnId(int columna)
{
   BL_FUNC_DEBUG
   return m_lcabecera.at(columna)->fieldTitle();
}


/// Devuelve el tipo de la columna indicada
/**
    No usamos directamente la columna actual, ya que en ocasiones puede no ser correcta.
   \return Tipo de datos del campo
**/
BlDbField::DbType BlSubForm::fieldTypeByColumnId(int columna)
{
   BL_FUNC_DEBUG
   return m_lcabecera.at(columna)->fieldType();
}


/// Para ser derivado, permite a las clases derivadas y a esta el tratamiento de cambio de celda.
void BlSubForm::editFinished ( int row, int col, BlDbSubFormRecord *rec, BlDbSubFormField *camp )
{
    BL_FUNC_DEBUG

    m_registrolinea = rec;
    m_campoactual = camp;

    /// Disparamos los plugins.
    g_plugins->run ( "BlSubForm_editFinished", this );


    
}

/// Para ser derivado, permite a las clases derivadas y a esta el tratamiento de cambio de celda.
void BlSubForm::pressedAsterisk ( int, int, BlDbSubFormRecord *rec, BlDbSubFormField *camp )
{
    BL_FUNC_DEBUG
    
    /// Establezco las variables de clase para que los plugins puedan operar.
    m_registrolinea = rec;
    m_campoactual = camp;

    /// Disparamos los plugins.
    g_plugins->run ( "BlSubForm_pressedAsterisk", this );
}


/// Para ser derivado, permite a las clases derivadas y a esta el tratamiento de cambio de celda.
void BlSubForm::pressedPlus ( int, int, BlDbSubFormRecord *, BlDbSubFormField * )
{
    BL_FUNC_DEBUG
    
}


/// Para ser derivado, permite a las clases derivadas y a esta el tratamiento de cambio de celda.
void BlSubForm::pressedMinus ( int, int, BlDbSubFormRecord *, BlDbSubFormField * )
{
    BL_FUNC_DEBUG
    
}

/// Para ser derivado, permite a las clases derivadas y a esta el tratamiento de cambio de celda.
void BlSubForm::pressedSlash ( int, int, BlDbSubFormRecord *, BlDbSubFormField * )
{
    BL_FUNC_DEBUG
    
}

/// Para ser derivado, permite a las clases derivadas y a esta el tratamiento de cambio de celda.
QString BlSubForm::fileConfig (  )
{
    BL_FUNC_DEBUG
    
    return m_fileconfig;
}

///
/**
**/
void BlSubForm::setSelectMode()
{
    BL_FUNC_DEBUG
    m_modo = BL_SELECT_MODE;
    

}


///
/**
**/
void BlSubForm::setEditMode()
{
    BL_FUNC_DEBUG
    m_modo = BL_EDIT_MODE;
    
}


///
/**
\return
**/
bool BlSubForm::editMode()
{
    BL_FUNC_DEBUG
    
    return m_modo == BL_EDIT_MODE;
}


///
/**
\return
**/
bool BlSubForm::selectMode()
{
    BL_FUNC_DEBUG
    
    return m_modo == BL_SELECT_MODE;
}


///
/**
\return
**/
edmode BlSubForm::mode()
{
    BL_FUNC_DEBUG
    
    return m_modo;
}


/// Hace la exportacion del campo a XML
QString BlSubForm::exportXML() {
    BL_FUNC_DEBUG
    QString val;
    int error;

    val = "<BLSUBFORM>\n";
    val += "\t<QUERY>" + blXMLEncode(m_query) + "</QUERY>\n";
    val += "\t<TABLENAME>" + blXMLEncode(m_tablename) + "</TABLENAME>\n";
    val += "\t<MODO>"+ blXMLEncode(QString::number(m_modo))+"</MODO>\n";
    val += "\t<FILECONFIG>"+ blXMLEncode(m_fileconfig)+"</FILECONFIG>\n";
    val += "\t<CAMPOID>"+ blXMLEncode(m_campoid)+"</CAMPOID>\n";
    val += "\t<INSERCION>" + blXMLEncode(QString(m_insercion ? "TRUE" : "FALSE")) + "</INSERCION>\n";
    val += "\t<PRIMERO>" + blXMLEncode(QString(m_primero ? "TRUE" : "FALSE")) + "</PRIMERO>\n";
    val += "\t<DELETE>" + blXMLEncode(QString(m_delete ? "TRUE" : "FALSE")) + "</DELETE>\n";
    val += "\t<SORTING>" + blXMLEncode(QString(m_sorting ? "TRUE" : "FALSE")) + "</SORTING>\n";
    val += "\t<ORDEN>" + blXMLEncode(QString(m_orden ? "TRUE" : "FALSE")) + "</ORDEN>\n";
    val += "\t<ORDENPORQUERY>" + blXMLEncode(QString(m_ordenporquery ? "TRUE" : "FALSE")) + "</ORDENPORQUERY>\n";
    val += "\t<PROCESACAMBIOS>" + blXMLEncode(QString(m_procesacambios ? "TRUE" : "FALSE")) + "</PROCESACAMBIOS>\n";
    val += "\t<TEXTOCELDAPARAROWSPAN>" + blXMLEncode(m_textoceldaParaRowSpan) + "</TEXTOCELDAPARAROWSPAN>\n";
    val += "\t<COLUMNAPARAROWSPAN>" + blXMLEncode(m_columnaParaRowSpan) + "</COLUMNAPARAROWSPAN>\n";
    val += "\t<FILAINICIALROWSPAN>" + QString::number(m_filaInicialRowSpan) + "</FILAINICIALROWSPAN>\n";
    val += "\t<CABECERA>\n";
    BlSubFormHeader *linea;
    for ( int i = 0; i < m_lcabecera.size(); ++i ) {
        linea = m_lcabecera.at ( i );
        val += "\t\t" + linea->exportXML().replace("\t<","\t\t\t<").replace("\n<","\n\t\t<");
    } // end for
    val += "\t</CABECERA>\n";
    val += "\t<LISTA>\n";
    BlDbSubFormRecord *linea1;
    for ( int i = 0; i < m_lista.size(); ++i ) {
        linea1 = m_lista.at ( i );
        val += "\t\t" + linea1->exportXML().replace("\t<","\t\t\t<").replace("\n<","\n\t\t<");
    } // end for
    val += "\t</LISTA>\n";
    val += "\t<LISTABORRAR>\n";
    for ( int i = 0; i < m_listaborrar.size(); ++i ) {
        linea1 = m_lista.at ( i );
        val += "\t\t" + linea1->exportXML().replace("\t<","\t\t\t<").replace("\n<","\n\t\t<");
    } // end for
    val += "\t</LISTABORRAR>\n";
    val += "</BLSUBFORM>\n";
    return val;
    
}


QString BlSubForm::likeFilterSQL(const QString &text)
{
    BL_FUNC_DEBUG
    QString result = "";
    /// busca en todos los campos de tipo varchar.
    if ( !text.isEmpty() ) {
	result += " AND (";
	bool andor = true;
    
	/// Recorre todas las columnas.
	for (int i=0; i < headerList()->count(); i++) {
	  if (headerList()->at(i)->fieldType() == BlDbField::DbVarChar) {  
	    if (andor) {
	      result += " lower(" + headerList()->at(i)->fieldName() + ") LIKE lower('%" + mainCompany()->sanearCadenaUtf8(text) + "%') ";
	      andor = false;
	    } else {
	      result += " OR lower(" + headerList()->at(i)->fieldName() + ") LIKE lower('%" + mainCompany()->sanearCadenaUtf8(text) + "%') ";
	    } // end if
	  } // end if
	} // end for
	
	result += " ) ";
    } // end if
    
    return result;
}


/// ===============================================================
///  Tratamientos del Item Delegate
/// ===============================================================
///
/**
\param parent
**/
BlSubFormDelegate::BlSubFormDelegate ( QObject *parent = 0 ) : QStyledItemDelegate ( parent ), BlMainCompanyPointer()
{
    BL_FUNC_DEBUG

    m_subform = ( BlSubForm * ) parent;
    installEventFilter ( this );
    g_plugins->run ( "BlSubFormDelegate_BlSubFormDelegate_Post", ( void * ) this );
}


///
/**
**/
BlSubFormDelegate::~BlSubFormDelegate()
{
    BL_FUNC_DEBUG
}


///
/**
\param parent
\param option
\param index
**/
QWidget *BlSubFormDelegate::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    BL_FUNC_DEBUG

    BlDebug::blDebug ( "BlSubFormDelegate::createEditor", 0, "CurrentColumn: " + QString::number ( index.column() ) + "CurrentRow" + QString::number ( index.row() )  );
    BlSubFormHeader *linea;
    linea = m_subform->headerList() ->at ( index.column() );
    g_fieldName = linea->fieldName();
    g_editor = parent;

    if ( g_plugins->run ( "BlSubFormDelegate_createEditor", ( void * ) this ) )
    {
        return ( QWidget * ) g_plugParams;
    } // end if

    if (linea->fieldType() == BlDbField::DbVarChar)
    {
        BlTextEditDelegate * editor = new BlTextEditDelegate ( parent );
        editor->setObjectName ( "BlTextEditDelegate" );

        return editor;
    } else if ( linea->fieldType() == BlDbField::DbNumeric )
    {
        BlDoubleSpinBox * editor = new BlDoubleSpinBox ( parent );
        editor->setMinimum ( -100000000 );
        editor->setMaximum ( 100000000 );
        editor->setDecimals(linea->numericPrecision());

        return editor;
    } else if ( linea->fieldType() == BlDbField::DbInt )
    {
        QSpinBox * editor = new QSpinBox ( parent );
        editor->setMinimum ( -100000000 );
        editor->setMaximum ( 100000000 );

        return editor;
    } else if ( linea->fieldType() == BlDbField::DbTime )
    {
        BlTextEditDelegate * editor = new BlTextEditDelegate ( parent );
        editor->setObjectName ( "BlTextEditDelegate" );
        return editor;
    } // end if

    return QStyledItemDelegate::createEditor ( parent, option, index );
}


///
/**
\param editor
\param model
\param index
\return
**/
void BlSubFormDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubFormDelegate::setModelData", 0, "CurrentColumn: " + QString::number ( index.column() ) + "CurrentRow: " + QString::number ( index.row() ) );

    /// Si la fila o columna pasadas son inv&aacute;lidas salimos.
    if ( index.column() < 0 || index.row() < 0 )
    {
        return;
    } // end if
    g_index = index;
    g_model = model;
    g_editor = editor;
    BlSubFormHeader *linea;
    linea = m_subform->headerList() ->at ( index.column() );
    g_fieldName = linea->fieldName();

    if ( g_plugins->run ( "BlSubFormDelegate_setModelData", ( void * ) this ) )
    {
        return;
    } // end if


    if (linea->fieldType() == BlDbField::DbVarChar)
    {
        BlTextEditDelegate * textedit = qobject_cast<BlTextEditDelegate *> ( editor );
        model->setData ( index, textedit->toPlainText() );
    } else if ( linea->fieldType() == BlDbField::DbNumeric )
    {
        BlDoubleSpinBox * spinBox = static_cast<BlDoubleSpinBox*> ( editor );
        spinBox->interpretText();
        QString value = spinBox->text();
        model->setData ( index, value );
    } else if ( linea->fieldType() == BlDbField::DbInt )
    {
        QSpinBox * spinBox = static_cast<QSpinBox*> ( editor );
        spinBox->interpretText();
        QString value = QString::number(spinBox->value());
        model->setData ( index, value );
    } else if ( linea->fieldType() == BlDbField::DbTime )
    {
        BlTextEditDelegate * textedit = qobject_cast<BlTextEditDelegate *> ( editor );
        model->setData ( index, textedit->toPlainText() );
    } else
    {
        QStyledItemDelegate::setModelData ( editor, model, index );
    } // end if

}


///
/**
\param editor
\param index
**/
void BlSubFormDelegate::setEditorData ( QWidget* editor, const QModelIndex& index ) const
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlSubFormDelegate::setEditorData", 0, "CurrentColumn: " + QString::number ( index.column() ) +  "CurrentRow: " + QString::number ( index.row() )  );


    g_index = index;
    g_editor = editor;
    BlSubFormHeader *linea;
    linea = m_subform->headerList() ->at ( index.column() );
    g_fieldName = linea->fieldName();

    if ( g_plugins->run ( "BlSubFormDelegate_setEditorData", ( void * ) this ) )
    {
        return;
    } // end if

    if (linea->fieldType() == BlDbField::DbVarChar)
    {
        QString data = index.model() ->data ( index, Qt::DisplayRole ).toString();
        BlTextEditDelegate *textedit = qobject_cast<BlTextEditDelegate*> ( editor );
        textedit->setText ( data );
    } else if ( linea->fieldType() == BlDbField::DbNumeric )
    {
        QString value = index.model() ->data ( index, Qt::DisplayRole ).toString();
        BlDoubleSpinBox *spinBox = static_cast<BlDoubleSpinBox*> ( editor );
        spinBox->setValue ( value.toDouble() );
        spinBox->selectAll();
    } else if ( linea->fieldType() == BlDbField::DbInt )
    {
        QString value = index.model() ->data ( index, Qt::DisplayRole ).toString();
        QSpinBox *spinBox = static_cast<QSpinBox*> ( editor );
        spinBox->setValue ( value.toInt() );
        spinBox->selectAll();
    } else if ( linea->fieldType() == BlDbField::DbTime )
    {
        QString data = index.model() ->data ( index, Qt::DisplayRole ).toString();
        BlTextEditDelegate *textedit = qobject_cast<BlTextEditDelegate*> ( editor );
        textedit->setText ( data );
    } else
    {
        QStyledItemDelegate::setEditorData ( editor, index );
    } // end if

}


///
/**
\param obj
\param event
\return
**/
bool BlSubFormDelegate::eventFilter ( QObject *obj, QEvent *event )
{
//    BL_FUNC_DEBUG  /* No hacemos debug porque da problemas
    /// Si es un release de tecla se hace la funcionalidad especificada.
    if ( event->type() == QEvent::KeyPress )
    {
        BlDebug::blDebug ( "BlSubFormDelegate::eventFilter", 0, obj->objectName() + " --> " + QString::number ( event->type() ) );
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> ( event );
        int key = keyEvent->key();
        BlDebug::blDebug ( "BlSubFormDelegate::key = : ", 0, QString::number ( key ) );
        Qt::KeyboardModifiers mod = keyEvent->modifiers();
        /// En caso de pulsacion de un retorno de carro o similar procesamos por nuestra cuenta.
        /// Si hemos pulsado ademas el Shift se lo pasamos al editor de texto para que haga un salto de linea
        /// Si no hemos pulsado el Shift se lo pasamos al BlTableWidget para que haga un saldo de celda.
        switch ( key )
        {
          case Qt::Key_Return:
          case Qt::Key_Enter:
            if ( obj->objectName() == "BlTextEditDelegate" )
            {
              if (mod & Qt::ShiftModifier)
              {
                obj->event ( event );
              } else
              {
                QApplication::sendEvent ( m_subform->mui_list, event );
              } // end if

              return true;
            } // end if

          case Qt::Key_Backtab:
          case Qt::Key_Tab:
            return true;
        } // end switch
    } else if ( event->type() == QEvent::KeyRelease )
    {
        BlDebug::blDebug ( "BlSubFormDelegate::eventFilter", 0, obj->objectName() + " --> " + QString::number ( event->type() ) );
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> ( event );
        int key = keyEvent->key();
        BlDebug::blDebug ( "BlSubFormDelegate::key = : ", 0, QString::number ( key ) );
        Qt::KeyboardModifiers mod = keyEvent->modifiers();
        /// En caso de pulsacion de un retorno de carro o similar procesamos por nuestra cuenta.
        /// Si hemos pulsado ademas el Shift se lo pasamos al editor de texto para que haga un salto de linea
        /// Si no hemos pulsado el Shift se lo pasamos al BlTableWidget para que haga un saldo de celda.
        switch ( key )
        {
          case Qt::Key_Return:
          case Qt::Key_Enter:
            if ( obj->objectName() == "BlTextEditDelegate" )
            {
              if (mod & Qt::ShiftModifier)
              {
                /// Esta propagacion esta deshabilitata porque en caso contrario siempre se produce
                /// Un cambio de foco en la tabla y el Shift + Return no funciona como toca.
                //obj->event ( event );
              } else
              {
                ((QWidget*)obj)->clearFocus();
                QApplication::sendEvent ( m_subform->mui_list, event );
              } // end if

              return true;
            } // end if

          case Qt::Key_Backtab:
          case Qt::Key_Tab:
            ((QWidget *)obj)->clearFocus();
            QApplication::sendEvent ( m_subform->mui_list, event );
            return true;
        } // end switch
    } // end if

    bool res = QStyledItemDelegate::eventFilter ( obj, event );
    return res;
}


///
/**
\param editor
\return
**/
int BlSubFormDelegate::cerrarEditor ( QWidget *editor )
{
    BL_FUNC_DEBUG

    emit commitData(editor);
    emit closeEditor(editor, QAbstractItemDelegate::NoHint);
    return 0;
}


void BlSubFormDelegate::readyEdit(QWidget *editor)
{
    BL_FUNC_DEBUG

    cerrarEditor(editor);
}


bool BlSubFormDelegate::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab) {
            return true;
        }
    }

    return QStyledItemDelegate::event(event);
}
