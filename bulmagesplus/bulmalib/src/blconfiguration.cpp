/***************************************************************************
 *   Copyright (C) 2003 by Tomeu Borrás Riera.                             *
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

/// Este fichero contiene la implementación de la clase 'BlConfiguration' que se encarga
/// de recoger todos los parámetros de configuración de la aplicación y los centraliza
/// en un único objeto que luego es instanciado como objeto global 'confpr'
/// De este modo cualquier objeto de la aplicación puede consultar
/// (sin demasiados problemas cuál es la configuración que le corresponde).

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#ifndef Q_OS_WIN32
#include <unistd.h>
#endif
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QString>
#include "blfunctions.h"
#include "blapplication.h"
#include "blpostgresqlclient.h"
#include "blconfig.h"
#include "blconfiguration.h"
#include "blcompanydialog.h"


/// El objeto global g_confpr es la instancia de la clase 'BlConfiguration'. Este objeto
/// puede ser accedido desde todas las clases de la aplicación.
BlConfiguration BL_EXPORT *g_confpr = NULL;


void initConfiguration(QString config, QString confReplaceString)
{
  /// Hay funciones reentrantes que comprueban el valor de g_confpr y pueden causar segfault mientras este se está inicializando.
  /// Creando una variable temporal y asignándola después evitamos las reentradas con la clase inicializada a medias.
  BlConfiguration *confpr{new BlConfiguration(config, confReplaceString)};
  /// Inicializa el sistema de traducciones 'gettext'.
  setlocale(LC_ALL, "");
  blBindTextDomain("bulmalib", confpr->value(CONF_DIR_TRADUCCION).toLatin1().constData());
  blTextDomain("bulmalib");
  g_confpr = confpr;
}


/// Constructor de la clase que hace directamente la lectura de los posibles
/// archivos que pueden tener información de configuración. Como parámetro
/// reciben el nombre del programa y buscan en este orden su archivo de configuración.
///
/// En Linux:
/// 1) /etc/bulmages/ -> Opciones genéricas para todos los usuarios.
/// 2) /home/~/.bulmages/  -> Opciones específicas para cada usuario.
///
/// En Windows:
/// 1) %ProgramFiles%\bulmages\etc\ -> Opciones genéricas para todos los usuarios.
/// 2) %USERPROFILE%\.bulmages\  -> Opciones específicas para cada usuario.
///
/// Las opciones se irán cargando desde las más genéricas a las más específicas.
/// Estas últimas prevalecerán a las genéricas cuando existan.
///
/// Dentro de cada directorio hay las siguientes preferencias a la hora de aplicar
/// las configuraciones:
///
/// 1) bulmages.conf -> archivo para todos los programas del paquete Bulmages.
/// 2) bulmaxxx.conf -> archivo especifico a un solo programa del paquete.
///
/// NOTA: No se puede utilizar BlDebug::blDebug dentro de esta clase porque necesita
/// valores que no se disponen antes de leer el archivo de configuración.


///
/**
\param nombreprograma
**/
BlConfiguration::BlConfiguration(QString nombreprograma, QString confReplaceString)
{
  /// Definimos los directorios donde buscar primero.
  m_dirGlobalConf = CONFIG_DIR_CONFIG;

  QString mensaje;
  QFile genericGlobalConfFile;
  QFile programGlobalConfFile;
  QFile genericLocalConfFile;
  QFile programLocalConfFile;
  QDir dirGlobalConf{m_dirGlobalConf};

#ifndef Q_OS_WIN32
  QString dirusuario{getenv("HOME")};
#else
  QString dirusuario{getenv("UserProfile")};
  dirusuario.replace('\\', '/'); // Para no tener barras de los dos tipos mezclados en la misma ruta, usaremos las normales siempre.
#endif

  m_dirLocalConf = dirusuario + "/.bulmages/";
  m_genericGlobalConfFile = "bulmages.conf";
  m_programGlobalConfFile = nombreprograma + ".conf";
  m_genericLocalConfFile = m_genericGlobalConfFile;
  m_programLocalConfFile = m_programGlobalConfFile;

  /// Establecemos los valores por defecto, que luego la propia configuracion puede sobreescribir.
  setValue(CONF_REPLACE_STRING, confReplaceString);
  setValue(CONF_DIR_USER, m_dirLocalConf);
  setValue(CONF_PRIVILEGIOS_USUARIO, "1");
  setValue(CONF_LOGIN_USER, "");
  setValue(CONF_PASSWORD_USER, "");
  setValue(CONF_TOTAL_DECIMAL_PLACES, "2");

  /// Comprobamos la existencia de los directorios y archivos de configuración.
  /// Directorios y archivos obligatorios (sale si no existe):
  if (!dirGlobalConf.exists())
  {
    mensaje = "--> ERROR: El directorio '" + m_dirGlobalConf + "' no existe. Debe crearlo. <--\n";
    fprintf(stderr, "%s", mensaje.toLatin1().constData());
    exit(-1);

  } else
  {
    if (!genericGlobalConfFile.exists(m_dirGlobalConf + m_genericGlobalConfFile))
    {
      mensaje = "--> ERROR: El archivo '" + m_dirGlobalConf + m_genericGlobalConfFile + "' no existe. Debe crearlo. <--\n";
      fprintf(stderr, "%s", mensaje.toLatin1().constData());

    } else
    {
      /// 1) Leemos la configuracion del archivo generico global.
      mensaje = "--> Leyendo el archivo '" + m_dirGlobalConf + m_genericGlobalConfFile + "'<--\n";
      fprintf(stderr, "%s", mensaje.toLatin1().constData());
      readConfig(m_dirGlobalConf + m_genericGlobalConfFile);
    } // end if
  } // end if

  /// Directorios y archivos opcionales:
  if (!programGlobalConfFile.exists(m_dirGlobalConf + m_programGlobalConfFile))
  {
    mensaje = "--> El archivo '" + m_dirGlobalConf + m_programGlobalConfFile + "' no existe. <--\n";
    fprintf(stderr, "%s", mensaje.toLatin1().constData());

  } else
  {
    /// 2) Leemos la configuracion del archivo especifico global.
    mensaje = "--> El archivo '" + m_dirGlobalConf + m_programGlobalConfFile + "' existe. Se va a leer.<--\n";
    fprintf(stderr, "%s", mensaje.toLatin1().constData());
    readConfig(m_dirGlobalConf + m_programGlobalConfFile);
  } // end if

  /// Comprobamos si el usuario tiene creado su '/home/~/.bulmages/' directorio
  /// de configuración.
  if (!dirGlobalConf.exists(value(CONF_DIR_USER)))
  {
    if (dirGlobalConf.mkpath(value(CONF_DIR_USER)))
    {
      mensaje = "--> Se ha creado el directorio '" + value(CONF_DIR_USER) + "'. <--\n";
      fprintf(stderr, "%s", mensaje.toLatin1().constData());

    } else
    {
      mensaje = "--> ERROR: No se ha podido crear el directorio '" + value(CONF_DIR_USER) + "'. <--\n";
      fprintf(stderr, "%s", mensaje.toLatin1().constData());
      exit(-1);
    } // end if
  } // end if

  if (!genericLocalConfFile.exists(value(CONF_DIR_USER) + m_genericLocalConfFile))
  {
    mensaje = "--> El archivo '" + value(CONF_DIR_USER) + m_genericLocalConfFile + "' no existe. <--\n";
    fprintf(stderr, "%s", mensaje.toLatin1().constData());

  } else
  {
    /// 3) Leemos la configuración del archivo generico local.
    mensaje = "--> El archivo '" + value(CONF_DIR_USER) + m_genericLocalConfFile + "' existe. Se va a leer.<--\n";
    fprintf(stderr, "%s", mensaje.toLatin1().constData());
    readConfig(value(CONF_DIR_USER) + m_genericLocalConfFile );
  } // end if

  if (!programLocalConfFile.exists(value(CONF_DIR_USER ) + m_programLocalConfFile))
  {
    mensaje = "--> El archivo '" + value(CONF_DIR_USER) + m_programLocalConfFile + "' no existe. <--\n";
    fprintf(stderr, "%s", mensaje.toLatin1().constData());

  } else
  {
    /// 4) Leemos la configuración del archivo especifico local.
    mensaje = "--> El archivo '" + value(CONF_DIR_USER) + m_programLocalConfFile + "' existe. Se va a leer.<--\n";
    fprintf(stderr, "%s", mensaje.toLatin1().constData());
    readConfig(value(CONF_DIR_USER) + m_programLocalConfFile);
  } // end if

  fprintf(stderr, "Configuraciones cargadas\n");
}


/// El destructor de la clase no hace nada porque no hay que liberar memoria.
/**
**/
BlConfiguration::~BlConfiguration()
{
}


/// Puesto que la configuración funciona sobre un array y sobre defines en dicho array
/// esta función dado un define devuelve el nombre utilizado.
/// Esta función es útil para hacer la inserción inicial de elementos
/// También es útil para hacer nosotros el guardado de los parámetros.
/**
\param i
\return
**/
QString BlConfiguration::name(int i)
{
  if (i == CONF_BG_APUNTES)
    return "CONF_BG_APUNTES";
  if (i == CONF_FG_APUNTES)
    return "CONF_FG_APUNTES";
  if (i == CONF_BG_APUNTESA)
    return "CONF_BG_APUNTESA";
  if (i == CONF_FG_APUNTESA)
    return "CONF_FG_APUNTESA";
  if (i == CONF_FONTSIZE_APUNTES)
    return "CONF_FONTSIZE_APUNTES";
  if (i == CONF_FONTFAMILY_APUNTES)
    return "CONF_FONTFAMILY_APUNTES";
  if (i == CONF_BG_DIARIO)
    return "CONF_BG_DIARIO";
  if (i == CONF_BG_ALT_DIARIO)
    return "CONF_BG_ALT_DIARIO";
  if (i == CONF_FG_DIARIO1)
    return "CONF_FG_DIARIO1";
  if (i == CONF_FG_DIARIO2)
    return "CONF_FG_DIARIO2";
  if (i == CONF_FONTFAMILY_DIARIO)
    return "CONF_FONTFAMILY_DIARIO";
  if (i == CONF_FONTSIZE_DIARIO)
    return "CONF_FONTSIZE_DIARIO";
  if (i == CONF_CONTRAPARTIDA_DIARIO)
    return "CONF_CONTRAPARTIDA_DIARIO";
  if (i == CONF_CANAL_DIARIO )
    return "CONF_CANAL_DIARIO";
  if (i == CONF_CCOSTE_DIARIO)
    return "CONF_CCOSTE_DIARIO";
  if (i == CONF_BG_EXTRACTO)
    return "CONF_BG_EXTRACTO";
  if (i == CONF_BG_BALANCE)
    return "CONF_BG_BALANCE";
  if (i == CONF_BG_BALANCETREE)
    return "CONF_BG_BALANCETREE";
  if (i == CONF_CONTRAPARTIDA_EXTRACTO)
    return "CONF_CONTRAPARTIDA_EXTRACTO";
  if (i == CONF_CANAL_EXTRACTO)
    return "CONF_CANAL_EXTRACTO";
  if (i == CONF_CCOSTE_EXTRACTO)
    return "CONF_CCOSTE_EXTRACTO";
  if (i == CONF_CONTRAPARTIDA_APUNTES)
    return "CONF_CONTRAPARTIDA_APUNTES";
  if (i == CONF_CANAL_APUNTES)
    return "CONF_CANAL_APUNTES";
  if (i == CONF_CCOSTE_APUNTES)
    return "CONF_CCOSTE_APUNTES";
  if (i == CONF_METABASE)
    return "CONF_METABASE";
  if (i == CONF_SERVIDOR)
    return "CONF_SERVIDOR";
  if (i == CONF_PUERTO)
    return "CONF_PUERTO";
  if (i == CONF_DB_NAME)
    return "CONF_DB_NAME";
  if (i == CONF_SPOOL)
    return "CONF_SPOOL";
  if (i == CONF_FLIP)
    return "CONF_FLIP";
  if (i == CONF_PYTHON)
    return "CONF_PYTHON";
  if (i == CONF_DIR_TMP)
    return "CONF_DIR_TMP";
  if (i == CONF_PDF)
    return "CONF_PDF";
  if (i == CONF_ODS)
    return "CONF_ODS";
  if (i == CONF_SXC)
    return "CONF_SXC";
  if (i == CONF_NAVEGADOR)
    return "CONF_NAVEGADOR";
  if (i == CONF_EMAIL_CLIENT)
    return "CONF_EMAIL_CLIENT";
  if (i == CONF_EDITOR)
    return "CONF_EDITOR";
  if (i == CONF_SPLASH)
    return "CONF_SPLASH";
  if (i == CONF_SPLASH_BULMACONT)
    return "CONF_SPLASH_BULMACONT";
  if (i == CONF_SPLASH_BULMAFACT)
    return "CONF_SPLASH_BULMAFACT";
  if (i == CONF_SPLASH_BULMATPV)
    return "CONF_SPLASH_BULMATPV";
  if (i == CONF_EJECUTABLES)
    return "CONF_EJECUTABLES";
  if (i == CONF_PLANTILLA)
    return "CONF_PLANTILLA";
  if (i == CONF_TRADUCCION)
    return "CONF_TRADUCCION";
  if (i == CONF_DIR_TRADUCCION)
    return "CONF_DIR_TRADUCCION";
  if (i == CONF_ARCHIVO_LOG)
    return "CONF_ARCHIVO_LOG";
  if (i == CONF_DIR_REPORTS)
    return "CONF_DIR_REPORTS";
  if (i == CONF_DIR_KUGAR)
    return "CONF_DIR_KUGAR";
  if (i == CONF_DIR_OPENREPORTS)
    return "CONF_DIR_OPENREPORTS";
  if (i == CONF_DIR_AINTELIGENTES)
    return "CONF_DIR_AINTELIGENTES";
  if (i == CONF_PLUGINS_BULMACONT)
    return "CONF_PLUGINS_BULMACONT";
  if (i == CONF_PLUGINS_BULMAFACT)
    return "CONF_PLUGINS_BULMAFACT";
  if (i == CONF_PLUGINS_BULMAGES)
    return "CONF_PLUGINS_BULMAGES";
  if (i == CONF_PLUGINS_BULMATPV)
    return "CONF_PLUGINS_BULMATPV";
  if (i == CONF_DIR_IMG_ARTICLES)
    return "CONF_DIR_IMG_ARTICLES";
  if (i == CONF_DIR_THUMB_ARTICLES)
    return "CONF_DIR_THUMB_ARTICLES";
  if (i == CONF_DIR_IMG_PERSONAL)
    return "CONF_DIR_IMG_PERSONAL";
  if (i == CONF_DIR_IMG_ALUMNOS)
    return "CONF_DIR_IMG_ALUMNOS";
  if (i == CONF_FONTFAMILY_BULMAGES)
    return "CONF_FONTFAMILY_BULMAGES";
  if (i == CONF_FONTSIZE_BULMAGES)
    return "CONF_FONTSIZE_BULMAGES";
  if (i == CONF_PROGDATA)
    return "CONF_PROGDATA";
  if (i == CONF_NUM_AUT_PRESUP)
    return "CONF_NUM_AUT_PRESUP";
  if (i == CONF_NUM_AUT_FACTURA)
    return "CONF_NUM_AUT_FACTURA";
  if (i == CONF_NUM_AUT_ALBARAN)
    return "CONF_NUM_AUT_ALBARAN";
  if (i == CONF_MOSTRAR_ALMACEN)
    return "CONF_MOSTRAR_ALMACEN";
  if (i == CONF_BG_LISTPRESUPUESTOS)
    return "CONF_BG_LISTPRESUPUESTOS";
  if (i == CONF_BG_LINPRESUPUESTOS)
    return "CONF_BG_LINPRESUPUESTOS";
  if (i == CONF_BG_DESCPRESUPUESTOS)
    return "CONF_BG_DESCPRESUPUESTOS";
  if (i == CONF_BG_LISTPEDIDOSCLIENTE)
    return "CONF_BG_LISTPEDIDOSCLIENTE";
  if (i == CONF_BG_LINPEDIDOSCLIENTE)
    return "CONF_BG_LINPEDIDOSCLIENTE";
  if (i == CONF_BG_DESCPEDIDOSCLIENTE)
    return "CONF_BG_DESCPEDIDOSCLIENTE";
  if (i == CONF_BG_LISTALBARANESCLIENTE )
    return "CONF_BG_LISTALBARANESCLIENTE";
  if (i == CONF_BG_LINALBARANESCLIENTE )
    return "CONF_BG_LINALBARANESCLIENTE";
  if (i == CONF_BG_DESCALBARANESCLIENTE )
    return "CONF_BG_DESCALBARANESCLIENTE";
  if (i == CONF_BG_LISTFACTURASCLIENTE )
    return "CONF_BG_LISTFACTURASCLIENTE";
  if (i == CONF_BG_LINFACTURASCLIENTE )
    return "CONF_BG_LINFACTURASCLIENTE";
  if (i == CONF_BG_DESCFACTURASCLIENTE )
    return "CONF_BG_DESCFACTURASCLIENTE";
  if (i == CONF_BG_LISTARTICULOS )
    return "CONF_BG_LISTARTICULOS";
  if (i == CONF_DIR_CANUALES)
    return "CONF_DIR_CANUALES";
  if (i == CONF_TICKET_PRINTER_FILE)
    return "CONF_TICKET_PRINTER_FILE";
  if (i == CONF_CASHBOX_FILE)
    return "CONF_CASHBOX_FILE";
  if (i == CONF_CASHBOX_OPEN_CODE)
    return "CONF_CASHBOX_OPEN_CODE";
  if (i == CONF_DEBUG)
    return "CONF_DEBUG";
  if (i == CONF_ALERTAS_DB)
    return "CONF_ALERTAS_DB";
  if (i == CONF_IDCLIENTE_DEFECTO)
    return "CONF_IDCLIENTE_DEFECTO";
  if (i == CONF_IDALMACEN_DEFECTO)
    return "CONF_IDALMACEN_DEFECTO";
  if (i == CONF_IDARTICULO_DEFECTO)
    return "CONF_IDARTICULO_DEFECTO";
  if (i == CONF_IDTRABAJADOR_DEFECTO)
    return "CONF_IDTRABAJADOR_DEFECTO";
  if (i == CONF_TPV_VISOR_FILE)
    return "CONF_TPV_VISOR_FILE";
  if (i == CONF_TPV_PRINTER_LOGO)
    return "CONF_TPV_PRINTER_LOGO";
  if (i == CONF_TPV_FULLSCREEN)
    return "CONF_TPV_FULLSCREEN";
  if (i == CONF_REFRESH_LIST)
    return "CONF_REFRESH_LIST";
  if (i == CONF_SHOW_PROGRESS_BAR)
    return "CONF_SHOW_PROGRESS_BAR";
  if (i == CONF_IDFORMA_PAGO_VISA)
    return "CONF_IDFORMA_PAGO_VISA";
  if (i == CONF_IDFORMA_PAGO_CONTADO)
    return "CONF_IDFORMA_PAGO_CONTADO";
  if (i == CONF_IDFORMA_PAGO_DEFECTO)
    return "CONF_IDFORMA_PAGO_DEFECTO";
  if (i == CONF_SERIE_FACTURA_DEFECTO)
    return "CONF_SERIE_FACTURA_DEFECTO";
  if (i == CONF_DIR_PLUGINS)
    return "CONF_DIR_PLUGINS";
  if (i == CONF_ARTICULOS_VARIOS)
    return "CONF_ARTICULOS_VARIOS";
  if (i == CONF_TPV_CELL_WIDTH)
    return "CONF_TPV_CELL_WIDTH";
  if (i == CONF_TPV_CELLS_PER_ROW)
    return "CONF_TPV_CELLS_PER_ROW";
  if (i == CONF_TPV_TEXTOMESA)
    return "CONF_TPV_TEXTOMESA";
  if (i == CONF_FACT_ALT)
    return "CONF_FACT_ALT";
  if (i == CONF_CONT_ALT)
    return "CONF_CONT_ALT";
  if (i == CONF_STYLESHEET)
    return "CONF_STYLESHEET";
  if (i == CONF_BACKGROUND_COLOR)
    return "CONF_BACKGROUND_COLOR";
  if (i == CONF_BACKGROUND_IMAGE)
    return "CONF_BACKGROUND_IMAGE";
  if (i == CONF_CUPS_DEFAULT_PRINTER)
    return "CONF_CUPS_DEFAULT_PRINTER";
  if (i == CONF_BLOCK_WINDOWS)
    return "CONF_BLOCK_WINDOWS";
  if (i == CONF_DIR_CONFIG)
    return "CONF_DIR_CONFIG";
  if (i == CONF_GLOBAL_CONFIG_USER)
    return "CONF_GLOBAL_CONFIG_USER";
  if (i == CONF_GLOBAL_CONFIG_COMPANY)
    return "CONF_GLOBAL_CONFIG_COMPANY";
  if (i == CONF_TPV_SERVER_SYNC_IP)
    return "CONF_TPV_SERVER_SYNC_IP";
  if (i == CONF_CENTER_TEXT_FIELDS)
    return "CONF_CENTER_TEXT_FIELDS";
  if (i == CONF_ASK_BEFORE_EXIT)
    return "CONF_ASK_BEFORE_EXIT";
  if (i == CONF_TPV_BASCULA_FILE)
    return "CONF_TPV_BASCULA_FILE";
  if (i == CONF_MODO_EXPERTO)
    return "CONF_MODO_EXPERTO";
  if (i == CONF_DIR_DEFAULT_CONFS)
    return "CONF_DIR_DEFAULT_CONFS";
  if (i == CONF_RESIZEROWSTOCONTENTS)
    return "CONF_RESIZEROWSTOCONTENTS";
  if (i == CONF_TPV_REIMPRIMIR)
    return "CONF_TPV_REIMPRIMIR";
  if (i == CONF_TPV_CATEGORIES_COLUMNS)
    return "CONF_TPV_CATEGORIES_COLUMNS";
  if (i == CONF_MAIN_WINDOW_TITLE)
    return "CONF_MAIN_WINDOW_TITLE";
  if (i == CONF_NUMCHAR_RELOAD_FILTRO)
    return "CONF_NUMCHAR_RELOAD_FILTRO";
  if (i == CONF_URL_CONTENTS)
    return "CONF_URL_CONTENTS";
  if (i == CONF_REPLACE_STRING)
    return "CONF_REPLACE_STRING";
  if (i == CONF_DIR_USER)
    return "CONF_DIR_USER";
  if (i == CONF_USE_DESCARTICULO)
    return "CONF_USE_DESCARTICULO";
  if (i == CONF_TPV_CATEGORY_TREE)
    return "CONF_TPV_CATEGORY_TREE";
  if (i == CONF_PLUGINBT_MODIFICADORES_EDIT_ARTICLE_DESCRIPTION)
    return "CONF_PLUGINBT_MODIFICADORES_EDIT_ARTICLE_DESCRIPTION";
  if (i == CONF_PLUGINBT_MODIFICADORES_EDIT_MODIFICADORES)
    return "CONF_PLUGINBT_MODIFICADORES_EDIT_MODIFICADORES";
  if (i == CONF_BGP_DISABLE_UPDATE)
    return "CONF_BGP_DISABLE_UPDATE";
  if (i == CONF_DOCUMENTATION)
    return "CONF_DOCUMENTATION";
  if (i == CONF_PREVIEW)
    return "CONF_PREVIEW";
  if (i == CONF_DEFAULT_ARTICLE_LEVEL)
    return "CONF_DEFAULT_ARTICLE_LEVEL";
  if (i == CONF_TOTAL_DECIMAL_PLACES)
    return "CONF_TOTAL_DECIMAL_PLACES";
  if (i == CONF_DIR_IMG_CLIENTS)
    return "CONF_DIR_IMG_CLIENTS";
  if (i == CONF_LAST_USED_DATABASE)
    return "CONF_LAST_USED_DATABASE";
  if (i == CONF_BGP_ENABLE_API_SERVER)
    return "CONF_BGP_ENABLE_API_SERVER";
  if (i == CONF_BGP_API_SERVER_USE_TLS)
    return "CONF_BGP_API_SERVER_USE_TLS";
  if (i == CONF_BGP_API_SERVER_LISTEN_ADDRESSES)
    return "CONF_BGP_API_SERVER_LISTEN_ADDRESSES";
  if (i == CONF_BGP_API_SERVER_PORT)
    return "CONF_BGP_API_SERVER_PORT";
  if (i == CONF_BGP_API_SERVER_TLS_CERT_FILE)
    return "CONF_BGP_API_SERVER_TLS_CERT_FILE";
  if (i == CONF_BGP_API_SERVER_TLS_KEY_FILE)
    return "CONF_BGP_API_SERVER_TLS_KEY_FILE";
  if (i == CONF_BGP_API_SERVER_TLS_CA_FILE)
    return "CONF_BGP_API_SERVER_TLS_CA_FILE";

  return "";
}


/// This method writes the configuration of the system to the home bulmages.conf file
/// Este metodo escribe la configuracion del sistema en el fichero bulmages.conf del
/// 'home' del usuario.
/**
**/
void BlConfiguration::saveConfig()
{
  QString dirusuario{g_confpr->value(CONF_DIR_USER) + QString{"bulmages.conf"}};

  QFile file{dirusuario};
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

  QTextStream filestr{&file};
  for (int i{0}; i < 1000; i++)
  {
    if (!name(i).isEmpty())
    {
      filestr << name(i).toLatin1().data();
      filestr << "   ";
      filestr << value(i).toLatin1().data();
      filestr << endl;
    } // end if
  } // end for

  file.close();
}


/// This method reads the configuration params from a file 'fich' that
/// contains the configuration.
/// Lee la configuracion del fichero de configuracion pasado y rellena la estructura.
/**
\param fich
\return
**/
bool BlConfiguration::readConfig(QString fich)
{
  QFile arch{fich};
  if (arch.open(QIODevice::ReadOnly))
  {
    QString cadaux1{QString{"Leyendo configuracion"} + fich + QString{"\n"}};
    fprintf(stderr, "%s", cadaux1.toLatin1().constData());
    fprintf(stderr, "%s", "\n");
    QTextStream in{&arch};

    /// Esto es necesario para que se lean bien los carácteres especiales.
    in.setCodec("UTF-8");

    while (!in.atEnd())
    {
      QString cad{in.readLine()};
      /// Hacemos la lectura de líneas de configuración multilínea.
      while (cad.endsWith("\\"))
      {
        cad = cad.left(cad.length() - 2) + in.readLine().trimmed();
      } // end while

      QString simplificada{cad.simplified()};
      QStringList list{simplificada.split(QRegExp{"\\s+"})};

      for (int i{0}; i < 1000; i++)
      {
        if (list[0] == name(i) && !name(i).isEmpty())
        {
          cad = cad.right(cad.length() - name(i).length());
          cad = cad.trimmed();
          m_valores[i] = cad;
        } // end if
      } // end for
    } // end while

    arch.close();
    fprintf(stderr, "%s", "FIN Leyendo configuracion\n");
    return true;
  } // end if

  return false;
}


