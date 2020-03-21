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

#include <QMenu>

#include "blformlist.h"
#include "blmaincompany.h"

#include "bldatesearch.h"
#include "blcombobox.h"
#include "blsearchwidget.h"
#include "bldatesearch.h"

#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDomNode>
#include <QCheckBox>


/**  procedimiento de QtScript
Importa el objeto que se pasa como referencia al engine de script
Busca el archivo en el directorio de openreports que tenga el mismo nombre que la clase
Importa el script y lo lanza.

*/
void BlFormList::blScript(QObject * obj) {
    
    /// Lanzamos los scripts de QScript
    QString fileName = g_confpr->value( CONF_DIR_OPENREPORTS ) + "blformlist_"+metaObject()->className()+".qs";
    QFile scriptFile1(fileName);
    if (scriptFile1.open(QIODevice::ReadOnly)) {
	  QTextStream stream(&scriptFile1);
	  QString contents = stream.readAll();
	  QScriptValue objectValue = m_myEngine.newQObject(this);
	  m_myEngine.globalObject().setProperty(metaObject()->className(), objectValue);
	  QScriptValue objectValue1 = m_myEngine.newQObject(mainCompany());
	  m_myEngine.globalObject().setProperty("MainCompany", objectValue1);
	  m_myEngine.importExtension("qt.core");
	  m_myEngine.importExtension("qt.gui"); 
	  m_myEngine.evaluate(contents);
	  scriptFile1.close();
	  if (m_myEngine.hasUncaughtException()) {
	    blMsgInfo(m_myEngine.uncaughtException().toString());
	  } // end if
    } // end if
    
    
    fileName = g_confpr->value( CONF_DIR_OPENREPORTS ) + "blformlist"+".qs";
    scriptFile1.setFileName(fileName);
    if (scriptFile1.open(QIODevice::ReadOnly)) {
	  QTextStream stream(&scriptFile1);
	  QString contents = stream.readAll();
	  QScriptValue objectValue = m_myEngine.newQObject(this);
	  m_myEngine.globalObject().setProperty("BlForm", objectValue);
	  QScriptValue objectValue1 = m_myEngine.newQObject(mainCompany());
	  m_myEngine.globalObject().setProperty("MainCompany", objectValue1);
	  m_myEngine.importExtension("qt.core");
	  m_myEngine.importExtension("qt.gui"); 
	  m_myEngine.evaluate(contents);
	  scriptFile1.close();
	  if (m_myEngine.hasUncaughtException()) {
	    blMsgInfo(m_myEngine.uncaughtException().toString());
	  } // end if
    } // end if
}


///
/**
**/
void BlFormList::setSelectMode()
{
    BL_FUNC_DEBUG
    m_modo = BL_SELECT_MODE;
    

}


///
/**
**/
void BlFormList::setEditMode()
{
    BL_FUNC_DEBUG
    m_modo = BL_EDIT_MODE;
    
}


///
/**
\return
**/
bool BlFormList::editMode()
{
    BL_FUNC_DEBUG
    
    return m_modo == BL_EDIT_MODE;
}


///
/**
\return
**/
bool BlFormList::selectMode()
{
    BL_FUNC_DEBUG
    
    return m_modo == BL_SELECT_MODE;
}


///
/**
\param parent
\param f
\param modo
**/
BlFormList::BlFormList ( QWidget *parent, Qt::WindowFlags f, edmode modo ) : BlWidget ( parent, f )
{
    BL_FUNC_DEBUG
    m_modo = modo;
    m_listado = NULL;
    
}


///
/**
\param emp
\param parent
\param f
\param modo
**/
BlFormList::BlFormList ( BlMainCompany *emp, QWidget *parent, Qt::WindowFlags f, edmode modo ) : BlWidget ( emp, parent, f )
{
    BL_FUNC_DEBUG
    m_modo = modo;
    m_listado = NULL;
    
}


///
/**
**/
BlFormList::~BlFormList()
{
    BL_FUNC_DEBUG
     
    guardaFiltrosXML();
    removeWindow();
}


///
/**
\return
**/
int BlFormList::removeWindow()
{
    BL_FUNC_DEBUG
    mainCompany() ->removeWindow ( this );
    
    return 0;
}


