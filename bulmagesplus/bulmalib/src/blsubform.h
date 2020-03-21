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

#ifndef BLSUBFORM_H
#define BLSUBFORM_H

#include <Qt>
#include <QWidget>
#include <QHeaderView>
#include <QPalette>

#include "blfunctions.h"
#include "ui_blsubformbase.h"
#include "blpostgresqlclient.h"
#include "bltablewidget.h"
#include "bldbsubform.h"
#include "blfixed.h"
#include "blwidget.h"
#include "blplugins.h"

class BlSubForm;

/// Describe una cabecera de recordset preparada para trabajar con SubFormularios \ref BlSubForm
/**
Al crear un subformulario \ref BlSubForm, todos los registros tienen exactamente las mismas cabeceras.
Esta clase sirve para describir una cabecera. Es analoga a la clase \ref BlDbField pero en lugar de
describir un unico registro describe a varios.
*/
class BL_EXPORT BlSubFormHeader
{
public:
    /** Opciones para la columna del SubFormulario
    - DbNone        = Opciones por defecto.
    - DbReadOnly    = La columna es de solo lectura.
    - DbHideView    = Por defecto la columna es invisible aunque puede verse si el usuario lo desea.
    - DbNoWrite     = El usuario no puede escribir en esta columna.
    - DbDisableView = El usuario no podrá ver esta columna ni aun configurandola.
    */
    enum DbOptions {DbNone = 0, DbReadOnly = 1, DbHideView = 2, DbNoWrite = 4, DbDisableView = 8};

protected:
    QString m_fieldName;          /// El nombre de la cabecera en el recordset.
    QString m_fieldTitle;         /// El nombre para mostrar en los errores.
    unsigned int m_fieldRestrictions; /// Las restricciones de la columna en el recordset.
    BlDbField::DbType m_tipo;     /// El tipo de los datos.
    unsigned int m_options;       /// Las opciones para la columna en el SubFormulario.
    int m_numericPrecision;       /// indica la precision en decimales si el campo es de tipo numerico.


public:
    BlSubFormHeader ( QString nom, BlDbField::DbType typ, int res, int options, QString nomp = "" );
    ~BlSubFormHeader();
    unsigned int options();
    unsigned int restrictions();
    void setOptions(int options);
    BlDbField::DbType fieldType();
    QString fieldTitle();
    int fieldRestrictions();
    QString fieldName();
    virtual QString exportXML();
    const int numericPrecision();
    void setNumericPrecision(int pres);
};




/// Necesito exportar algunos datos.
extern BL_EXPORT QModelIndex g_index;
extern BL_EXPORT QWidget *g_editor;
extern BL_EXPORT QAbstractItemModel *g_model;
extern BL_EXPORT QString g_fieldName;


/// Clase BfSubFormDelegate
/** Se encarga del control de los 'Widgets' de edici&oacute;n del sistema.*/
class BL_EXPORT BlSubFormDelegate : public QStyledItemDelegate, public BlMainCompanyPointer
{
    Q_OBJECT

  public:
    BlSubForm *m_subform;
  public:
    explicit BlSubFormDelegate(QObject *);
    virtual ~BlSubFormDelegate();
    virtual void setEditorData ( QWidget *, const QModelIndex &index ) const override;
    virtual void setModelData ( QWidget *editor,  QAbstractItemModel *model, const QModelIndex &index ) const override;
    virtual QWidget *createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;

    virtual bool eventFilter ( QObject *obj, QEvent *event );
    virtual int cerrarEditor ( QWidget *editor );

  protected:
    virtual bool event(QEvent * event);

  signals:
    void cant_changed(BlDbSubFormRecord *) const;

  public slots:
    void readyEdit(QWidget *editor);
};


