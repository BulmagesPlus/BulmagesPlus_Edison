/***************************************************************************
 *   Copyright (C) 2012 by Aron Galdón.                                    *
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

#include "blpicture.h"

/// Constructor
/**
\param padre
\param es_prev_flotante false por defecto (cuando es true, quiere decir que esta etiqueta es un popup flotante)
**/
BlPicture::BlPicture ( QWidget *parent, bool es_prev_flotante)
    : QLabel ( parent )
    , m_doble_click ( false )
    , m_width ( 320 )
    , m_quality ( 85 )
{
    BL_FUNC_DEBUG

    m_height = 3 * m_width / 4;

    /// Si se trata de una imagen incrustada y reducida, crear su versi&oacute;n ampliada flotante
    if ( !es_prev_flotante )
    {
        m_prev_ori = new BlPicture ( this, true );
        m_prev_ori->setWindowFlags ( Qt::Popup );
    }
    else {
        /// Si la imagen actual es ampliada flotante, el puntero a NULL indicar&aacute; que lo es
        m_prev_ori = NULL;
    } // end if

    installEventFilter ( this );
}


/// Destructor
/**
**/
BlPicture::~BlPicture()
{
    BL_FUNC_DEBUG

    /// Ocultar la imagen flotante si se qued&oacute; visible
    if ( m_prev_ori != NULL ) {
        m_prev_ori->hide();
    } // end if
}


/// Cargar una imagen en la etiqueta
/**
\param path
   \return Verdadero si to fue bien
**/
bool BlPicture::load_picture ( const QString path )
{
    BL_FUNC_DEBUG

    if ( ( path != thumbnail_path ( m_picpath ) )
     && ( path != original_path ( m_picpath ) ) )
    {
        detect_picture_path_to_show ( path );
        clear();
        QFileInfo fich( m_picpath );

        /// Si no hay ruta de imagen o no es correcta, salir del m&eacute;todo
        if ( m_picpath.isEmpty() || !fich.exists() || fich.isDir() )
        {
            m_picpath.clear();
            m_ima = QImage();
            return false;
        } // end if

        /// Ser&aacute; "verdadero" s&oacute;lo si hay una imagen cargada
        bool cargada = false;

        cargada = m_ima.load ( m_picpath );

        if ( !cargada || m_ima.isNull() ) {
            return false;
        } // end if
    } // end if

    /// Esto podr&iacute;a pasar si no es la primera vez consecutiva que la imagen est&aacute; vac&iacute;a
    /// Me di cuenta por el aviso: QImage::scaled: Image is a null image
    if ( m_ima.isNull() ) {
        return true;
    } // end if

    /// S&oacute;lo en el caso de que sea una previsualizaci&oacute;n en grande,
    /// ajustar el tama&ntilde;o de la etiqueta si la imagen es menor (as&iacute; evitamos pixelar la imagen si la etiqueta es redimensionable)
    if ( m_prev_ori == NULL) {
        show_original_picture();
    }
    else {
        /// Establecer la imagen de modo que se adapte a la etiqueta pero manteniendo su relaci&oacute;n de aspecto
        QLabel::setPixmap(QPixmap::fromImage ( m_ima.scaled(size(), Qt::KeepAspectRatio) ) );
    } // end if

    return true;
}


/// Guardar la imagen visualizada en la etiqueta
/**
\param path
   \path If empty, uses the loading path
   \format
   \quality
   \return Verdadero si todo fue bien
**/
bool BlPicture::save_picture ( QString path, const char *format, int quality )
{
    BL_FUNC_DEBUG

    if ( path.isEmpty()) {
        path = m_picpath;
    } // end if

    if ( quality <= -2 ) {
        quality = m_quality;
    } // end if

    bool result = true;
    result = pixmap()->save ( path, format, quality );
    return result;
}


/// Gestiona una lista desplegable de im&aacute;genes existentes en una ruta para poder seleccionar una
/**
\param prev_ima Puntero a la etiqueta de previsualizaci&oacute;n de la imagen
\param desple Puntero a la lista desplegable
\param ruta Ruta de las im&aacute;genes
\param ref_ruta_ant_ima Ruta anterior pasada por referencia
**/
void BlPicture::refresh_combo_pics_dir ( QComboBox *desple, QDir ruta, QString &ref_ruta_ant_ima )
{
    BL_FUNC_DEBUG

    QFileInfoList images = search_images(ruta );

    /// Antes de limpiar la lista desplegable, nos guardamos la ruta que hab&iacute;a
    QString ruta_actual_imagen = desple->currentText();

    QStringList rutas;
    bool existe_imagen = false;

    /// Poner en la lista las rutas de las im&aacute;genes detectadas en la carpeta
    foreach(QFileInfo f, images ) {
        rutas << f.absoluteFilePath();
        if ( ruta.absolutePath() == rutas.last() ) {
            existe_imagen = true;
        } // end if
    } // end foreach

    if ( rutas.count() != desple->count() || !existe_imagen )
    {
        /// Borrar todos los elementos de la lista
        desple->clear();

        /// Poner en la lista desplegable las rutas de las im&aacute;genes detectadas en la carpeta
        foreach(QFileInfo f, images )
            desple->addItem( f.absoluteFilePath() );

        QFile fic(ruta_actual_imagen );

        /// Recuperar la ruta que hab&iacute;a antes de limpiar el listado (si es correcta)
        if ( !ruta_actual_imagen.isEmpty() && fic.exists() ) {
            desple->setEditText(ruta_actual_imagen );
        }
        else {
            /// Usar la primera ruta si no hab&iacute;a una antes de la limpieza
            desple->setCurrentIndex(0);
        } // end if
    } // end if

    /// Poner en el campo la ruta de la imagen original o de la miniatura si es necesario
    ruta_actual_imagen = get_thumbnail_path ( desple->currentText() );

    /// Cargar la imagen si ha cambiado la ruta
    if ( ref_ruta_ant_ima != ruta_actual_imagen )
    {
        desple->setEditText ( ruta_actual_imagen );

        /// Intentar cargar la ruta escrita
        bool cargada = load_picture ( ruta_actual_imagen );

        /// Bucle que busca alguna imagen que se pueda cargar en caso de que la ruta indicada no haya funcionado
        const int total = desple->count();
        for ( int i = 0; !cargada && i < total; i++ )
        {
            desple->setCurrentIndex(i);
            cargada = load_picture ( desple->currentText() );
        } // end for
    } // end if
}


/// Si la imagen supera las medidas establecidas, genera una versi&oacute;n reducida de la misma
/// y devuelve la ruta. En caso contrario, devuelve la ruta a la misma imagen.
/**
  \param fich_imagen
  \return
**/
QString BlPicture::get_thumbnail_path ( QFileInfo fich_imagen )
{
    BL_FUNC_DEBUG

    /// Si no hay ruta de imagen o no es correcta, salir del m&eacute;todo
    if ( !fich_imagen.exists() || fich_imagen.isDir() ) {
        return "";
    } // end if

    QString ruta_imagen = fich_imagen.absoluteFilePath();

    QImage imagen;
    bool cargada = imagen.load ( ruta_imagen );

    if ( !cargada ) {
        return "";
    } // end if

    if (imagen.width() > m_width )
    {
        /// Generar una versi&oacute;n reducida de la imagen
        QImage mini = imagen.scaled ( m_width, m_height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );

        ruta_imagen = thumbnail_path ( ruta_imagen );

        /// Guardar la versi&oacute;n reducida en un nuevo fichero
        bool guardada = mini.save ( ruta_imagen, 0, m_quality);

        /// Si no se pudo guardar la versi&oacute;n reducida, usar la imagen original como miniatura
        if ( !guardada )
            ruta_imagen = fich_imagen.absoluteFilePath();

        /// Dar permiso de escritura al grupo "users" sobre la imagen reducida
        QFile ( ruta_imagen ).setPermissions ( QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner|QFile::ReadGroup|QFile::WriteGroup|QFile::ExeGroup);
    } // end if

    return ruta_imagen;
}


///
/**
**/
void BlPicture::detect_picture_path_to_show ( QString path )
{
    m_picpath = "";

    if ( !path.isEmpty() )
    {
        QFileInfo fic_original ( original_path ( path ) );
        QFileInfo fic_mini ( thumbnail_path ( path ) );

        /// Si estamos ante una previsualizaci&oacute;n en grande...
        if ( m_prev_ori == NULL )
        {

            /// ...usa la imagen original si todav&iacute;a existe
            if ( fic_original.exists() ) {
                m_picpath = original_path ( path );
            }
            else {
                if ( fic_mini.exists() ) {
                    m_picpath = thumbnail_path ( path );
                } // end if
            } // end if
        }
        else
        {
            if ( fic_mini.exists() ) {
                m_picpath = thumbnail_path ( path );
            }
            else {
                if ( fic_original.exists() ) {
                    m_picpath = original_path ( path );
                } // end if
            } // end if
        } // end if
    } // end if
}


/// Captura los eventos de dobles pulsaciones de rat&oacute;n
/**
\param evento
**/
void BlPicture::mouseDoubleClickEvent ( QMouseEvent *evento )
{
    BL_FUNC_DEBUG

    if ( evento->type() == QEvent::MouseButtonDblClick) {
        m_doble_click = true;
    } // end if
}


/// Captura los eventos de pulsaciones de rat&oacute;n
/**
\param evento
**/
void BlPicture::mousePressEvent ( QMouseEvent *evento )
{
    BL_FUNC_DEBUG

    /// Un click sobre la previsualizaci&oacute;n popup simplemente la cierra
    if ( m_prev_ori == NULL)
    {
        hide();
        return;
    } // end if

    /// Con click izquierdo, visualizar/ocultar la imagen en un popup
    if ( evento->button() == Qt::LeftButton && !m_doble_click)
    {
        if ( m_prev_ori->isVisible() || m_picpath.isEmpty() ) {
            return;
        } // end if

        m_prev_ori->load_picture ( m_picpath );
    } // end if

    /// Con click derecho o central, abrir la imagen desde NxSpooler
    if ( evento->button() == Qt::RightButton || evento->button() == Qt::MidButton )
    {
        m_prev_ori->hide();
        if ( !m_picpath.isEmpty() ) {
            emit special_click( m_picpath );
        } // end if
    } // end if

    m_doble_click = false;
}


///
/**
**/
QString BlPicture::original_path ( QString ruta )
{
    BL_FUNC_DEBUG

    if ( !ruta.isEmpty() ) {
        ruta = ruta.replace(QString ( "_mini%1_" ).arg ( m_width ), "" );
    } // end if
    return ruta;
}


///
/**
**/
QString BlPicture::thumbnail_path ( QString ruta )
{
    BL_FUNC_DEBUG

    if ( !ruta.isEmpty() && !ruta.contains ( QString ( "_mini%1_" ).arg ( m_width ) ) )
    {
        QFileInfo fic ( ruta );
        QString nom_fic = fic.fileName();
        ruta = fic.absolutePath();
        ruta += QDir::separator();

        if (nom_fic.startsWith ( "_mini" ) ) {
            nom_fic.replace(QRegExp( "_mini.*_" ), QString ( "_mini%1_" ).arg ( m_width ) );
        }
        else {
            nom_fic.prepend ( QString ( "_mini%1_" ).arg ( m_width ) );
        } // end if

        ruta += nom_fic;
    } // end if

    return ruta;
}


/// 
/**
**/
void BlPicture::show_original_picture()
{
    BL_FUNC_DEBUG

    const int marg = 2; ///< Tama&ntilde;o del m&aacute;rgen entre imagen y borde del widget
    const int max_w = g_theApp->desktop()->width() - ( marg * 2 ); ///< L&iacute;mite de tama&ntilde;o horizontal
    const int max_h = g_theApp->desktop()->height() - ( marg * 2 ); ///< L&iacute;mite de tama&ntilde;o vertical
    int pic_w = m_ima.width(); ///< Anchura adaptada a la pantalla (inicialmente es la original)
    int pic_h = m_ima.height(); ///< Altura adaptada a la pantalla (inicialmente es la original)
    int x = 0; ///< Posici&oacute;n izquierda del widget
    int y = 0; ///< Posici&oacute;n superior del widget

    if ( pic_w >= max_w )
    {
        pic_w = max_w;

        // Ajustar la altura en proporci&oacute;n
        pic_h = pic_h * pic_w / m_ima.width();
    } // end if

    if ( pic_h >= max_h )
    {
        int pre_pic_h = pic_h;
        pic_h = max_h;

        // Ajustar la anchura en proporci&oacute;n
        pic_w = pic_w * pic_h / pre_pic_h;
    } // end if

    if ( pic_w < max_w )
    {
        x = ( max_w + ( marg * 2 ) - pic_w ) / 2;
    } // end if

    if ( pic_h < max_h )
    {
        y = ( max_h + ( marg * 2 ) - pic_h ) / 2;
    } // end if

    setGeometry ( x, y, pic_w + ( marg * 2 ), pic_h + ( marg * 2 ) );
    setFrameStyle ( QFrame::Panel | QFrame::Raised );
    setAlignment ( Qt::AlignCenter );

    /// Establecer la imagen de modo que se adapte a la etiqueta pero manteniendo su relaci&oacute;n de aspecto
    QLabel::setPixmap ( QPixmap::fromImage ( m_ima.scaled ( QSize ( pic_w, pic_h ), Qt::KeepAspectRatio) ) );

//    blMsgInfo( _( "x=%1 y=%2 pic_h=%3 pic_w=%4 \n").arg ( x ).arg ( y ).arg ( pic_h ).arg ( pic_w )
//             + _( "m_ima.height=%5 m_ima.width=%6 max_h=%7 max_w=%8" ).arg ( m_ima.height() ).arg ( m_ima.width() ).arg ( max_h ).arg ( max_w ) );

    show();
}


///
/**
**/
QFileInfoList BlPicture::search_images ( QDir ruta )
{
    BL_FUNC_DEBUG

    QStringList filtros;
    filtros
            << "*.jpg"
            << "*.bmp"
            << "*.png";
    ruta.setNameFilters( filtros );
    return ruta.entryInfoList();
}


///
/**
  \pix
**/
void BlPicture::setPixmap ( const QPixmap &pix )
{
    BL_FUNC_DEBUG
    m_picpath.clear();
    QLabel::setPixmap ( pix );
}


///
/**
  \pic
**/
void BlPicture::setPicture ( const QPicture &pic )
{
    BL_FUNC_DEBUG
    m_picpath.clear();
    QLabel::setPicture ( pic );
}


///
/**
  \mov
**/
void BlPicture::setMovie ( QMovie *mov )
{
    BL_FUNC_DEBUG
    m_picpath.clear();
    QLabel::setMovie ( mov );
}
