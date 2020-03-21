/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borras Riera                              *
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

#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QKeyEvent>
#include <QEvent>
#include <QComboBox>
#include <QAbstractItemView>

#include "bfsubform.h"
#include "blfunctions.h"
#include "bfbuscartipoiva.h"
#include "bltexteditdelegate.h"
#include "bfbuscartrabajador.h"
#include "bfbuscaralmacen.h"
#include "bfbuscararticulo.h"
#include "blplugins.h"


/// Clase de subformularios especificos.
/**
\param parent El widget propietario del subformulario.
**/
BfSubForm::BfSubForm ( QWidget *parent ) : BlSubForm ( parent )
{
    BL_FUNC_DEBUG
    setDelete ( true );
    m_delegate = new BfSubFormDelegate ( this );
    mui_list->setItemDelegate ( m_delegate );
    mdb_idcliente = "";

    /// Disparamos los plugins.
    int res = g_plugins->run ( "BfSubForm_BfSubForm", this );
    if ( res != 0 ) {
        
        return;
    } // end if

    
}


/// Destructor de la clase.
/**
**/
BfSubForm::~BfSubForm()
{
    BL_FUNC_DEBUG
    delete m_delegate;
    
}


/// Carga un Query
/**
\param query Una consulta SQL valida.
**/
void BfSubForm::load ( QString query )
{
    BL_FUNC_DEBUG
    BlSubForm::load ( query );
    
}


///
/**
\param row
\param col
\return
**/
void BfSubForm::pressedAsterisk ( int row, int col, BlDbSubFormRecord *rec, BlDbSubFormField *camp )
{
    BL_FUNC_DEBUG

    /// Establezco las variables de clase para que los plugins puedan operar.
    m_registrolinea = rec;
    m_campoactual = camp;

    /// Disparamos los plugins.
    int res = g_plugins->run ( "BfSubForm_pressedAsterisk", this );
    if ( res != 0 ) {
        return;
    } // end if

    
}


///
/**
\param row
\param col
**/
void BfSubForm::pressedSlash ( int row, int col, BlDbSubFormRecord *rec, BlDbSubFormField *camp )
{
    BL_FUNC_DEBUG
    QString text = blTextEditor ( camp->text() );
    camp->set ( text );
    
}


///
/**
\param row
\param col
**/
void BfSubForm::pressedMinus ( int row, int col, BlDbSubFormRecord *rec, BlDbSubFormField *camp )
{
    BL_FUNC_DEBUG

    /// Como no invoca llamadas al listado de articulos lo podemos dejar aqui aunque en pluginbf_articulo estaria mucho mejor.

    if ( !rec->exists ( "idarticulo" ) ) {
        
        return;
    } // end if

    BlDbRecordSet *cur = mainCompany() ->loadQuery ( "SELECT * FROM articulo WHERE idarticulo = " + rec->dbValue ( "idarticulo" ) );
    if ( !cur ) {
        
        return;
    } // end if

    if ( !cur->eof() ) {
        rec->setDbValue ( camp->fieldName(), cur->value( "obserarticulo" ) );
    } // end if
    delete cur;

    
}