///
/**
\param title
\return
**/
int BlFormList::insertWindow ( QString title )
{
    BL_FUNC_DEBUG
    mainCompany() ->insertWindow ( title, this );
    
    return 0;
}


/** SLOT que responde al doble click sobre un elemento del listado
    llama a editArticle para que actue correspondientemente.
*/
/**
\param a
**/
void BlFormList::on_mui_list_cellDoubleClicked ( int a, int )
{
    BL_FUNC_DEBUG
    editar ( a );
    
}


///
/**
**/
void BlFormList::editar ( int )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "metodo para ser reimplementado en clases hijas", 2 );
}


///
/**
**/
void BlFormList::crear()
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "metodo para ser reimplementado en clases hijas", 2 );
}


///
/**
**/
void BlFormList::remove()
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "metodo para ser reimplementado en clases hijas", 2 );
}


///
/**
**/
void BlFormList::imprimir()
{
    BL_FUNC_DEBUG
    m_listado->printPDF ( "" );
    
}

/// Este metodo carga el subformulario con un SELECT generico. Si no quiere usarse este select
/// Debe reimplementar el metodo en las clases derivadas.
/**
**/
void BlFormList::presentar()
{
    BL_FUNC_DEBUG
    QString query = "SELECT * FROM " + m_listado->tableName() + " WHERE 1=1 " + generaFiltro();
    m_listado->load ( query );
    
}

/** Metodo auxiliar que genera la clausula WHERE del listado con las opciones de filtrado segun unas normas especificas
 * Todas las busquedas tienen la clausula AND
 * Los campos de tipo texto (excepto m_filtro) hace una busqueda LIKE sobre el campo seleccionado
 * Los campos de tipo fecha (si contienen in) hacen una busqueda >= si contienen fin hacen una busqueda <=
 * El resto de campos hacen una busqueda por igualdad.
* .
*/
const QString BlFormList::generaFiltro()
{
    BL_FUNC_DEBUG
    QString filtro = "";

    QLineEdit * lfiltro = findChild<QLineEdit *>("m_filtro");

    /// Protecci&oacute;n para el caso de no existir m_filtro.
    if (lfiltro != 0) {

        if (lfiltro->text() != "") {
	    /// Hacemos el filtrado like del campo m_filtro
	    filtro += m_listado->likeFilterSQL(lfiltro->text());
	} // end if
	
    } // end if

    QList<BlComboBox *> l4 = findChildren<BlComboBox *>();
    QListIterator<BlComboBox *> it4 ( l4 );
    while ( it4.hasNext() ) {
	BlComboBox * item = it4.next();
	if ( item->objectName().startsWith ( "mui_" ) && item->id() != "") {
	    filtro += " AND " + item->objectName().right(item->objectName().size()-4) + " = " + item->id();
	} // end if
    } // end while

    QList<QCheckBox *> l6 = findChildren<QCheckBox *>();
    QListIterator<QCheckBox *> it6 ( l6 );
    while ( it6.hasNext() ) {
	QCheckBox * item = it6.next();
	if ( item->objectName().startsWith ( "mui_" ) && item->isChecked()) {
	    filtro += " AND " + item->objectName().right(item->objectName().size()-4) + " = TRUE";
	} // end if
    } // end while

      QList<BlDateSearch *> l7 = findChildren<BlDateSearch *>();
      QListIterator<BlDateSearch *> it7 ( l7 );
      while ( it7.hasNext() ) {
	    BlDateSearch * item = it7.next();
	    if ( item->objectName().startsWith ( "mui_" ) && item->text() != "" ) {
		if (item->objectName().contains("fin")) {
		    filtro += " AND " + item->objectName().right(item->objectName().size()-4) + " <= '" + item->text() + "'";
		} else {
		    filtro += " AND " + item->objectName().right(item->objectName().size()-4) + " >= '" + item->text() + "'";
		} // end if
	    } // end if
      } // end while

    QList<BlSearchWidget *> l9 = findChildren<BlSearchWidget *>();
    QListIterator<BlSearchWidget *> it9 ( l9 );
    while ( it9.hasNext() ) {
	BlSearchWidget * item = it9.next();
	if ( item->objectName().startsWith ( "mui_" ) && item->id() != "") {
	    filtro += " AND " + item->objectName().right(item->objectName().size()-4) + " = " + item->id();
	} // end if
    } // end while

    return ( filtro );
}