/// Devuelve el valor de un campo determinado.
/**
\param i Parámetro del que se quiere el valor.
\return El valor que tiene dicho parámetro.
**/
QString BlConfiguration::value(int i)
{
  QString result{""};

  if (m_valores.contains(i))
  {
    /// Si el valor incluye una variable de substitución y esta existe se realiza.
    /// Coge el valor de CONF_REPLACE_STRING y substituye la cadena ${CONF_REPLACE_STRING}
    QString valor{m_valores[i]};

    /// First
    if (i != CONF_REPLACE_STRING)
    {
      valor.replace("${CONF_REPLACE_STRING}", value(CONF_REPLACE_STRING));
    } // end if


    if (i == CONF_DIR_OPENREPORTS)
    {
        QString pathWithoutDbName {""};
        QString pathWithDbName {""};
        QString tmpValor {""};
        bool changeDefaultValue {false};

        if (valor.contains("${CONF_DB_NAME}"))
        {
          tmpValor = valor;
          pathWithoutDbName = tmpValor.replace("${CONF_DB_NAME}", "");
          tmpValor = valor;
          pathWithDbName = tmpValor.replace("${CONF_DB_NAME}", value(CONF_DB_NAME));
          changeDefaultValue = true;
        }
        else if (valor.contains("${UNDERSCORE_CONF_DB_NAME}"))
        {
          tmpValor = valor;
          pathWithoutDbName = tmpValor.replace("${UNDERSCORE_CONF_DB_NAME}", "");
          tmpValor = valor;
          pathWithDbName = tmpValor.replace("${UNDERSCORE_CONF_DB_NAME}", QString("_") + value(CONF_DB_NAME));
          changeDefaultValue = true;
        } // end if

        if (changeDefaultValue)
        {
          /// Which one will we use.
          const QFileInfo dir(pathWithDbName);
          if ((dir.exists()) && (dir.isDir()))
          {
            valor = pathWithDbName;
          }
          else
          {
            valor = pathWithoutDbName;
          } // end if
        } // end if
    } // end if

    result = valor;
  } // end if

  return result;
}


/// Establece el valor de un campo determinado con la tupla que se pasa como parámetro.
/**
\param i El índice del parámetro a cambiar.
\param valor El valor que tomaría dicho parámetro.
**/
void BlConfiguration::setValue(int i, QString valor)
{
  m_valores[i] = valor;
}


struct ConfLine
{
  QString key{""};
  QStringList values;
};


bool BlConfiguration::updateProgramLocalConf(QList<QPair<QString, QString>> & programLocalConf, const QString & fileName)
{
  bool result{false};

  QStringList inputLines;
  QStringList usedConfs;
  QList<ConfLine> outputLines;

  QString configFile{m_programLocalConfFile};
  if (!fileName.isEmpty()) configFile = fileName;

  QFile file{configFile};
  bool opened{file.open(QIODevice::ReadWrite)};

  if (opened)
  {
    QTextStream in{&file};

    /// Needed for non ASCII characters.
    in.setCodec("UTF-8");

    /// 1) Read all file line by line.
    while (!in.atEnd())
    {
      QString line{in.readLine()};
      inputLines << line;
    } // end while

    /// 2) Make groups of lines (by keys).
    for (int i{0}; i < inputLines.size(); ++i)
    {
      ConfLine groupLine;
      QString line{inputLines.at(i)};
      groupLine.values << line;

      QString simplificada{line.simplified()};
      QStringList lineFragments{simplificada.split(QRegExp{"\\s+"})};

      for (int j{0}; j < 1000; j++ )
      {
        if (lineFragments[0] == name(j) && !name(j).isEmpty())
        {
          /// Key found.
          groupLine.key = lineFragments[0].trimmed();
          break;
        } // end if
      } // end for

      {
        /// Hacemos la lectura de líneas de configuración multilínea.
        bool nextLineIsSameGroup{line.endsWith("\\")};
        while (nextLineIsSameGroup)
        {
          ++i;
          line = inputLines.at(i);
          groupLine.values << line;
          if (!line.endsWith('\\'))
          {
            /// Last line in group
            ++i;
            line = inputLines.at(i);
            groupLine.values << line;
            break;
          } // end if
        } // end while
      }

      /// 3) Search over keys.
      for (int j{0}; j < programLocalConf.size(); ++j)
      {
        if (groupLine.key == programLocalConf.at(j).first)
        {
          usedConfs << groupLine.key;
          /// 4) Make substitution of lines.
          groupLine.values.clear();
          groupLine.values << (programLocalConf.at(j).first + QString{" "} + programLocalConf.at(j).second);
        } // end if
      } // end for

      outputLines << groupLine;
    } // end for

    /// Append new lines.
    for (int j{0}; j < programLocalConf.size(); ++j)
    {
      QString key{programLocalConf.at(j).first};
      if (!usedConfs.contains(key))
      {
        ConfLine groupLine;
        groupLine.key = key;
        groupLine.values << (programLocalConf.at(j).first + QString{" "} + programLocalConf.at(j).second);
        outputLines << groupLine;
      } // end if
    } // end for

    /// 5) Truncate file.
    file.resize(0);

    /// 6) Save new content to file.
    QTextStream out{&file};

    /// Needed for non ASCII characters.
    out.setCodec("UTF-8");

    for (int i{0}; i < outputLines.size(); ++i)
    {
      ConfLine confLine{outputLines.at(i)};
      for (int j{0}; j < confLine.values.size(); ++j)
      {
        out << confLine.values.at(j) << QString{"\n"};
      } // end for
    } // end for

    file.close();
    result = true;
  } // end if

  return result;
}