/// BlSubForm, constructor de la clase base para subformularios.
/** Proporciona un widget que combina la clase QTable con los
registros de la base de datos proporcionando un potente manejador
tabular de datos que puede generar desde listados hasta subformularios
*/
class BL_EXPORT BlSubForm: public BlWidget, public Ui_BlSubFormBase
{
    Q_OBJECT
    Q_PROPERTY(QString tableName READ isDelete WRITE setDbTableName)
    Q_PROPERTY(QString dbFieldId READ isDelete WRITE setDbFieldId)
    Q_PROPERTY(bool isDelete READ isDelete WRITE setDelete USER false)
    Q_PROPERTY(bool isInsert READ isInsert WRITE setInsert USER false)


public:
    BlDbSubFormRecord *m_registrolinea;
    /// Usada para pasar parametros a los plugins.
    BlDbSubFormField  *m_campoactual;
    struct FormatField {
      QString text = "";
      QString style = "";
      QString vAlign = "";
    };

private:
    virtual void load(BlDbRecordSet *cur);
    const QString nameFileConfig(); 
    const QString nameFileDefaultConfig(); 

private:
    QString m_query;
    /// Indica si es modo consulta o modo edicion. (altera el comportamiento del
    /// doble click sobre la lista)
    edmode m_modo;

protected:
    /// Nombre de la tabla en la BD.
    QString m_tablename;
    /// Lista de encabezados del listado. Coordenadas X de la tabla.
    QList<BlSubFormHeader *> m_lcabecera;
    /// Filas y contenido. Coordenadas Y de la tabla.
    QList<BlDbSubFormRecord *> m_lista;
    /// Lista de subformularios marcados para ser borrados.
    QList<BlDbSubFormRecord *> m_listaborrar;
    /// Fichero de configuracion del subformulario.
    QString m_fileconfig;
    /// Campo Identificador de la tabla en la BD.
    QString m_campoid;
    /// Indica si se pueden insertar nuevas filas o no.
    bool m_insercion;
    /// Indica si ya se ha cargado la configuracion o no.
    bool m_primero;
    /// Indica si se pueden borrar registros.
    bool m_delete;
    /// Indica si el subformulario es ordenable o no.
    bool m_sorting;
    /// Indica si se usa ordenacion por la base de datos. (Su uso implica que exista en la tabla el campo orden)
    bool m_orden;
    /// Indica que no se va aplicar ninguna ordenaci&oacute;n a la consulta de la base de datos.
    bool m_ordenporquery;
    /// Indica si procesar cambios en las celdas o no
    bool m_procesacambios;
    /// Texto de la celda que se coge para saber si se tienen que unir con RowSpan.
    QString m_textoceldaParaRowSpan;
    /// Define que columna se va a coger para unir las filas que sean iguales.
    QString m_columnaParaRowSpan;
    /// Se utiliza para saber en que fila comenzamos a contar para unir con RowSpan las filas
    /// que son iguales.
    int m_filaInicialRowSpan;
    QColor m_colorfondo1;
    QColor m_colorfondo2;
    void ponItemColorFondo ( QTableWidget *twidget, int filainicial, int totalfilas, QColor colorfondo );


public:
    /// Indica cual es el &uacute;ltimo campo (fila) que se ha editado.
    int m_prevRow;
    /// Indica cual es el &uacute;ltimo campo (columna) que se ha editado.
    int m_prevCol;

protected:
    /// Carga la configuración de especificaciones.
    /// Los specs son archivos en /etc/bulmages que suplen o Incrementan los campos presentados en un subformulario.
    /// Son utiles para hacer ampliaciones sin programacion.
    void loadSpecs();
    /// Prepara las columnas de la tabla del formulario y pone en
    /// ella los encabezados.
    virtual void pintaCabeceras();
    /// Genera las etiquetas RML necesarias para poder mostrar un dato con el estilo adecuado a su tipo
    /// a la hora de imprimirlo como una celda del "BlockTable" ubicado en el "story" del informe "listado"
    FormatField formatFieldTableStory(BlDbSubFormField *value, QLocale spanish);

public:
    /// Guarda la configuraci&oacute;n de presentaci&oacute;n del subformulario
    /// en el directorio .bulmages situado dentro del directorio 'home' del usuario.
    void saveConfig();
    void saveConfigXML();
    /// Carga la configuraci&oacute;n que ten&iacute;a el formulario la &uacute;ltima
    /// vez que fue utilizado por el usuario.
    void loadConfig();
    void loadConfigXML();
    /// Devuelve la lista de registros de la base de datos que
    /// se presentan en el subformulario.
    QList<BlDbSubFormRecord *> *lista();
    /// Devuelve la lista de cabeceras que forman el subformulario.
    QList<BlSubFormHeader *> *headerList();

public:
    explicit BlSubForm(QWidget *parent);
    virtual ~BlSubForm();
    virtual void setMainCompany(BlMainCompany *) override;
    void setProcesarCambios ( bool );
    bool procesaCambios();
    /// Define si el listado presentar&aacute; las filas con colores alternativos.
    void setListadoPijama ( bool pijama );
    bool listadoPijama();
    /// Establece el modo de funcionamiento del scroll horizontal.
    void setHorizontalScrollMode ( QAbstractItemView::ScrollMode mode );
    /// Establece los colores para dibujar el fondo de las filas.
    /// Si la propiedad 'alternatingRowColors' está en 'true' entonces de usan
    /// los dos colores, si está en 'false' solo se usa 'ColorFondo1'.
    /// Estos valores corresponden a QPalette::Base y QPalette::AlternateBase.
    void setColorFondo1 ( QString color );
    void setColorFondo2 ( QString color );
    /// Establece que los elementos del subformulario son ordenables
    void setSortingEnabled ( bool );
    /// Indica si los elementos del subformulario son ordenables.
    bool sortingEnabled();
    /// Establece en el subformulario un modo de funcionamiento mediante
    /// el cual el orden de los elementos se almacena en la base de datos
    /// mediante un campo orden"tabla" de tipo num&eacute;rico.
    void setOrdenEnabled ( bool );
    void setOrdenPorQuery ( bool );
    /// Indica si esta establecido el modo de ordenaci&oacute;n en la base
    /// de datos.
    bool ordenEnabled();
    /// Indica si una fila determinada del subformulario esta lista
    /// para intentar ser guardada en la base de datos. O lo que es lo mismo
    /// si todos los campos necesarios para guardar el registro est&aacute;n
    /// rellenados
    bool campoCompleto ( int row );
    /// Establece el modo de borrado de registros que permite al subformulario
    /// borrar filas con su correspondiente borrado en la base de datos.
    void setDelete ( bool f );
    bool isDelete ();
    /// Devuelve el elemento del subformulario ubicado en la posici&oacute;n
    /// indicada
    BlDbSubFormField *item ( int row, int col );
    /// Pone el foco en una celda determinada.
    void setCurrentItem ( int row, int col );
    /// Devuelve el n&uacute;mero total de columnas del subformulario.
    int columnCount();
    /// Devuelve la posici&oacute;n de la columna con el nombre indicado.
    int columnNumber ( const QString & );
    /// Mostrar u ocultar una columna
    void setColumnVisible ( int, bool ); // inline
    void setColumnVisible ( const QString &, bool ); // inline
    /// Muestra la columna indicada en el subformulario.
    void showColumn ( int i );
    /// Devuelve el ancho de la columna indicada.
    int columnWidth ( int i );
    /// Establece el ancho de la columna indicada.
    void setColumnWidth ( int i, int j );
    /// Oculta la columna indicada.
    void hideColumn ( int i );
    /// Devuelve la fila que en la actualidad posee el foco en el subformulario.
    int currentRow();
    /// Devuelve el n&uacute;mero de filas en el subformulario.
    int rowCount();
    /// Devuelve la columna que en la actualidad posee el foco del subformulario.
    int currentColumn();
    /// Establece la forma de redimensionado de las columnas del subformulario.
    void setResizeMode ( QHeaderView::ResizeMode modo );

