/***************************************************************************
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

#include <stdio.h>
#include <QTextBrowser>
#include <QPropertyAnimation>
#include <QProcess>
#include <QDir>
#include <QCryptographicHash>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include "blconfiguration.h"
#include "blconfig.h"
#include "blupdate.h"
#include "../../bulmagesplus_version.h"


BlUpdate::BlUpdate(BlMainWindow * parent)
{
  BL_FUNC_DEBUG

  /// TODO https
  m_urlUpdateServer = "http://update.bulmagesplus.com/index.php";
// DEBUG/TESTING::
//    m_urlUpdateServer = "http://correcaminos/bulmagesplus.com/server/index.php";

  m_replyError = QNetworkReply::NoError;

  m_programRunningName = "";
  m_programRunningVersion = QString(BUILD_VERSION);

  m_firstExecution = true;
  m_parent = parent;
  m_dockWidget = 0;

  m_dockWidth = 200;
  m_dockHeight = 100;

  m_isUserRegistered = false;
  m_userEmail = "";
  m_tokenA = "";
  m_tokenB = "";
  m_eliteUntil = "";

  if (m_parent)
  {
    m_timer1 = new QTimer{};
    m_timer1->setInterval(1000);
    m_timer1->start();
    connect (m_timer1, SIGNAL(timeout()), this, SLOT(checkWindowVisible()));

    connect (m_parent, SIGNAL(resizeEventCalled()), this, SLOT(resizeDockWidget()));
  } // end if
}


BlUpdate::~BlUpdate()
{
  BL_FUNC_DEBUG
}


QString BlUpdate::encode(QString dato, QString clave)
{
  BL_FUNC_DEBUG

  /// Quita los guiones (-)
  /// Mira que la longitud sea par.
  /// Divide la cadena en parejas.
  /// Hace lo mismo tanto con la clave como con el dato.
  dato = dato.replace("-", "").trimmed();
  clave = clave.replace("-", "").trimmed();

  QString salida{""};
  if ((dato.size() == 32) && (dato.size() == clave.size()))
  {
    /// Divide
    for (int i{0}; i < dato.size(); i = i + 2)
    {
      bool ok;
      uint d1{dato.mid(i, 2).toUInt(&ok, 16)};
      uint d2{clave.mid(i, 2).toUInt(&ok, 16)};

      QString hexadecimal{""};
      hexadecimal.setNum(d1 ^ d2, 16);
      hexadecimal = hexadecimal.rightJustified(2, '0');

      salida.append(hexadecimal);
    } // end for

    salida.insert(8, "-");
    salida.insert(13, "-");
    salida.insert(18, "-");
    salida.insert(23, "-");
  } // end if

  return salida;
}


bool BlUpdate::isUserRegistered()
{
  BL_FUNC_DEBUG

  QFile userConfFile;

  /// Definimos los directorios donde buscar primero.
  QDir dirGlobalConf(CONFIG_DIR_CONFIG);

  if (!dirGlobalConf.exists())
  {
    m_isUserRegistered = false;

  } else
  {
    /// Si el archivo no existe es BASIC directamente.
    if (userConfFile.exists(QString(CONFIG_DIR_CONFIG) + "user.conf"))
    {
      QFile arch(QString(CONFIG_DIR_CONFIG) + "user.conf");

      if (arch.open(QIODevice::ReadOnly))
      {
        QTextStream in{&arch};
        in.setCodec("UTF-8");
        unsigned row{0};

        while (!in.atEnd())
        {
          QString cad{in.readLine().trimmed()};
          if (cad.startsWith("email: "))
          {
            /// email
            if (cad.size() > 7)
            {
              m_userEmail = cad.mid(7);
            } // end if
          } // end if

          if (cad.startsWith("token_a: "))
          {
            /// tokenA
            if (cad.size() > 9)
            {
              m_tokenA = cad.mid(9);
            } // end if
          } // end if

          if (cad.startsWith("token_b: "))
          {
            /// tokenB
            if (cad.size() > 9)
            {
              m_tokenB = cad.mid(9);
            } // end if
          } // end if

          if (cad.startsWith("elite_until: "))
          {
            /// eliteUntil
            if (cad.size() > 13)
            {
              m_eliteUntil = cad.mid(13);
            } // end if
          } // end if

          row++;
        } // end while

        if (!m_tokenA.isEmpty() && !m_tokenB.isEmpty() && !m_userEmail.isEmpty())
        {
          /// Verifica
          QString md5email{QString(QCryptographicHash::hash((m_userEmail.toLatin1()), QCryptographicHash::Md5).toHex())};
          QString s1{encode(m_tokenA, md5email)};
          QString s2{encode (s1, PRODUCT_AUTH_ID)};

          /// compara datos
          if ((s2 != "") && (s2 == m_tokenB))
          {
            m_isUserRegistered = true;
          } // end if
        } // end if
      } // end if
    } // end if
  } // end if

  return m_isUserRegistered;
}


QString BlUpdate::userEmail() const
{
  BL_FUNC_DEBUG

  return m_userEmail;
}


QString BlUpdate::tokenA() const
{
  BL_FUNC_DEBUG

  return m_tokenA;
}


QString BlUpdate::tokenB() const
{
  BL_FUNC_DEBUG

  return m_tokenB;
}


void BlUpdate::checkWindowVisible()
{
  BL_FUNC_DEBUG

  /// Nos aseguramos que la ventana es visible en pantalla y es la primera ejecución
  /// antes de iniciar el proceso.
  if (m_parent->testAttribute(Qt::WA_Mapped) && (m_firstExecution))
  {
    m_timer1->stop();
    m_firstExecution = false;

    /// Comprueba actualizaciones disponibles.
    m_timer2 = new QTimer{};
    connect(m_timer2, SIGNAL(timeout()), this, SLOT(checkUpdates()));
    m_timer2->start(5000);
  } // end if
}


/* Comprueba si hay actualizaciones e informa al usuario.
 */