///
/**
\param list
**/
void BlFormList::setSubForm ( BlSubForm *list )
{
    BL_FUNC_DEBUG
    m_listado = list;
    /// Establecemos el mismo modo en el subformulario que en el listado.
    if ( m_modo == BL_EDIT_MODE ) {
        m_listado->setEditMode();
    } else {
        m_listado->setSelectMode();
    } // end if
    
}



///
/**
\param list
**/
BlSubForm *BlFormList::subForm ( )
{
    BL_FUNC_DEBUG
    
    return m_listado;
}


///
/**
\param text
**/
void BlFormList::on_m_filtro_textChanged ( const QString &text )
{
    BL_FUNC_DEBUG
    if ( text.size() >= g_confpr->value(CONF_NUMCHAR_RELOAD_FILTRO).toInt() )  {
        on_mui_actualizar_clicked();
    } // end if
}


///
/**
**/
void BlFormList::on_m_filtro_editingFinished()
{
    BL_FUNC_DEBUG
    static QString valant = "";
    QLineEdit *line = findChild<QLineEdit *> ( "m_filtro" );
    if ( line->text() != valant ) {
        valant = line->text();
        on_mui_actualizar_clicked();
    } // end if
    
}


/** SLOT que responde a la pulsacion de la tecla Enter
    en la caja de texto del buscador del formulario
    Comprueba que solo haya un item en el listado. Si
    es asi, abre ese item.
*/
void BlFormList::on_m_filtro_returnPressed()
{
    BL_FUNC_DEBUG
    /// Actualiza.
    presentar();
    /// Selecciona si solo existe un registro.
    if ( m_listado->lista()->count() == 1 ) {
        editar ( 0 );
    } // end if
    
}


///
/**
**/
void BlFormList::on_mui_actualizar_clicked()
{
    BL_FUNC_DEBUG
    presentar();
}


///
/**
**/
void BlFormList::on_mui_imprimir_clicked()
{
    BL_FUNC_DEBUG
    imprimir();
    
}


///
/**
**/
void BlFormList::on_mui_crear_clicked()
{
    BL_FUNC_DEBUG
    crear();
    
}


///
/**
**/
void BlFormList::on_mui_borrar_clicked()
{
    BL_FUNC_DEBUG
    remove();
    
}


/** SLOT que responde al boton de editar articulo
    Comprueba que haya un elemento seleccionado y llama a editArticle.
*/
/**
\return
**/
void BlFormList::on_mui_editar_clicked()
{
    BL_FUNC_DEBUG
    int a = m_listado->currentRow();
    if ( a < 0 ) {
        blMsgInfo ( _ ( "Tiene que seleccionar un elemento" ) );
        return;
    } // end if
    editar ( a );
    
}


///
/**
\param checked
**/
void BlFormList::on_mui_configurar_toggled ( bool checked )
{
    BL_FUNC_DEBUG
    if ( checked ) {
        m_listado->showConfig();
    } else {
        m_listado->hideConfig();
    } // end if
    
}


/** \TODO: REVISAR ESTE METODO YA QUE NO PARECE SER EL ADECUADO
    EN LA LLAMADA DE SUBMENUS
*/
/**
\param p
**/
void BlFormList::on_mui_list_customContextMenuRequested ( const QPoint &p )
{
    BL_FUNC_DEBUG
    submenu ( p );
    
}

void BlFormList::contextMenuEvent ( QContextMenuEvent * ) {
  BL_FUNC_DEBUG
  QMenu *popup = new QMenu ( this );

/// Si estamos en modo experto. Lo primero que hacemos es encabezar el menu con el nombre del objeto para tenerlo bien ubicado.
if (g_confpr->value(CONF_MODO_EXPERTO) == "TRUE") {
  QAction *nombreobjeto = popup->addAction( objectName() );
  nombreobjeto->setDisabled(true);
} // end if

    QAction *opcion = popup->exec ( QCursor::pos() );
    
    
}


