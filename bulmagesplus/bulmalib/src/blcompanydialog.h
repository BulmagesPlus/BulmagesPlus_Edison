/***************************************************************************
 *   Copyright (C) 2002 by Tomeu Borrás Riera.                             *
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

#ifndef BLCOMPANYDIALOG_H
#define BLCOMPANYDIALOG_H

#include <QDialog>
#include "ui_blcompanydialogbase.h"

QT_FORWARD_DECLARE_CLASS(QString);
QT_FORWARD_DECLARE_CLASS(QWidget);
QT_FORWARD_DECLARE_CLASS(QObject);
QT_FORWARD_DECLARE_CLASS(QEvent);
QT_FORWARD_DECLARE_CLASS(QFile);
QT_FORWARD_DECLARE_CLASS(QStringList);


/// Abre el diálogo de seleccionar una empresa.
/** Esta clase muestra el diálogo de seleccionar una empresa y espera a que se haga doble
    click sobre uno de los elementos que contiene, se haga click en aceptar o se de la
    accion de cancelar.
    La ventana siempre se abre en modo 'modal' y detiene la ejecución del programa.
    Se puede especificar, si es preciso, un tipo de mui_empresas a mostrar
    (bulmacont o bulmafact) para facilitar la selección de elementos.
**/
class BL_EXPORT BlCompanyDialog : public QDialog, public Ui_BlCompanyDialogBase
{
  Q_OBJECT

  public:
    /// Asigna el modo de la ventana a destructivo (true) o no destructivo (false).
    void setDestroyMode(bool destroyMode);
    /// Inicia las variables m_tipo y m_tipoempresa y carga el archivo para hacer la
    /// presentación.
    explicit BlCompanyDialog(QStringList tipo, QWidget * parent = nullptr, const char * name = 0, bool modal = true);
    ~BlCompanyDialog();
    /// Devuelve el nombre de la base de datos seleccionada.
    QString dbName();
    /// Devuelve el nombre de la empresa seleccionada.
    QString companyName();
    /// Devuelve el tipo de empresa que se ha seleccionado.
    QString companyType();
    /// Devuelve si se está en modo destructivo o no.
    bool isDestroyMode();
    bool isNormalMode();
    QString lastUsedDatabase();
    bool saveNormalMode();
    bool saveExtendedMode();
    bool saveLastUsedDatabase(QString dbName);

  public slots:
    /// Este SLOT se activa al seleccionar una compañía y pulsar INTRO.
    virtual void accept();
    /// This SLOT searches in the databases of postgres all the companies needed.
    virtual void on_mui_actualizar_clicked();

  private:
    /// Se usa para almacenar el nombre de la base de datos seleccionada.
    QString m_empresabd;
    /// Se usa para almacenar el nombre de la empresa seleccionada.
    QString m_nombreempresa;
    /// Se usa para tener presente que tipo de elementos queremos que el selector muestre.
    QStringList m_tipo;
    /// Se usa para devolver el tipo de empresa que se ha seleccionado en el selector.
    QString m_tipoempresa;
    /// Almacena el nombre del usuario que desea acceder.
    QString user;
    /// Almacena el password del usuario que desa acceder.
    QString password;
    /// Indica el modo en que opera la ventana. Cerrando la aplicacion al cancelar
    /// o cerrando solo la ventana. Por defecto es 0 -> cerrar aplicación.
    bool m_modo;
    /// Guarda en el archivo especificado en LISTEMPRESAS la lista de mui_empresas disponibles.
    void saveFile();
    /// Carga del archivo especificado en LISTEMPRESAS la lista de mui_empresas disponibles.
    void loadFile();
    /// Inserta una compañía en el QList mui_empresas definido en el diálogo.
    void insertCompany(QString, QString, QString, QString);
    void preparamui_empresas();
    void findCompanyDatabase(QString, QFile *);

    /// Asigna el nombre de la base de datos seleccionada.
    void setDbName(QString dbName);
    /// Asigna el nombre de la empresa seleccionada.
    void setCompanyName(QString companyName);
    /// Asigna el tipo de empresa que se ha seleccionado.
    void setCompanyType(QString companyType);

  private slots:
    /// Al pulsar el botón de cancelar se aborta la ejecución del programa.
    void on_mui_cancelar_clicked();
    void on_mui_aceptar_clicked();
    bool close(bool);
    void mui_empresasdobleclick();

    void on_mui_checkNormal_toggled(bool toggled);
    void on_mui_checkExtended_toggled(bool toggled);

    /// Invita al usuario a visitar la web de bulmagesplus
    void visitBulmagesPlus();

  protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif
