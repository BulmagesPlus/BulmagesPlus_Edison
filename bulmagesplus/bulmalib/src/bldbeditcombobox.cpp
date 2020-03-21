#include <QCoreApplication>
#include <QKeyEvent>
#include <QEvent>
#include <QWidget>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDebug>
#include <QMenu>
#include "blplugins.h"
#include "bldbeditcombobox.h"


/**
\param parent
**/
BlDbEditComboBox::BlDbEditComboBox ( QWidget *parent )
        : BlComboBox ( parent )
{
    BL_FUNC_DEBUG

    m_cursorcombo = NULL;
    /// Desconectamos el activated ya que en los subformularios no tiene que funcionar.
    disconnect ( this, SIGNAL ( activated ( int ) ), 0, 0 );
    setContextMenuPolicy ( Qt::DefaultContextMenu );

    setEditable ( true );
    connect ( this, SIGNAL ( editTextChanged ( const QString & ) ), this, SLOT ( s_editTextChanged ( const QString & ) ) );

    installEventFilter(this);
    m_view = static_cast<QObject *>(view());
    view()->installEventFilter(this);
}


/** Libera la memoria reservada. */
/**
**/
BlDbEditComboBox::~BlDbEditComboBox()
{
    BL_FUNC_DEBUG
}


/** Permite indicar al Widget cual es la serie de factura seleccionada por defecto.
    Recarga cursor de serie_factura y cuando encuentra un registro cuyo codigoserie_factura coincide con el pasado
    como parametro lo establece como el registro activo por el comboBox.
*/
/**
\param cod
\return
**/
void BlDbEditComboBox::setQuery ( const QString &query )
{
    BL_FUNC_DEBUG

    m_cursorcombo = mainCompany() ->loadQuery ( query );
    clear();
    while ( !m_cursorcombo->eof() )
    {
      addItem ( m_cursorcombo->value(0) );
      m_cursorcombo->nextRecord();
    } // end while

    delete m_cursorcombo;
}


/// Retorna el codi associat a la unica entrada del combo que
/// hagi estat trobada a la BD a partir de l'entrada de l'usuari. Aixo
/// permet que abans de donar un error per codi d'article incorrecte
/// se li assigni l'unic article trobat per l'entrada (incompleta?) de l'usuari.
/// Retorna NULL si no se n'ha trobat cap o se n'ha trobat mes d'un.
QString BlDbEditComboBox::unicaEleccion ( void )
{
    BL_FUNC_DEBUG

    int num = 0;
    QString elec = NULL;
    for ( int i = 0; ( num < 2 ) && ( i < count() ); i++ ) {
        if ( itemData ( i ).isValid() ) {
            elec = itemData ( i ).toString();
            num++;
        } // end if
    } // end for

    return ( num == 1 ? elec : NULL );
}


/// Sii el combo nomes ha trobat un article a la BD per l'entrada de
/// l'usuari substitueix el text entrat per l'entrada del combo de l'article trobat.
QString BlDbEditComboBox::eligeUnico ( void )
{
    BL_FUNC_DEBUG

    BlDebug::blDebug ( "BlDbEditComboBox::eligeUnico", 0, "count = " + QString::number ( count() ) );

    QString elec = unicaEleccion();
    if ( !elec.isNull() ) {
        BlDebug::blDebug ( "elec=" + elec, 0 );
        setEditText ( elec );
    } // end if

    return elec;
}


QString BlDbEditComboBox::entrada()
{
    BL_FUNC_DEBUG

    return m_entrada;
}