/** \TODO: REVISAR ESTE METODO YA QUE NO PARECE SER EL ADECUADO
    EN LA LLAMADA DE SUBMENUS
*/
/**
\return
**/
void BlFormList::submenu ( const QPoint & )
{
    BL_FUNC_DEBUG
    int a = m_listado->currentRow();
    if ( a < 0 )
        return;
    QMenu *popup = new QMenu ( this );
    
    /// Si estamos en modo experto. Lo primero que hacemos es encabezar el menu con el nombre del objeto para tenerlo bien ubicado.
    if (g_confpr->value(CONF_MODO_EXPERTO) == "TRUE") {
      QAction *nombreobjeto = popup->addAction( objectName() );
      nombreobjeto->setDisabled(true);
    } // end if
    
    
    QAction *edit = popup->addAction ( _ ( "Editar" ) );
    QAction *del = popup->addAction ( _ ( "Borrar" ) );
    QAction *opcion = popup->exec ( QCursor::pos() );
    if ( opcion == del )
        on_mui_borrar_clicked();
    if ( opcion == edit )
        on_mui_editar_clicked();
    delete popup;
}


///
/**
\param check
**/
void BlFormList::on_mui_list_toogledConfig ( bool check )
{
    BL_FUNC_DEBUG

    QToolButton *botonconfigurar = findChild<QToolButton *> ( "mui_configurar" );
    if ( botonconfigurar )
        botonconfigurar->setChecked ( check );
    
}


///
/**
**/
void BlFormList::hideBotonera()
{
    BL_FUNC_DEBUG
    QWidget *botonera = findChild<QWidget *> ( "m_botonera" );
    if ( botonera )
        botonera->hide();
    
}


///
/**
**/
void BlFormList::showBotonera()
{
    BL_FUNC_DEBUG
    QWidget *botonera = findChild<QWidget *> ( "m_botonera" );
    if ( botonera )
        botonera->show();
    
}


///
/**
**/
void BlFormList::hideBusqueda()
{
    BL_FUNC_DEBUG
    QWidget *busqueda = findChild<QWidget *> ( "m_busqueda" );
    if ( busqueda )
        busqueda->hide();
    
}


///
/**
**/
void BlFormList::showBusqueda()
{
    BL_FUNC_DEBUG
    QWidget *busqueda = findChild<QWidget *> ( "m_busqueda" );
    if ( busqueda )
        busqueda->show();
    
}

/// Configure the standard buttons depending on the privileges on the table.
/**
\param nomtabla
**/
void BlFormList::trataPermisos ( QString nomtabla )
{
    BL_FUNC_DEBUG


    if ( mainCompany() != NULL ) {
       QAbstractButton * pbut = NULL; /// Puntero para buscar y manipular botones
       /// Buscamos los permisos que tiene el usuario y desactivamos botones.
       if ( !mainCompany() ->hasTablePrivilege ( nomtabla, "INSERT" ) ) {

          pbut = findChild<QAbstractButton *> ( "mui_crear" );
          if ( pbut ) pbut->setDisabled ( true );
          pbut = findChild<QAbstractButton *> ( "mui_importar" );
          if ( pbut ) pbut->setDisabled ( true );
          pbut = findChild<QAbstractButton *> ( "mui_exportar" );
          if ( pbut ) pbut->setDisabled ( true );
       } // end if

       if ( !mainCompany() ->hasTablePrivilege ( nomtabla, "DELETE" ) ) {
          pbut = findChild<QAbstractButton *> ( "mui_borrar" );
          if ( pbut ) pbut->setDisabled ( true );
       } // end if
    } // end if
}


/// Guarda todos los parametros de filtraje que pudiera tener el listado.
/**
 **/
void BlFormList::guardaFiltrosXML() {
      QString aux = "";
    QFile file ( nameFileConfig() );
    /// Guardado del orden y de configuraciones varias.
    if ( file.open ( QIODevice::WriteOnly ) ) {
        QTextStream stream ( &file );
	stream << "<DOCUMENT>" << "\n";
	QList<BlDateSearch *> l7 = findChildren<BlDateSearch *>();
	QListIterator<BlDateSearch *> it7 ( l7 );
	while ( it7.hasNext() ) {
	    BlDateSearch * item = it7.next();
	    QString objname = item->objectName();
	    QString date = item->text();
	    QString objparent = item->parent()->objectName();
	    stream << "<BLDATESEARCH>\n\t<OBJNAME>" << objname << "</OBJNAME>\n\t<OBJVALUE>" << date << "</OBJVALUE>\n\t<OBJPARENT>" << objparent << "</OBJPARENT>\n</BLDATESEARCH>";
	} // end while
	
	QList<QLineEdit *> l8 = findChildren<QLineEdit *>();
	QListIterator<QLineEdit *> it8 ( l8 );
	while ( it8.hasNext() ) {
	    QLineEdit * item = it8.next();
	    QString objname = item->objectName();
	    QString objparent = item->parent()->objectName();
	    QString date = item->text();
	    stream << "<QLINEEDIT>\n\t<OBJNAME>" << objname << "</OBJNAME>\n\t<OBJVALUE>" << date << "</OBJVALUE>\n\t<OBJPARENT>" << objparent << "</OBJPARENT>\n</QLINEEDIT>";
	} // end while
	
	stream << "</DOCUMENT>\n";
	file.close();
    } // end if
}


/// Guarda todos los parametros de filtraje que pudiera tener el listado.
/**
 **/
void BlFormList::cargaFiltrosXML() {
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

    /// Leemos la visibilidad de las columnas. Se hace antes de ordenarlas.
    QDomNodeList nodos = docElem.elementsByTagName ( "BLDATESEARCH" );
    for ( int i = 0; i < nodos.count(); i++ ) {
        QDomNode visible = nodos.item ( i );
        QDomElement e1 = visible.toElement(); /// try to convert the node to an element.
        if ( !e1.isNull() ) { /// the node was really an element.
	    /// Cogemos el nombre y el valor
	    QString objname = e1.firstChildElement ( "OBJNAME" ).toElement().text();
	    QString objvalue = e1.firstChildElement ( "OBJVALUE" ).toElement().text();
	    QString objparent = e1.firstChildElement ( "OBJPARENT" ).toElement().text();
	    BlDateSearch *dates = findChild<BlDateSearch *>(objname);
	    if (dates) {
		if (dates->parent()->objectName() == objparent) {
		    dates->setText(objvalue);
		} // end if
	    } // end if
	} // end if
    } // end form
  
    /// Leemos la visibilidad de las columnas. Se hace antes de ordenarlas.
    nodos = docElem.elementsByTagName ( "QLINEEDIT" );
    for ( int i = 0; i < nodos.count(); i++ ) {
        QDomNode visible = nodos.item ( i );
        QDomElement e1 = visible.toElement(); /// try to convert the node to an element.
        if ( !e1.isNull() ) { /// the node was really an element.

	    /// Cogemos el nombre y el valor
	    QString objname = e1.firstChildElement ( "OBJNAME" ).toElement().text();
	    QString objvalue = e1.firstChildElement ( "OBJVALUE" ).toElement().text();
	    QString objparent = e1.firstChildElement ( "OBJPARENT" ).toElement().text();
	    QLineEdit *dates = findChild<QLineEdit *>(objname);
	    if (dates) {
		if (dates->parent()->objectName() == objparent) {
		    dates->setText(objvalue);
		} // end if
	    } // end if
	    
	} // end if
    } // end form
  
}


/// Sacamos cual es el archivo en el que guardar/cargar configuraciones
const QString BlFormList::nameFileConfig() 
{
   QString directorio = g_confpr->value(CONF_DIR_USER);
   if (g_confpr->value(CONF_GLOBAL_CONFIG_USER) == "TRUE") {
      directorio = g_confpr->value(CONF_DIR_CONFIG);
   } // end if

   QString empresa = mainCompany()->dbName();
   if (g_confpr->value(CONF_GLOBAL_CONFIG_COMPANY) == "TRUE") {
      empresa  = "";
   } // end if

  QString nombre = directorio  + "blformlist_" + empresa + "_" + objectName() +"_cfn.cfn" ;
  return nombre;
}

/// Sacamos cual es el archivo de configuraciones por defecto en caso de que no exista un archivo de configuraciones.
/// Lo usamos para mejorar la presentacion en la primera ejecucion ya que la primera impresion es la que queda.
const QString BlFormList::nameFileDefaultConfig() 
{
   QString directorio = g_confpr->value(CONF_DIR_DEFAULT_CONFS);

  QString nombre = directorio + "blformlist_" + objectName() + "_cfn.cfn" ;
  return nombre;
}