///
/**
\param row
\param col
\return
**/
void BfSubForm::editFinished ( int row, int col, BlDbSubFormRecord *rec, BlDbSubFormField *camp )
{
    BL_FUNC_DEBUG
    m_registrolinea = rec;
    m_campoactual = camp;

//    BlDbRecordSet *cur2 = NULL;
    BlDbRecordSet *cur = NULL;
//    BlDbRecordSet *cur1 = NULL;

    /// Lanzamos el manejador de la SuperClase para que se atiendan las opciones mas genericas.
    BlSubForm::editFinished ( row, col, rec, camp );

    if ( camp->fieldName() == "desctipo_iva" ) {
        cur = mainCompany() ->load ( "SELECT * FROM tipo_iva WHERE desctipo_iva = $1",  camp->text());
        if ( !cur->eof() ) {
            rec->setDbValue ( "idtipo_iva", cur->value( "idtipo_iva" ) );
        } // end if
    } // end if

    if ( camp->fieldName() == "nomtrabajador" ) {
        cur = mainCompany() ->load ( "SELECT * FROM trabajador WHERE apellidostrabajador ||', '||nomtrabajador = $1'", camp->text()  );
        if ( !cur->eof() ) {
            rec->setDbValue ( "idtrabajador", cur->value( "idtrabajador" ) );
        } // end if
    } // end if

    if ( camp->fieldName() == "nomalmacen" ) {
        cur = mainCompany() ->load ( "SELECT * FROM almacen WHERE nomalmacen ||', '||codigoalmacen = $1" , camp->text() );
        if ( !cur->eof() ) {
            rec->setDbValue ( "idalmacen", cur->value( "idalmacen" ) );
        } // end if
    } // end if

    if ( camp->fieldName() == "codigocompletoarticulo" )
    {
        cur = mainCompany() ->load( "SELECT * FROM articulo WHERE codigocompletoarticulo = $1", camp->text()  );
        if (!cur->eof())
        {
            rec->setDbValue ( "idarticulo", cur->value( "idarticulo" ) );
            rec->setDbValue ( "codigocompletoarticulo", cur->value( "codigocompletoarticulo" ) );
            rec->setDbValue ( "nomarticulo", cur->value( "nomarticulo" ) );
            if ( m_tablename == "lpresupuesto"
                    || m_tablename == "lpedidoproveedor"
                    || m_tablename == "lpedidocliente"
                    || m_tablename == "lalbaranp"
                    || m_tablename == "lfacturap"
                    || m_tablename == "lalbaran"
                    || m_tablename == "lfactura" )
            {
              if (g_confpr->value(CONF_USE_DESCARTICULO) == "TRUE" || g_confpr->value(CONF_USE_DESCARTICULO) == "T" || g_confpr->value(CONF_USE_DESCARTICULO) == "1")
              {
                rec->setDbValue ( "desc" + m_tablename, cur->value( "nomarticulo" ) +"\n" + cur->value("obserarticulo") );
              } else
              {
                rec->setDbValue ( "desc" + m_tablename, cur->value( "nomarticulo" ) );
              } // end if

              rec->setDbValue ( "cant" + m_tablename, "1.00" );
              rec->setDbValue ( "descuento" + m_tablename, "0.00" );
              calculaPVP ( rec );
            } // end if
        } else
        {
            blMsgWarning ( _ ( "El codigo del articulo no existe.\nATENCION: No se guadara bien el documento hasta que sea valido." ) );
            /// \TODO Preparo para revisarlo para la version 0.11.2 o posterior.
            /** El codigo introducido no existe en la base de datos.
            Aqui se tiene que dar la siguiente informacion:
            - "El uso de codigos de articulos inexistentes esta prohibido. Corrija
            esta situacion para poder guardar el documento."
            Se tiene que informar de las siguientes opciones:
            - Crear articulo con ese codigo.
            - Mostrar el listado de articulos para seleccionar uno valido.
            - Editar codigo para corregirlo.
            **/
            /*
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setWindowTitle(_("Articulo inexistente"));
            msgBox.setText(_("El uso de codigos de articulos inexistentes esta prohibido.\nCorrija esta situacion para poder guardar el documento."));
            QPushButton *botonCrear = msgBox.addButton(_("&Crear articulo"), QMessageBox::ActionRole);
            QPushButton *botonSeleccionar = msgBox.addButton(_("&Seleccionar articulo"), QMessageBox::ActionRole);
            QPushButton *botonEditar = msgBox.addButton(_("&Editar codigo"), QMessageBox::ActionRole);
            msgBox.setDefaultButton(botonCrear);
            msgBox.setEscapeButton(botonEditar);

            msgBox.exec();

            if (msgBox.clickedButton() == botonCrear) {
             blMsgInfo("Crear", this);

            } else if (msgBox.clickedButton() == botonSeleccionar) {
             blMsgInfo("Seleccionar", this);

            } else if (msgBox.clickedButton() == botonEditar) {


            }
            */

            delete cur;

            return;
        } // end if

        if (cur) delete cur;
    } // end if

    /// Disparamos los plugins.
    int res = g_plugins->run ( "BfSubForm_on_mui_list_editFinished", this );
    if ( res != 0 ) {
        return;
    } // end if

    /// Hago la actualizacion del campo total
    if ( (camp->fieldName() != "desc" + m_tablename) && (camp->fieldName() != "orden" + m_tablename))
    {
      if (existsHeader("cant" + m_tablename) && existsHeader("pvp" + m_tablename) && existsHeader("total" + m_tablename) )
      {
        /// El campo total es calculado, asi que tratamos su actualización aqui.
        BlFixed total = (BlFixed(rec->dbValue("cant" + m_tablename)) *
         BlFixed(rec->dbValue("pvp" + m_tablename)) *
         (BlFixed("1") - BlFixed(rec->dbValue("descuento" + m_tablename)) / BlFixed("100") ) *
         (BlFixed("1") + ((BlFixed(rec->dbValue("iva" + m_tablename)) + BlFixed(rec->dbValue("reqeq" + m_tablename))) / BlFixed("100")
        )));

        int decimals = mainCompany()->numericScale(m_tablename, QString("pvp") + m_tablename, "public");

        rec->setDbValue ( "total" + m_tablename, total.toQString('0', decimals) );

      }
    } // end if

    /// Refrescamos el registro.
    rec->refresh();
}