    /// Redimensiona las columnas del subformulario para ajustarlas a los contenidos
    void resizeColumnToContents ( int i );
    /// Ajusta la fila indicada a sus contenidos.
    void resizeRowToContents ( int i );
    /// Oculta el configurador del subformulario
    void hideConfig();
    /// Muestra el configurador del subformulario
    void showConfig();
    /// Hace la impresion del subformulario en formato de tabla RML.
    QString imprimir();
    void sortItems ( int col, Qt::SortOrder orden );
    void setInsert ( bool b );
    bool isInsert ();
    /// Define que campo se utilizar&aacute; para unir las filas que tengan el mismo texto (rowSpan).
    /// Es &uacute;til en contabilidad para unir filas del mismo asiento.
    void setColumnToRowSpan ( QString campo );
    void setDbTableName ( QString nom );
    QString tableName();
    void setFileConfig ( QString nom );
    QString fileConfig();
    void setDbFieldId ( QString nom );
    QString dbFieldId();
    void clear();
    void setRowCount ( int i );
    void setColumnCount ( int i );
    void situarse ( unsigned int, unsigned int, bool back = false );
    void situarse1 ( unsigned int, unsigned int );
    int addSubFormHeader ( QString nom, BlDbField::DbType typ, int res, int opt, QString nomp );
    void removeSubFormHeader ( QString nom );
    BlDbSubFormRecord *newDbSubFormRecord();
    virtual void setColumnValue ( QString, QString );
    QString dbValue ( const QString &campo, int row = -1 );
    void setDbValue ( const QString &campo, int row, const QString &valor );
    BlDbSubFormRecord *lineaact();
    BlDbSubFormRecord *lineaat ( int row );
    virtual void newRecord();
    virtual int remove ( int );
    virtual int save();
    virtual void load ( QString query );
    virtual int inicializar();
    virtual void pintar();
    virtual void createMenu ( QMenu * );
    virtual void execMenuAction ( QAction * );
    virtual BlFixed sumarCampo ( QString );
    bool existsHeader ( const QString & );
    BlSubFormHeader *header ( const QString & );
    virtual int cerrarEditor();
    virtual void confquery();
    QString dbFieldNameByColumnId ( int columna );
    QString dbFieldViewNameByColumnId ( int columna );
    BlDbField::DbType fieldTypeByColumnId ( int columna );

