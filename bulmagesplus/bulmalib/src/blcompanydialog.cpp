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

#include <fstream>
#include <stdio.h>

#include <QLineEdit>
#include <QMessageBox>
#include <QString>
#include <QList>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QStringList>
#include <QHeaderView>
#include <QKeyEvent>
#include <QEvent>
#include <QTextStream>
#include <QTextCodec>
#include <QString>
#include <QFile>
#include <QStringList>
#include "blfunctions.h"
#include "blcompanydialog.h"
#include "blpostgresqlclient.h"
#include "blconfiguration.h"
#include "blclickablelabel.h"


/// Este es el archivo en el que se almacenan las mui_empresas que existen.
/// Es un archivo separado por tabuladores, que se suele alojar en
/// el directorio '.bulmages/' del home del usuario (perfil del usuario en Windows).
#define LISTEMPRESAS QString("listempresas.lst")
/// N&uacute;mero de columna para el nombre en la lista.
#define ABRE_NOMBRE  0
/// N&uacute;mero de columna que almacena el ejercicio de la empresa.
#define ABRE_ANO     1
/// N&uacute;mero de columna que almacena el nombre de la base de datos de la empresa.
#define ABRE_ARCHIVO 2
/// N&uacute;mero de columna que almacena el tipo de datos al que hace referencia la l&iacute;nea
/// (BulmaCont o BulmaFact).
#define ABRE_TIPO    3


///
/**
\param dbName
**/
void BlCompanyDialog::setDbName(QString dbName)
{
  BL_FUNC_DEBUG

  m_empresabd = dbName;
}


///
/**
\param companyName
**/
void BlCompanyDialog::setCompanyName(QString companyName)
{
  BL_FUNC_DEBUG

  m_nombreempresa = companyName;
}


///
/**
\param pTipoEmpresa
**/
void BlCompanyDialog::setCompanyType(QString companyType)
{
  BL_FUNC_DEBUG

  m_tipoempresa = companyType;
}


///
/**
\return
**/
bool BlCompanyDialog::isDestroyMode()
{
  BL_FUNC_DEBUG

  return m_modo;
}


///
/**
\param pModo
**/
void BlCompanyDialog::setDestroyMode(bool destroyMode)
{
  BL_FUNC_DEBUG

  m_modo = destroyMode;
}


///
/**
\return
**/
QString BlCompanyDialog::dbName()
{
  BL_FUNC_DEBUG

  return m_empresabd;
}


///
/**
\return
**/
QString BlCompanyDialog::companyName()
{
  BL_FUNC_DEBUG

  return m_nombreempresa;
}


///
/**
\return
**/
QString BlCompanyDialog::companyType()
{
  BL_FUNC_DEBUG

  return m_tipoempresa;
}


///
/**
**/
void BlCompanyDialog::on_mui_aceptar_clicked()
{
  BL_FUNC_DEBUG

  accept();
}


///
/**
**/
void BlCompanyDialog::on_mui_cancelar_clicked()
{
  BL_FUNC_DEBUG

  if (!isDestroyMode())
  {
    exit(1);

  } else
  {
    done(1);
  } // end if
}


///
/**
\return
**/
bool BlCompanyDialog::close(bool)
{
  BL_FUNC_DEBUG

  on_mui_cancelar_clicked();

  return true;
}