///
/**
\param id
\return
**/
void BfSubForm::setIdCliente ( QString id )
{
    BL_FUNC_DEBUG

    /// En la primera carga no hay reajustes, pero si actualizacion del cliente.
    if ( mdb_idcliente  == "" ) {
        mdb_idcliente = id;
        
        return;
    } // end if

    /// En las cargas sucesivas si el idcliente no ha cambiado no se hace nada
    if ( mdb_idcliente == id ) {
        
        return;
    } // end if

    /// Reseteamos los valores.
    for ( int i = 0; i < rowCount() - 1; i++ ) {
        BlDbSubFormRecord *rec = lineaat ( i );
        rec->setDbValue ( "iva" + m_tablename, "0" );
        rec->setDbValue ( "reqeq" + m_tablename, "0" );
    } // end for

    mdb_idcliente = id;

    /// Si el idcliente no existe salimos.
    if ( id == "" ) {
        
        return;
    } // end if

    BlDbRecordSet *curcliente = mainCompany() ->loadQuery ( "SELECT recargoeqcliente, regimenfiscalcliente FROM cliente WHERE idcliente = " + mdb_idcliente );

    if ( !curcliente->eof() ) {
        /// Cuando se cambia el cliente se deben recalcular las lineas por si hay Recargo Equivalente
        for ( int i = 0; i < rowCount() - 1; i++ ) {
            BlDbSubFormRecord *rec = lineaat ( i );
            BlDbRecordSet *cur = mainCompany() ->loadQuery ( "SELECT * FROM articulo WHERE idarticulo = " + rec->dbValue ( "idarticulo" ) );
            BlDbRecordSet *cur1 = mainCompany() ->loadQuery ( "SELECT * FROM tasa_iva WHERE idtipo_iva = " + cur->value( "idtipo_iva" ) + " ORDER BY fechatasa_iva DESC LIMIT 1" );
            if ( !cur->eof() ) {

                if ( curcliente->value( "regimenfiscalcliente" ) == "Normal" ) {
                    rec->setDbValue ( "iva" + m_tablename, cur1->value( "porcentasa_iva" ) );
                } // end if

                if ( curcliente->value( "recargoeqcliente" ) == "t" ) {
                    rec->setDbValue ( "reqeq" + m_tablename, cur1->value( "porcentretasa_iva" ) );
                } // end if

            } // end if
            delete cur1;
            delete cur;
        } // end for
    } // end if
    delete curcliente;
    
}


///
/**
\param id
\return
**/
void BfSubForm::setIdProveedor ( QString id )
{
    BL_FUNC_DEBUG

    /// En la primera carga no hay reajustes, pero si actualizacion del proveedor.
    if ( mdb_idproveedor  == "" ) {
        mdb_idproveedor = id;
        return;
    } // end if

    /// En las cargas sucesivas si el idproveedor no ha cambiado no se hace nada
    if ( mdb_idproveedor == id ) return;

    mdb_idproveedor = id;

    /// Reseteamos los valores
    for ( int i = 0; i < rowCount() - 1; i++ ) {
        BlDbSubFormRecord *rec = lineaat ( i );
        rec->setDbValue ( "iva" + m_tablename, "0" );
        rec->setDbValue ( "reqeq" + m_tablename, "0" );
    } // end for

    /// Si el idproveedor no existe salimos.
    if ( id == "" ) {
        
        return;
    } // end if

    BlDbRecordSet *curproveedor = mainCompany() ->loadQuery ( "SELECT recargoeqproveedor, regimenfiscalproveedor FROM proveedor WHERE idproveedor=" + mdb_idproveedor );

    if ( !curproveedor->eof() ) {
        /// Cuando se cambia el proveedor se deben recalcular las lineas por si hay Recargo Equivalente
        for ( int i = 0; i < rowCount() - 1; i++ ) {
            BlDbSubFormRecord *rec = lineaat ( i );
            BlDbRecordSet *cur = mainCompany() ->loadQuery ( "SELECT * FROM articulo WHERE idarticulo = " + rec->dbValue ( "idarticulo" ) );
            BlDbRecordSet *cur1 = mainCompany() ->loadQuery ( "SELECT * FROM tasa_iva WHERE idtipo_iva = " + cur->value( "idtipo_iva" ) + " ORDER BY fechatasa_iva DESC LIMIT 1" );
            if ( !cur->eof() ) {
                if ( curproveedor->value( "regimenfiscalproveedor" ) == "Normal" ) {
                    rec->setDbValue ( "iva" + m_tablename, cur1->value( "porcentasa_iva" ) );
                } // end if
                if ( curproveedor->value( "recargoeqproveedor" ) == "t" ) {
                    rec->setDbValue ( "reqeq" + m_tablename, cur1->value( "porcentretasa_iva" ) );
                } // end if

            } // end if
            delete cur1;
            delete cur;
        } // end for
    } // end if
    delete curproveedor;
    
}


///
/**
\param id
**/
void BfSubForm::setIdAlmacen ( QString id )
{
    BL_FUNC_DEBUG
    m_idAlmacen = id;
    
}


///
/**
\param c
**/
void BfSubForm::setMainCompany ( BlMainCompany *c )
{
    BL_FUNC_DEBUG
    BlSubForm::setMainCompany ( c );
    m_delegate->setMainCompany ( c );
    
}


///
/**
\return
**/
int BfSubForm::cerrarEditor()
{
    BL_FUNC_DEBUG

    QWidget *editor = mui_list->QAbstractItemView::indexWidget ( mui_list->currentIndex() );
    m_delegate->readyEdit ( editor );
    
    return 0;
}


/// Devuelve el identificador de cliente, si es un subformulario de cliente.
/**
\return El identificador de cliente
**/
QString BfSubForm::idcliente()
{
    return mdb_idcliente;
}


/// Devuelve el identificador de proveedor, si es un subformulario de proveedor.
/**
\return El identificador de proveedor
**/
QString BfSubForm::idproveedor()
{
    return mdb_idproveedor;
}


///
/**
**/
QString BfSubForm::idAlmacen ()
{
    BL_FUNC_DEBUG
    
    return m_idAlmacen;
}


/// ===============================================================
///  Tratamientos del Item Delegate
/// ===============================================================
///
/**
\param parent
**/
BfSubFormDelegate::BfSubFormDelegate ( QObject *parent = 0 ) : BlSubFormDelegate ( parent )
{
    BL_FUNC_DEBUG
//    m_subform = ( BlSubForm * ) parent;
//    installEventFilter ( this );
    
}


///
/**
**/
BfSubFormDelegate::~BfSubFormDelegate()
{
    BL_FUNC_DEBUG
    
}


///
/**
\param parent
\param option
\param index
**/
QWidget *BfSubFormDelegate::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    BL_FUNC_DEBUG
    BlSubFormHeader *linea;
    linea = m_subform->headerList() ->at ( index.column() );

    if ( linea->fieldName() == "desc" + m_subform->tableName() ) {
        BlTextEditDelegate * editor = new BlTextEditDelegate ( parent );
        editor->setObjectName ( "BlTextEditDelegate" );
        
        return editor;
    } else if ( linea->fieldName() == "cant" + m_subform->tableName()
                || linea->fieldName() == "pvp" + m_subform->tableName()
                || linea->fieldName() == "descuento" + m_subform->tableName()
                || linea->fieldName() == "iva" + m_subform->tableName()
                || linea->fieldName() == "reqeq" + m_subform->tableName() ) {

        BlDoubleSpinBox * editor = new BlDoubleSpinBox ( parent );
        editor->setMinimum ( -10000000 );
        editor->setMaximum ( 10000000 );
        editor->setDecimals(linea->numericPrecision());
        
        return editor;
    } else if ( linea->fieldName() == "desctipo_iva" ) {
        BfBuscarTipoIVADelegate * editor = new BfBuscarTipoIVADelegate ( parent );
        editor->setMainCompany ( ( BfCompany * ) m_subform->mainCompany() );
        
        return editor;
    } else if ( linea->fieldName() == "nomtrabajador" && m_subform->tableName() != "trabajador" ) {
        BfBuscarTrabajadorDelegate * editor = new BfBuscarTrabajadorDelegate ( parent );
        editor->setMainCompany ( ( BfCompany * ) m_subform->mainCompany() );
        
        return editor;
    } else if ( linea->fieldName() == "nomalmacen"  && m_subform->tableName() != "almacen" ) {
        BfBuscarAlmacenDelegate * editor = new BfBuscarAlmacenDelegate ( parent );
        editor->setMainCompany ( ( BfCompany * ) m_subform->mainCompany() );
        
        return editor;
    } else  {

        return BlSubFormDelegate::createEditor ( parent, option, index );
    } // end if
}


///
/**
\param editor
\param model
\param index
\return
**/
void BfSubFormDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    BL_FUNC_DEBUG

    /// Si la fila o columna pasadas son inv&aacute;lidas salimos.
    if ( index.column() < 0 || index.row() < 0 ) {
        
        return;
    } // end if

    BlSubFormHeader *linea;
    linea = m_subform->headerList() ->at ( index.column() );
    if ( linea->fieldName() == "desc" + m_subform->tableName() ) {
        BlTextEditDelegate * textedit = qobject_cast<BlTextEditDelegate *> ( editor );
        model->setData ( index, textedit->toPlainText() );
    } else if ( linea->fieldName() == "cant" + m_subform->tableName()
                || linea->fieldName() == "pvp" + m_subform->tableName()
                || linea->fieldName() == "descuento" + m_subform->tableName()
                || linea->fieldName() == "reqeq" + m_subform->tableName()
                || linea->fieldName() == "iva" + m_subform->tableName() ) {

        BlDoubleSpinBox * spinBox = static_cast<BlDoubleSpinBox*> ( editor );
        spinBox->interpretText();
        QString value = spinBox->text();
    
        QString valueanterior = m_subform->lista()->at ( index.row() )->dbValue ( linea->fieldName() );

        model->setData ( index, value );

        
        /// Dispara senyales:
        if ( linea->fieldName() == "cant" + m_subform->tableName() /*
                || linea->fieldName() == "pvp" + m_subform->tableName()
                || linea->fieldName() == "descuento" + m_subform->tableName()
                || linea->fieldName() == "reqeq" + m_subform->tableName()
                || linea->fieldName() == "iva" + m_subform->tableName() */) {
            /// Mira si se ha cambiado la cantidad o es la misma que habia.
            if ( valueanterior.toDouble() != value.toDouble() ) {
                emit cant_changed ( m_subform->lista()->at ( index.row() ) );
            } // end if
        } // end if
    } else if ( linea->fieldName() == "desctipo_iva" ) {
        BfBuscarTipoIVADelegate * comboBox = static_cast<BfBuscarTipoIVADelegate*> ( editor );
        QString value = comboBox->currentText();
        model->setData ( index, value );
    } else if ( linea->fieldName() == "nomtrabajador"  && m_subform->tableName() != "trabajador" ) {
        BfBuscarTrabajadorDelegate * comboBox = static_cast<BfBuscarTrabajadorDelegate*> ( editor );
        QString value = comboBox->currentText();
        model->setData ( index, value );
    } else if ( linea->fieldName() == "nomalmacen" && m_subform->tableName() != "almacen" ) {
        BfBuscarAlmacenDelegate * comboBox = static_cast<BfBuscarAlmacenDelegate*> ( editor );
        QString value = comboBox->currentText();
        model->setData ( index, value );
    } else {
        BlSubFormDelegate::setModelData ( editor, model, index );
    } // end if
}