    /// Funciones para manipular los modos.
    void setSelectMode();
    void setEditMode();
    bool editMode();
    bool selectMode();
    edmode mode();

    /// Metodos relacionados con el menu superior del SubFormulario
    void preparaMenu();
    
    virtual QString exportXML();
    
    QString likeFilterSQL(const QString &text);

public slots:
    virtual void printPDF ( const QString & );
    virtual int remove();
    void columnMovedByUser(int column, int oldIndex, int newIndex);
    virtual void on_mui_list_cellRePosition ( int, int );
    void on_mui_list_cellChanged ( int, int );
    virtual void contextMenuEvent ( QContextMenuEvent * );
    virtual void on_mui_confcol_clicked();
    virtual void on_mui_confquery_clicked();
    void on_mui_list_pressedSlash ( int row, int col );
    void on_mui_list_pressedAsterisk ( int row, int col );
    void on_mui_list_pressedPlus ( int row, int col );
    void on_mui_list_pressedMinus ( int row, int col );
    virtual void on_mui_list_ctrlUp ( int row, int col );
    virtual void on_mui_list_ctrlDown ( int row, int col );
    virtual void on_mui_pagsiguiente_clicked();
    virtual void on_mui_paganterior_clicked();
    virtual void on_mui_list_itemDoubleClicked ( QTableWidgetItem *item );
    virtual void on_mui_list_itemClicked ( QTableWidgetItem *item );
    virtual void on_mui_list_cellDoubleClicked ( int row, int col );
    virtual void on_mui_botonCerrar_clicked();
    virtual void on_mui_list_itemChanged ( QTableWidgetItem *it );
    virtual void editFinished ( int, int, BlDbSubFormRecord *, BlDbSubFormField * );
    virtual void pressedAsterisk ( int, int, BlDbSubFormRecord *, BlDbSubFormField * );
    virtual void pressedPlus ( int, int, BlDbSubFormRecord *, BlDbSubFormField * );
    virtual void pressedMinus ( int, int, BlDbSubFormRecord *, BlDbSubFormField * );
    virtual void pressedSlash ( int, int, BlDbSubFormRecord *, BlDbSubFormField * );
    /// Redimensiona las columnas del subformulario para ajustarlas
    /// a los contenidos.
    void resizeColumnsToContents();
    /// Redimensiona las filas del subformulario para ajustarlas
    /// a los contenidos.
    void resizeRowsToContents();
    virtual void toogleConfig();
    virtual void toogleMenuConfig();
    virtual int deleteCurrentRow();
    virtual void bpressedSlash ( );
    virtual void bSubir ( );
    virtual void bBajar ( );
    
signals:
    void toogledConfig ( bool );
    void toogledMenuConfig ( bool );
    void editFinish ( int, int );
    void pushAsterisk ( int, int );
    void pushPlus ( int, int );
    void pushMinus ( int, int );
    void pushSlash ( int, int );
    void itemDoubleClicked ( QTableWidgetItem * );
    void itemClicked ( QTableWidgetItem * );
    void cellDoubleClicked ( int row, int col );
    void pintaMenu ( QMenu * );
    void trataMenu ( QAction * );
};


inline void BlSubForm::setColumnVisible ( int i, bool visible )
{
    if ( visible ) {
        showColumn ( i );
    }
    else {
        hideColumn ( i );
    } // end if
}


inline void BlSubForm::setColumnVisible ( const QString &headerName, bool visible )
{
    setColumnVisible ( columnNumber ( headerName ), visible );
}

#endif