void BlUpdate::checkUpdates()
{
  BL_FUNC_DEBUG

  m_timer2->stop();
  QString url{""};

  if (m_isUserRegistered)
  {
    url = m_urlUpdateServer + "?" + "tokena=" + m_tokenA + "&" + "product_auth_id=" + QString(PRODUCT_AUTH_ID) + "&" + "os=" + QString(OPERATING_SYSTEM) + "&" + "arch=" + QString(ARCH) + "&" + "build_version=" + QString(BUILD_VERSION) + "&" + "caller=" + m_programRunningName + "&" + "last_installed_update=" + lastInstalledUpdate();

  } else
  {
    url = m_urlUpdateServer + "?" + "product_auth_id=" + QString(PRODUCT_AUTH_ID) + "&" + "os=" + QString(OPERATING_SYSTEM) + "&" + "arch=" + QString(ARCH) + "&" + "build_version=" + QString(BUILD_VERSION) + "&" + "caller=" + m_programRunningName + "&" + "last_installed_update=" + lastInstalledUpdate();
  } // end if

  BlDebug::blDebug("BlUpdate::checkUpdates() URL: " + url);

  QString lastDownloadData{QString(download(url))};

  if ((error() == QNetworkReply::NoError) && (g_confpr->value(CONF_BGP_DISABLE_UPDATE) != "TRUE"))
  {
    /// Debug
    //fprintf(stderr, lastDownloadData.toLatin1());

    /// Solo nos interesa el caso en que hay que actualizar.
    if (lastDownloadData.contains("<generalstatus>updates_available</generalstatus>", Qt::CaseInsensitive))
    {
      /// Existen actualizaciones pendientes. Muestra mensaje.
      m_dockWidget = new QDockWidget{"BulmagesPlus Updater", m_parent, Qt::Widget};
      resizeDockWidget();
      m_dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetClosable);
      m_dockWidget->setStyleSheet("background: yellow;");

      QTextBrowser *tb = new QTextBrowser{};
      tb->setFrameShadow(QFrame::Plain);
      tb->setFrameShape(QFrame::NoFrame);
      /// No sigue los enlaces de forma automatica.
      tb->setOpenLinks(false);
      connect(tb, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(anchorClicked (const QUrl &)));
      m_dockWidget->setWidget(tb);
      tb->setHtml("Existen actualizaciones.<br><a href=\"execute_updater\">Pulse aqui.</a>");
      tb->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

      m_dockWidget->show();

      QPropertyAnimation *animation = new QPropertyAnimation{m_dockWidget, "geometry"};
      animation->setDuration(800);
      animation->setStartValue(QRect(m_parent->width() - m_dockWidth, m_parent->height() - m_parent->statusBar()->height(), m_dockWidth, 0));
      animation->setEndValue(QRect(m_parent->width() - m_dockWidth, m_parent->height() - m_parent->statusBar()->height() - m_dockHeight, m_dockWidth, m_dockHeight));
      connect(animation, SIGNAL(valueChanged(const QVariant &)), this, SLOT(resizeDockWidget()));

      animation->start();
    } // end if
  } // end if
}


void BlUpdate::resizeDockWidget()
{
  BL_FUNC_DEBUG

  if (m_dockWidget != 0)
  {
    m_dockWidget->setGeometry(m_parent->width() - m_dockWidget->width(), m_parent->height() - m_parent->statusBar()->height() - m_dockWidget->height(), m_dockWidth, m_dockHeight);
  } // end if
}


void BlUpdate::anchorClicked(const QUrl & link)
{
  BL_FUNC_DEBUG

  if (link.toString() == QString{"execute_updater"})
  {
    m_dockWidget->hide();
    executeUpdater();
  } // end if
}


void BlUpdate::executeUpdater()
{
    BL_FUNC_DEBUG

    /// Ejecuta Updater.
    QProcess process{this};
    QList<QString> checkExistingFiles;
    QString appDirPath {g_confpr->value(CONF_EJECUTABLES)};

#ifdef Q_OS_WIN32
    QString path{appDirPath + QString{"\\updater.exe"}};
    QString path2{QString("\"") + path + QString("\"")};
    QString command{path2 + QString(" --calledfromprogram")};
    checkExistingFiles.append(appDirPath + QString{"\\updater.exe"});
    bool forceNewConsole{false};
    blStartDetachedProcess(path, command, checkExistingFiles, forceNewConsole);
#else
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if (env.value("PATH").isEmpty())
    {
      env.insert("PATH", appDirPath);
    }
    else
    {
      env.insert("PATH", appDirPath + QString(":") + env.value("PATH"));
    } // end if
    env.insert("TERMINFO", appDirPath + QString("../usr/local/share/terminfo"));
    env.insert("TERM", QString("xterm"));
    env.insert("QT_XKB_CONFIG_ROOT", appDirPath + QString("../usr/local/share/xcb"));
    env.insert("QT_PLUGIN_PATH", appDirPath + QString("../../libs/plugins"));
    env.insert("BGP_PROGRAM_BIN_PATH", appDirPath);
    if (env.value("LD_LIBRARY_PATH").isEmpty())
    {
      env.insert("LD_LIBRARY_PATH", appDirPath + QString("../../libs"));
    }
    else
    {
      env.insert("LD_LIBRARY_PATH", appDirPath + QString("../../libs:") + env.value("LD_LIBRARY_PATH"));
    } // end if

    QString loaderLinux {blGetEnv("BGP_LD_LINUX")};
    if (!loaderLinux.isEmpty())
    {
        env.insert("BGP_LD_LINUX", loaderLinux);
        loaderLinux += QString(" ");
    } // end if

    QString command{loaderLinux + appDirPath + QString{"/updater.bin --calledfromprogram"}};
    process.setWorkingDirectory(appDirPath);
    process.setProcessEnvironment(env);
    process.startDetached(command);
#endif
}


void BlUpdate::setProgramRunningName(QString name)
{
  BL_FUNC_DEBUG

  m_programRunningName = name;
}


/// Necesario para descargar.

void BlUpdate::registerError(QNetworkReply::NetworkError code)
{
  BL_FUNC_DEBUG

  BlDebug::blDebug("Network ERROR: " + QString::number(code));

  m_replyError = code;
}


QNetworkReply::NetworkError BlUpdate::error()
{
  BL_FUNC_DEBUG

  return m_replyError;
}


QByteArray BlUpdate::download(const QString & url)
{
  BL_FUNC_DEBUG

  QByteArray result;
  m_networkManager = new QNetworkAccessManager{this};
  QNetworkRequest networkRequest{url};
  m_reply = m_networkManager->get(networkRequest);

  QTimer timer;
  timer.start(0);
  connect(&timer, SIGNAL(timeout()), this, SLOT(processAllPendingEvents()));

  m_replyError = QNetworkReply::NoError;

  QEventLoop loop;
  connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(registerError(QNetworkReply::NetworkError)));
  connect(m_reply, SIGNAL(finished()), &loop, SLOT(quit()));
  loop.exec();

  if (m_replyError == QNetworkReply::NoError)
  {
    result = m_reply->readAll();
  } // end if

  m_reply->deleteLater();
  timer.stop();
  delete m_networkManager;

  return result;
}


void BlUpdate::processAllPendingEvents()
{
  BL_FUNC_DEBUG

  QCoreApplication::processEvents();
  QCoreApplication::flush();
}


QString BlUpdate::eliteUntil() const
{
  BL_FUNC_DEBUG

  return m_eliteUntil;
}


QString BlUpdate::lastInstalledUpdate()
{
  BL_FUNC_DEBUG

  QString result{""};

  QFile updatesRegistryFile;

  /// Definimos los directorios donde buscar primero.
  //QString CONFIG_DIR_CONFIG = "../etc/bulmages";

  QDir dirGlobalConf{CONFIG_DIR_CONFIG};

  if (dirGlobalConf.exists())
  {
    if (updatesRegistryFile.exists(QString(CONFIG_DIR_CONFIG) + QString("/updates_registry.conf")))
    {
      QFile arch(QString(CONFIG_DIR_CONFIG) + QString("/updates_registry.conf"));

      if (arch.open(QIODevice::ReadOnly))
      {
        QTextStream in{&arch};
        in.setCodec("UTF-8");
        unsigned row{0};

        QList<QString> listOfUpdates;
        bool allOk{true};

        while (!in.atEnd())
        {
          QString line{in.readLine().trimmed()};

          if (line.size() <= 54) // up_20180412000000_29302d28-4199-4f33-adda-7227068982c0
          {
            listOfUpdates.append(line);

          } else {
            /// TODO Error: File is corrupted
            allOk = false;
            break;
          } // end if

          row++;
        } // end while

        if (allOk & (listOfUpdates.count() > 0))
        {
          qSort(listOfUpdates.begin(), listOfUpdates.end());

          result = listOfUpdates.last();
        } // end if
      } // end if
    } // end if
  } // end if

  return result;
}