///
/**
\param editor
\param index
**/
void BfSubFormDelegate::setEditorData ( QWidget* editor, const QModelIndex& index ) const
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( Q_FUNC_INFO, 0, QString(_("Columna: '%1', fila: '%2'.")).arg(QString::number ( index.column() )).arg(QString::number ( index.row() )) );
    BlSubFormHeader *linea;
    linea = m_subform->headerList() ->at ( index.column() );
    if ( linea->fieldName() == "desc" + m_subform->tableName() ) {
        QString data = index.model() ->data ( index, Qt::DisplayRole ).toString();
        BlTextEditDelegate *textedit = qobject_cast<BlTextEditDelegate*> ( editor );
        textedit->setText ( data );
    } else if ( linea->fieldName() == "cant" + m_subform->tableName()
                || linea->fieldName() == "pvp" + m_subform->tableName()
                || linea->fieldName() == "descuento" + m_subform->tableName()
                || linea->fieldName() == "reqeq" + m_subform->tableName()
                || linea->fieldName() == "iva" + m_subform->tableName() ) {

        QString value = index.model() ->data ( index, Qt::DisplayRole ).toString();

        /// TODO
        value = value.replace(",", ".");

        BlDoubleSpinBox *spinBox = static_cast<BlDoubleSpinBox*> ( editor );
        spinBox->setValue ( value.toDouble() );
        spinBox->selectAll();
    } else if ( linea->fieldName() == "desctipo_iva" ) {
        QString value = index.model() ->data ( index, Qt::DisplayRole ).toString();
        BfBuscarTipoIVADelegate *comboBox = static_cast<BfBuscarTipoIVADelegate*> ( editor );
        comboBox->set ( value );
    } else if ( linea->fieldName() == "nomtrabajador"  && m_subform->tableName() != "trabajador" ) {
        QString value = index.model() ->data ( index, Qt::DisplayRole ).toString();
        BfBuscarTrabajadorDelegate *comboBox = static_cast<BfBuscarTrabajadorDelegate*> ( editor );
        comboBox->set ( value );
    } else if ( linea->fieldName() == "nomalmacen" && m_subform->tableName() != "almacen" ) {
        QString value = index.model() ->data ( index, Qt::DisplayRole ).toString();
        BfBuscarAlmacenDelegate *comboBox = static_cast<BfBuscarAlmacenDelegate*> ( editor );
        comboBox->set ( value );
    } else {
        BlSubFormDelegate::setEditorData ( editor, index );
    } // end if
    
}



void BfSubFormDelegate::updateEditorGeometry ( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    editor->setGeometry(option.rect);
}


///
/**
**/
QString BfSubForm::idArticulo()
{
    return m_idArticulo;
}


///
/**
**/
QString BfSubForm::idTarifa()
{
    return m_idTarifa;
}


///
/**
**/
void BfSubForm::calculaPVP ( BlDbSubFormRecord *rec )
{
    BL_FUNC_DEBUG

    BlDbRecordSet *cur = NULL;
    BlDbRecordSet *cur3 = NULL;
    BlDbRecordSet *cur1 = NULL;
    BlDbRecordSet *cur2 = NULL;

    /// Saca 'codigocompletoarticulo' del BlDbSubFormRecord pasado como parametro.
    QString codigocompleto = rec->dbValue ( "codigocompletoarticulo" );

    cur = mainCompany() ->load ( "SELECT * FROM articulo WHERE codigocompletoarticulo = $1", codigocompleto  );
    if ( !cur->eof() ) {


// ==============================
        // Miramos el IVA del articulo y lo ponemos.
        QString query = "SELECT * FROM tasa_iva WHERE idtipo_iva = " + cur->value( "idtipo_iva" ) + " ORDER BY fechatasa_iva DESC LIMIT 1";

        cur1 = mainCompany() ->loadQuery ( query );
        if ( !cur1->eof() )
        {

            if ( m_tablename == "lpresupuesto"
                    || m_tablename == "lpedidocliente"
                    || m_tablename == "lpedidoproveedor"
                    || m_tablename == "lalbaranp"
                    || m_tablename == "lfacturap"
                    || m_tablename == "lalbaran"
                    || m_tablename == "lfactura" ) {
                rec->setDbValue ( "iva" + m_tablename, cur1->value( "porcentasa_iva" ) );

                /// Calculamos el recargo equivalente.
                if ( mdb_idcliente != "" ) {
                    cur2 = mainCompany() ->loadQuery ( "SELECT recargoeqcliente FROM cliente WHERE idcliente = " + mdb_idcliente );
                    if ( !cur2->eof() ) {
                        if ( cur2->value( "recargoeqcliente" ) == "t" ) {
                            rec->setDbValue ( "reqeq" + m_tablename, cur1->value( "porcentretasa_iva" ) );
                        } // end if
                    } // end if
                    delete cur2;
                } else if ( mdb_idproveedor != "" ) {
                    cur2 = mainCompany() ->loadQuery ( "SELECT recargoeqproveedor FROM proveedor WHERE idproveedor = " + mdb_idproveedor );
                    if ( !cur2->eof() ) {
                        if ( cur2->value( "recargoeqproveedor" ) == "t" ) {
                            rec->setDbValue ( "reqeq" + m_tablename, cur1->value( "porcentretasa_iva" ) );
                        } // end if
                    } // end if
                    delete cur2;
                } else {
                    rec->setDbValue ( "reqeq" + m_tablename, "0" );
                } // end if

            } // end if
        } // end if
        if ( cur1 != NULL ) delete cur1;
// ==============================



        /// Aqui se establece el precio del articulo. Se tiene que tener en cuenta
        /// el cliente y la tarifa asignada si procede.
        if ( !mdb_idcliente.isEmpty() && !m_idAlmacen.isEmpty() ) {
            /// Se ha seleccionado un cliente.
            m_idArticulo = cur->value( "idarticulo" );
            cur3 = mainCompany() ->loadQuery ( "SELECT cliente.idtarifa, ltarifa.pvpltarifa, ltarifa.idalmacen FROM cliente INNER JOIN ltarifa ON (cliente.idtarifa = ltarifa.idtarifa) WHERE ltarifa.idalmacen = " + m_idAlmacen + " AND cliente.idcliente = " + mdb_idcliente + " AND ltarifa.idarticulo = " + m_idArticulo );
            m_idTarifa = cur3->value( "idtarifa" );
            if ( cur3->numregistros() > 0 ) {
                /// A) Se dispone de tarifa especial.
                rec->setDbValue ( "pvp" + m_tablename, cur3->value( "pvpltarifa" ) );

                /// Disparamos los plugins.
                int res = g_plugins->run ( "BfSubForm_calculaPVP", this );
                if ( res != 0 ) {
                    return;
                } // end if

            } else {
                /// B) No tiene tarifa especial se usa la asignada por defecto.
                rec->setDbValue ( "pvp" + m_tablename, cur->value( "pvparticulo" ) );

            } // end if
        } else {
            if (m_tablename != "ltarifa") {
                /// Sin cliente asignado se usa la tarifa asignada por defecto.
                rec->setDbValue ( "pvp" + m_tablename, cur->value( "pvparticulo" ) );

            } // end if
        } // end if

    } // end if

    delete cur;
    delete cur3;
    
}