/// Inicia las variables m_tipo y m_tipoempresa y carga el archivo para
/// hacer la presentaci&oacute;n.
/// Llama a la funci&oacute;n 'loadFile'.
/// @param parent La ventana que hace la llamada.
/// @param tipo Cadena de texto que indica si es contabilidad o facturaci&oacute;n.
/// @param name Nombre de la ventana.
/// @param modal Indica si la ventana debe comportar de forma 'modal' o no. (por defecto s&iacute;).
BlCompanyDialog::BlCompanyDialog(QStringList tipo, QWidget * parent, const char * name, bool modal) : QDialog{parent}
{
  BL_FUNC_DEBUG

  setModal(modal);
  this->setWindowTitle(name);
  setupUi(this);

  QString style{"*{"
                "    background: #ffffff;"
                "    color: #000000;"
                "}"
                "QTableWidget::item"
                "{"
                "    background: #d6d6ff;"
                "    color: #000000;"
                "}"
                "QTableWidget::item:alternate"
                "{"
                "    background: #e9e8ff;"
                "    color: #000000;"
                "}"
                "QTableWidget::item:selected"
                "{"
                "    background: #918cf3;"
                "    color: #ffffff;"
                "}"
                "QTableWidget::item:selected:active"
                "{"
                "    background: #3f39af;"
                "    color: #ffffff;"
                "}"
                "QTableWidget::item:selected:active:focus"
                "{"
                "    background: #111be8;"
                "    color: #ffffff;"
                "}"};

  setStyleSheet(style);

  blCenterOnScreen(this);

  mui_empresas->installEventFilter(this);
  QObject::connect(mui_empresas, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
                   this, SLOT(mui_empresasdobleclick()));

  m_tipo = tipo;
  setCompanyType(QString(""));
  setDestroyMode(false);

  /// Si el m_modo es false (salir del programa si se cancela la ventana) entonces se pone
  /// un título adecuado al botón.
  if (!isDestroyMode()) mui_cancelar->setText(_("&Cierra el programa"));

  loadFile();

  /// Seleccionar la primera empresa de la lista inicialmente, de este modo,
  /// si sólo hay una empresa o se quiere entrar a la primera bastará con pulsar la tecla INTRO.
  mui_empresas->selectRow(0);

  QString lastDatabase = lastUsedDatabase();

  if (!lastDatabase.isEmpty())
  {
    /// Recorre la lista de empresas
    for (int i{0}; i < mui_empresas->rowCount(); i++)
    {
      /// Si encuentra la empresa pre usada entonces la selecciona.
      QString itemDatabaseName = mui_empresas->item(i, 2)->text();

      if (lastDatabase == itemDatabaseName)
      {
        mui_empresas->selectRow(i);
      } // end if
    } // end for
  } // end if

  if (isNormalMode())
  {
    mui_checkNormal->setChecked(true);

  } else
  {
    mui_checkExtended->setChecked(true);
  } // end if

  connect(mui_logoBges, SIGNAL(clicked()), this, SLOT(visitBulmagesPlus()));
}


///
/**
**/
BlCompanyDialog::~BlCompanyDialog()
{
  BL_FUNC_DEBUG
}


///
/**
**/
void BlCompanyDialog::visitBulmagesPlus()
{
  BL_FUNC_DEBUG

  blWebBrowser("https://bulmagesplus.com/");
}


/// Inserta una compa&ntilde;ia en el 'QList' mui_empresas definido en el di&aacute;logo.
/// Crea un objeto QListViewItem para la QListView 'mui_empresas' y rellena sus
/// columnas con los datos pasados al m&eacute;todo.
/// @param nombre Nombre de la empresa.
/// @param ano Ejercicio de la empresa (aunque pueden ser varios).
/// @param archivo Nombre de la base de datos.
/// @param tipo Tipo de base de datos (BulmaCont o BulmaFact).
void BlCompanyDialog::insertCompany(QString nombre, QString ano, QString archivo, QString tipo)
{
  BL_FUNC_DEBUG

  if (nombre.isEmpty() || nombre == QString("--"))
  {
    nombre = _("<Su empresa>");
  } // end if

  mui_empresas->insertRow(mui_empresas->rowCount());

  QTableWidgetItem *nuevoItemNombre = new QTableWidgetItem(nombre);
  mui_empresas->setItem(mui_empresas->rowCount() - 1, 0, nuevoItemNombre);

  QTableWidgetItem *nuevoItemAno = new QTableWidgetItem (ano);
  mui_empresas->setItem(mui_empresas->rowCount() - 1, 1, nuevoItemAno);

  QTableWidgetItem *nuevoItemArchivo = new QTableWidgetItem (archivo);
  mui_empresas->setItem(mui_empresas->rowCount() - 1, 2, nuevoItemArchivo);

  QTableWidgetItem *nuevoItemTipo = new QTableWidgetItem (tipo);
  mui_empresas->setItem(mui_empresas->rowCount() - 1, 3, nuevoItemTipo);
}


/// Se ha pulsado sobre el bot&oacute;n de aceptar con lo que iniciamos la variables y
/// cerramos esta ventana ya que ha cumplido con su cometido.
/**
**/
void BlCompanyDialog::accept()
{
  BL_FUNC_DEBUG

  /// Comprueba que la fila está seleccionada.
  /// (Sólo necesitamos comprobar una celda de la fila) y luego
  /// recupera la información de la fila actual.
  if ((mui_empresas->currentRow() != -1) &&
      (mui_empresas->isItemSelected(mui_empresas->item(mui_empresas->currentRow(), 0))))
  {
    /// Columna 0: Nombre
    /// Columna 1: anyo.
    /// Columna 2: Archivo.
    /// Columna 3: Tipo.
    QTableWidgetItem *itemfilaactual0 = mui_empresas->item(mui_empresas->currentRow(), 0);
    QTableWidgetItem *itemfilaactual2 = mui_empresas->item(mui_empresas->currentRow(), 2);
    QTableWidgetItem *itemfilaactual3 = mui_empresas->item(mui_empresas->currentRow(), 3);

    QString dbName = itemfilaactual2->text();

    setCompanyName(itemfilaactual0->text());
    setDbName(dbName);
    setCompanyType(itemfilaactual3->text());

    saveLastUsedDatabase(dbName);

    /// Comprueba modo seleccionado (normal o extendido).
    if (mui_checkNormal->isChecked())
    {
      saveNormalMode();
      g_confpr->setValue(CONF_PREVIEW, "FALSE");

    } else
    {
      saveExtendedMode();
      g_confpr->setValue(CONF_PREVIEW, "TRUE");
    } // end if

    done(0);

  } else
  {
    blMsgInfo(_("Para entrar, antes tiene que seleccionar una empresa."));
  } // end if
}


/// Evento que se dispara al hacer doble click sobre el listado de mui_empresas.
/**
**/
void BlCompanyDialog::mui_empresasdobleclick()
{
  BL_FUNC_DEBUG

  accept();
}


/// Carga del archivo de mui_empresas las mui_empresas disponibles.
/**
\return
**/
void BlCompanyDialog::loadFile()
{
  BL_FUNC_DEBUG

  mui_empresas->setSortingEnabled(false);
  QString dir1{g_confpr->value(CONF_DIR_USER) + LISTEMPRESAS};

  /// Si el archivo no existe hacemos una recarga.
  /// Comprobamos la existencia del directorio personalizado. Y si no...
  if (!QFile::exists(dir1))
  {
    /// Hacemos una recarga de mui_empresas porque sabemos a que ha cambiado el listado.
    on_mui_actualizar_clicked();
  } // end if

  QFile file{dir1};
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

  preparamui_empresas();
  QTextStream filestr{&file};
  filestr.setCodec("UTF-8");
  QString lineatexto, nombre, ano, nombd, tipo;

  while (!filestr.atEnd())
  {
    /// Lee una línea completa del archivo.
    lineatexto = filestr.readLine();
    /// Separa los diferentes campos de la línea de texto y la asigna a las variables.
    QStringList listacampos{lineatexto.split("\t")};
    if (listacampos.count() == 4)
    {
      nombre = listacampos[0];
      ano = listacampos[1];
      nombd = listacampos[2];
      tipo = listacampos[3];

      /// Recorre la lista de tipos
      if (m_tipo.isEmpty())
      {
        insertCompany(nombre, ano, nombd, tipo);

      } else
      {
        for (int i{0}; i < m_tipo.size(); ++i)
        {
          if (tipo == m_tipo.at(i))
          {
            insertCompany(nombre, ano, nombd, tipo);
          } // end if
        } // end for
      } // end if

    } else
    {
        /// TODO Si listacampos.count() != 4 tratar error
    } // end if
  } // end while

  file.close();
  mui_empresas->setSortingEnabled(true);
}


/// Guarda en el archivo de mui_empresas las mui_empresas disponibles
/// También actualiza el listado de mui_empresas visibles.
/**
\return
**/
void BlCompanyDialog::saveFile()
{
  BL_FUNC_DEBUG

  QString dir1{g_confpr->value(CONF_DIR_USER) + LISTEMPRESAS};

  QFile file{dir1};
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

  /// Deshabilitamos las alertas para que no aparezcan avisos con bases de datos
  /// que no son del sistema.
  QString alertas{g_confpr->value(CONF_ALERTAS_DB)};
  g_confpr->setValue(CONF_ALERTAS_DB, "No");

  /// Nos conectamos a la base de datos 'template1' para obtener un listado de todas
  /// las bases de datos existentes.
  BlPostgreSqlClient *db{new BlPostgreSqlClient{}};
  if (db->inicializa(QString("template1")))
    if (db->inicializa(QString("bulmafact")))
      if (db->inicializa(QString("bulmacont")))
        if (db->inicializa(QString("bulmatpv")))
          return;

  BlDbRecordSet *curs{db->loadQuery(QString{"SELECT datname FROM pg_database"})};

  /// Preparamos el listado.
  preparamui_empresas();

  /// Para cada base de datos nos intentamos conectar y deducir de qué tipo es.
  while (!curs->eof())
  {
    findCompanyDatabase(curs->value("datname"), &file);
    curs->nextRecord();
  } // end while

  delete curs;
  delete db;

  g_confpr->setValue(CONF_ALERTAS_DB, alertas);
  file.close();
}


///
/**
\param empresa
\param file
\return
**/
void BlCompanyDialog::findCompanyDatabase(QString empresa, QFile *file)
{
  BL_FUNC_DEBUG

  QTextStream filestr{file};
  BlPostgreSqlClient *db1{nullptr};
  QString nombre;
  QString nomdb;
  QString ano;
  QString tipo;

  mui_empresas->setSortingEnabled(false);
  if (!empresa.startsWith(QString{"template"}))
  {
    db1 = new BlPostgreSqlClient{};
    db1->inicializa(empresa);

    try
    {
      BlDbRecordSet *cursa{db1->loadQuery(QString{"SELECT * FROM pg_tables WHERE tablename = 'configuracion'"})};
      if (!cursa) return;

      if (cursa->eof())
      {
        delete cursa;
        return;
      } // end if

      delete cursa;

      cursa = db1->loadQuery(QString{"SELECT * FROM configuracion WHERE nombre = 'Tipo'"});
      if (!cursa) return;

      if (!cursa->eof())
      {
        tipo = cursa->value(QString{"valor"});
        nomdb = empresa;
        delete cursa;

      } else
      {
        delete cursa;
        return;
      } // end if

      cursa = db1->loadQuery(QString{"SELECT * FROM configuracion WHERE nombre = 'NombreEmpresa'"});
      if (!cursa) return;

      if (!cursa->eof())
      {
        nombre = cursa->value(QString{"valor"});
        delete cursa;

      } else
      {
        delete cursa;
        return;
      } // end if

      cursa = db1->loadQuery(QString{"SELECT * FROM configuracion WHERE nombre = 'Ejercicio'"});
      if (cursa)
      {
        if (!cursa->eof())
        {
          ano = cursa->value(QString{"valor"});
        } // end if

        delete cursa;
      } // end if
    } catch ( ... )
    {
    } // end try

    if (nomdb != QString{""})
    {
      /// Recorre la lista de tipos.
      if (m_tipo.isEmpty())
      {
        insertCompany(nombre, ano, nomdb, tipo);
      } else
      {
        for (int i{0}; i < m_tipo.size(); ++i)
        {
          if (tipo == m_tipo.at(i))
          {
            insertCompany(nombre, ano, nomdb, tipo);
          } // end if
        } // end for
      } // end if

      /// Independientemente de si deben mostrarse o no hay que guardarlas
      /// en el archivo.
      filestr << nombre.toUtf8() << "\t";
      filestr << ano.toUtf8() << "\t";
      filestr << nomdb.toUtf8() << "\t";
      filestr << tipo.toUtf8() << endl;
      nomdb = QString{""};
    } // end if

    delete db1;
  } // end if

  mui_empresas->setSortingEnabled(true);
}


/// Recarga la lista de mui_empresas haciendo las gestiones necesarias con el motor de
/// base de datos. Al mismo tiempo guarda el archivo de bases de datos en el
/// archivo 'LISTEMPRESAS'.
/**
**/
void BlCompanyDialog::on_mui_actualizar_clicked()
{
  BL_FUNC_DEBUG

  saveFile();
}


///
/**
**/
void BlCompanyDialog::preparamui_empresas()
{
  BL_FUNC_DEBUG

  mui_empresas->clear();
  mui_empresas->setRowCount(0);
  mui_empresas->setColumnCount(4);
  QStringList nombrecolumnas;
  nombrecolumnas << _("Nombre") << _("Anyo") << _("Archivo") << _("Tipo");
  mui_empresas->setHorizontalHeaderLabels(nombrecolumnas);
  mui_empresas->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

  mui_empresas->verticalHeader()->hide();
  mui_empresas->setShowGrid(false);

  /// Si es el modo de facturación ocultamos la fecha.
  if (m_tipo.contains("BulmaFact", Qt::CaseInsensitive)) mui_empresas->hideColumn(ABRE_ANO);
}


///
/**
\param  object
\param event
\return
**/
bool BlCompanyDialog::eventFilter(QObject * object, QEvent * event)
{
  BL_FUNC_DEBUG

  if (object == mui_empresas)
  {
    if (event->type() == QEvent::KeyPress)
    {
      QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
      switch (keyEvent->key())
      {
        case Qt::Key_Enter:
        case Qt::Key_Return:
          /// Se comprueba que se ha seleccionado una entrada en la lista mui_empresas.
          if (mui_empresas->currentRow() != -1)
          {
            /// Entramos en el programa.
            this->accept();
          } // end if

          break;
      } // end switch
    } // end if

    return false;
  } // end if

  return QDialog::eventFilter(object, event);
}


///
/**
**/
void BlCompanyDialog::on_mui_checkNormal_toggled(bool toggled)
{
  BL_FUNC_DEBUG

  mui_checkExtended->setChecked(false);
  g_confpr->setValue(CONF_PREVIEW, QString{"FALSE"});
}


///
/**
**/
void BlCompanyDialog::on_mui_checkExtended_toggled(bool toggled)
{
  BL_FUNC_DEBUG

  mui_checkNormal->setChecked(false);
  g_confpr->setValue(CONF_PREVIEW, QString{"TRUE"});
}


bool BlCompanyDialog::isNormalMode()
{
  BL_FUNC_DEBUG

  bool result{true};

  QString dir1{g_confpr->value( CONF_DIR_USER ) + QString{"extended_mode.txt"}};

  QFile file{dir1};
  if (file.exists())
  {
    result = false;
  } // end if

  return result;
}


QString BlCompanyDialog::lastUsedDatabase()
{
  BL_FUNC_DEBUG

  QString result{""};
  QString dir1{g_confpr->value( CONF_DIR_USER ) + QString{"last_used_database.txt"}};

  if (QFile::exists(dir1))
  {
    QFile file{dir1};
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      result = QString(file.readLine()).trimmed();
      file.close();
    } // end if
  } // end if

  return result;
}


bool BlCompanyDialog::saveLastUsedDatabase(QString dbName)
{
  BL_FUNC_DEBUG

  bool result{false};
  QString dir1{g_confpr->value( CONF_DIR_USER ) + QString{"last_used_database.txt"}};

  QFile file{dir1};
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QTextStream stream(&file);
    stream << dbName << "\n";
    file.close();
    result = true;
  } // end if

  return result;
}


bool BlCompanyDialog::saveNormalMode()
{
  BL_FUNC_DEBUG

  bool result{false};

  /// Es la situación por defecto. Basta con que no exista el archivo "extended_mode.txt".
  QString dir1{g_confpr->value( CONF_DIR_USER ) + QString{"extended_mode.txt"}};

  QFile file{dir1};
  if (file.exists())
  {
    result = file.remove();
  } // end if

  return result;
}


bool BlCompanyDialog::saveExtendedMode()
{
  BL_FUNC_DEBUG

  bool result{false};
  /// Solo es necesario crear el archivo para que se detecte como modo extendido.
  QString dir1{g_confpr->value( CONF_DIR_USER ) + QString{"extended_mode.txt"}};

  QFile file{dir1};
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    file.close();
    result = true;
  } // end if

  return result;
}

