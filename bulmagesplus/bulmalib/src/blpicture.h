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

#ifndef BLPICTURE_H
#define BLPICTURE_H

#include <QComboBox>
#include <QLabel>
#include <QDir>
#include <QString>
#include <QMouseEvent>
#include "blfunctions.h"

class BL_EXPORT BlPicture
        : public QLabel
{
    Q_OBJECT

    public:
        explicit BlPicture(QWidget *parent = nullptr, bool es_prev_flotante = false);
        ~BlPicture();
        bool load_picture ( const QString );
        bool save_picture ( QString path = "", const char *format = 0, int quality = -2 );
        void refresh_combo_pics_dir ( QComboBox *, QDir, QString & );
        QString get_thumbnail_path ( QFileInfo );

    protected:
        void detect_picture_path_to_show ( QString );
        void mouseDoubleClickEvent ( QMouseEvent * );
        void mousePressEvent ( QMouseEvent * );
        QString original_path ( QString );
        QString thumbnail_path ( QString );
        void show_original_picture();
        QFileInfoList search_images ( QDir );

    private:
        bool m_doble_click; ///< Verdadero si hay un evento activo de doble click
        int m_width; ///< Anchura de la miniatura en pixels
        int m_height; ///< Altura de la miniatura en pixels
        int m_quality; ///< Calidad de la reducción de imagen para miniatura
        QString m_picpath; ///< Ruta en la que se encuentra la imagen cargada
        QImage m_ima; ///< Imagen cargada y manipulable
        BlPicture *m_prev_ori; ///< Objeto para almacenar a la visualización al tamaño original

    public slots:
        void setPixmap(const QPixmap &);
        void setPicture(const QPicture &);
        void setMovie(QMovie *);

    signals:
        void special_click(QString); ///< Señal a emitir cuando se haga un click sobre la imagen con el botón central o el derecho
};

#endif