/**
\param cod
\return
**/
void BlDbEditComboBox::s_editTextChanged ( const QString &cod )
{
    BL_FUNC_DEBUG

    BlDebug::blDebug ( "BlDbCompleterComboBox::s_editTextChanged", 0, cod );
    static bool semaforo = false;
    if ( semaforo ) {
        return;
    } else {
        semaforo = true;
    } // end if
    m_entrada = cod;

    if ( !g_plugins->run ( "BlDbCompleterComboBox_textChanged", this ) )
    {
        QString codigo = m_entrada;
        if ( codigo.size() >= g_confpr->value(CONF_NUMCHAR_RELOAD_FILTRO).toInt() )
        {
            int pos = codigo.indexOf ( ".-" );
            // no se si es el autoComplete o què però em criden a
            // aquesta senyal quan omplo el combo, amb el primer valor
            // i si no m'aturo ara, recalcularia el combo amb nomes
            // aquest valor encara que l'usuari nomes hagi escrit
            // un prefix que permeti mes candidats
            if ( pos < 0 )
            {
                QString cadwhere = "";
                /// Inicializamos los valores de vuelta a ""
                QMapIterator<QString, QString> i ( m_valores );
                QString cador = "";
                while ( i.hasNext() )
                {
                    i.next();
                    cadwhere = cadwhere + cador + " upper( COALESCE(" + i.key() + ",'')::varchar ) LIKE  upper('%" + mainCompany()->sanearCadenaUtf8(cod) + "%')";
                    cador = " OR ";
                } // end while
                QString SQLQuery = "SELECT * FROM " + tableName() + " WHERE " + cadwhere;

                m_cursorcombo = mainCompany() ->loadQuery ( SQLQuery );
                clear();
                while ( !m_cursorcombo->eof() )
                {
                    QMapIterator<QString, QString> i ( m_valores );
                    QString cad = "";
                    QString sep = ".- ";
                    QString cad1 = "";
                    while ( i.hasNext() )
                    {
                        i.next();
                        cad = cad + m_cursorcombo->value( i.key() ) + sep;
                        if ( sep == ".- " ) {
                            cad1 = i.key();
                            sep = " ";
                        } // end if
                    } // end while
                    addItem ( cad , QVariant ( m_cursorcombo->value( cad1 ) ) );
                    m_cursorcombo->nextRecord();
                } // end while
                delete m_cursorcombo;
            } // end if
        } // end if
    } // end if
    g_plugins->run ( "BlDbCompleterComboBox_textChanged_Post", this );
    setEditText ( cod );
    semaforo = false;
}


bool BlDbEditComboBox::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab)
        {
            return true;
        }
    }

    return QWidget::event(event);
}


bool BlDbEditComboBox::eventFilter(QObject *obj, QEvent *event)
{

    if ( event->type() == QEvent::KeyRelease )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> ( event );
        int key = keyEvent->key();
        switch ( key )
        {
            case Qt::Key_F2:
              emit keyF2();
              return true;
              break;
        } // end switch
    } // end if

    if (event->type() == QEvent::FocusIn)
    {
    } // end if

    if (event->type() == QEvent::FocusOut)
    {
        bool focoFuera = false;
        QWidget *currentFocusWidget = QApplication::focusWidget();
        if (currentFocusWidget)
        {
          if ((currentFocusWidget == this) || (currentFocusWidget == m_view))
          {
            /// Do nothing.
          } else
          {
              focoFuera = true;
          } // end if
        } else
        {
            focoFuera = true;
        } // end if

        if (focoFuera)
        {
              /// Es focusout
              eligeUnico();
              BlComboBox::focusOutEvent ( static_cast<QFocusEvent *>(event) );
              emit focusOut();
              return true;
        } // end if

    } // end if

    return false;
}


void BlDbEditComboBox::contextMenuEvent(QContextMenuEvent *event)
{
    BL_FUNC_DEBUG

    QMenu *popup = new QMenu ( this );

    /// Lanzamos el evento para que pueda ser capturado por terceros.
    emit pintaMenu ( popup );

    QAction *actionCopy = popup->addAction ( _ ( "Copiar " ) );
    QAction *actionPaste = popup->addAction ( _ ( "Pegar" ) );
    QAction *opcion = popup->exec ( QCursor::pos() );

    popup->popup(mapToGlobal(pos()));

    if (opcion)
    {
        if (opcion == actionCopy)
        {
          if (lineEdit() && lineEdit()->hasSelectedText())
          {
            lineEdit()->copy();
          } // end if
        } else if (opcion == actionPaste)
        {
          if (lineEdit())
          {
            lineEdit()->paste();
          } // end if
        } else
        {
          emit trataMenu ( opcion );
        } // end if
    } // end if

    delete popup;
}

