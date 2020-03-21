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

#ifndef BLCONFIGURATION_H
#define BLCONFIGURATION_H

#include <QString>
#include <QList>
#include <QPair>
#include <QMap>
#include "bldefs.h"

QT_FORWARD_DECLARE_CLASS(QString);


/// Indica el color de fondo de la ventana de apuntes cuando esta cerrada.
#define CONF_BG_APUNTES                1
/// Indica el color de la fuente de la ventana de apuntes cuando esta cerrada.
#define CONF_FG_APUNTES                2
/// Indica el color de fondo de la ventana de apuntes cuando esta abierta.
#define CONF_BG_APUNTESA               3
/// Indica el color de la fuente cuando los aputnes estan abiertos.
#define CONF_FG_APUNTESA               4
/// indica el tamanyo de la tipografia de la introduccion de apuntes.
#define CONF_FONTSIZE_APUNTES          5
/// Indica la fuente de la pantalla de introduccion de apuntes.
#define CONF_FONTFAMILY_APUNTES        6
/// Indica si hay una facturacion alternativa a la que traspasar datos
#define CONF_FACT_ALT                 50
#define CONF_CONT_ALT                 51
/// La URL que el visor HTML debe visitar
#define CONF_URL_CONTENTS             52

/// Traspaso de informacion extendida de los articulos en los documentos.
#define CONF_USE_DESCARTICULO         53


///Indica el color de fondo del libro diario. (obsoleto)
#define CONF_BG_DIARIO               106
/// Indica el color de la letra del diario en el pijama opcion 1.(obsoleto)
#define CONF_FG_DIARIO1              107
/// Indica el color de la letra del diario en el pijama opcion 2.(obsoleto)
#define CONF_FG_DIARIO2              108
/// Indica el tipo de letra del libro diario.(obsoleto)
#define CONF_FONTFAMILY_DIARIO       109
/// Indica el tamanyo de letra del libro diario.(obsoleto)
#define CONF_FONTSIZE_DIARIO         110
/// Indica si en el diario debe aparecer la contrapartida o no.(obsoleto)
#define CONF_CONTRAPARTIDA_DIARIO    111
/// Indica si en el diario debe aparecer el canal o no.(obsoleto)
#define CONF_CANAL_DIARIO            112
/// Indica si en el diario debe aparecer el centro de coste o no.(obsoleto)
#define CONF_CCOSTE_DIARIO           113
/// Indica cual es el color de fondo del extracto.(obsoleto)
#define CONF_BG_EXTRACTO             120
/// Indica cual es el color de fondo del balance.(obsoleto)
#define CONF_BG_BALANCE              121
/// Indica cual es el color de fondo del balance desplegable.(obsoleto)
#define CONF_BG_BALANCETREE          122
/// Indica si en el extracto de cuentas debe aparecer la contrapartida o no.(obsoleto)
#define CONF_CONTRAPARTIDA_EXTRACTO  123
/// Indica si en el extracto de cuentas debe aparecer el canal o no.(obsoleto)
#define CONF_CANAL_EXTRACTO          124
/// Indica si en el extracto de cuentas debe aparecer o no el centro de coste.(obsoleto)
#define CONF_CCOSTE_EXTRACTO         125
/// Indica si en la introduccion de apuntes debe aparecer el campo de contrapartida.(obsoleto)
#define CONF_CONTRAPARTIDA_APUNTES   130
/// Indica si en la introduccion de apuntes debe aparecer la columna de canales.(obsoleto)
#define CONF_CANAL_APUNTES           131
/// Indica si en la introduccion de apuntes deben aparecer los centros de coste.(obsoleto)
#define CONF_CCOSTE_APUNTES          132

/// Colores para BulmaFact. (OBSOLETO)
#define CONF_BG_LISTPRESUPUESTOS     140
#define CONF_BG_LINPRESUPUESTOS      141
#define CONF_BG_DESCPRESUPUESTOS     142
#define CONF_BG_LISTPEDIDOSCLIENTE   143
#define CONF_BG_LINPEDIDOSCLIENTE    144
#define CONF_BG_DESCPEDIDOSCLIENTE   145
#define CONF_BG_LISTALBARANESCLIENTE 146
#define CONF_BG_LINALBARANESCLIENTE  147
#define CONF_BG_DESCALBARANESCLIENTE 148
#define CONF_BG_LISTFACTURASCLIENTE  149
#define CONF_BG_LINFACTURASCLIENTE   150
#define CONF_BG_DESCFACTURASCLIENTE  151
#define CONF_BG_LISTARTICULOS        152

/// Ajuste para BlSubform
#define CONF_RESIZEROWSTOCONTENTS    155



#define CONF_SPLASH_BULMACONT        170
#define CONF_SPLASH_BULMATPV         171
/// Indica que base de datos es la META (OBSOLETO=
#define CONF_METABASE                180
/// Indica por defecto que servidor de bases de datos utilizar.
#define CONF_SERVIDOR                181
/// Indica por defecto que puerto tiene el servidor de bases de datos para conectar con el.
#define CONF_PUERTO                  182
/// Indica la base de datos actual
#define CONF_DB_NAME                 183

/// Indica que visor de documentos PDF va a utilizar el programa.
#define CONF_SPOOL					 191
#define CONF_FLIP                    192
#define CONF_PYTHON                  193
#define CONF_DIR_TMP                 194
#define CONF_PDF                     195
/// Indica que imagen utilizar como splash de BfBulmaFact.
#define CONF_SPLASH_BULMAFACT        196
/// Indica que navegador utilizar para mostrar los resultados de los listados (por defecto
/// mozilla).
#define CONF_NAVEGADOR               197
/// Indica que editor usar para mostrar listados (por defecto kedit).
#define CONF_EDITOR                  198
/// Indica que imagen utilizar como splash de BulmaCont.
#define CONF_SPLASH                  199

/// Indica con que programa abrimos archivos ODS y SXC
#define CONF_ODS                     200
#define CONF_SXC                     201


/// Indica que cliente de email vamos a usar.
#define CONF_EMAIL_CLIENT            202

/// Determina si hay refresco
#define CONF_REFRESH_LIST            210
#define CONF_SHOW_PROGRESS_BAR       211

/// Esto indica donde se van a albergar los programas auxiliares a bulmages (cargaemp,
/// creaemp, borraemp).
#define CONF_EJECUTABLES             300
/// # Esto indica que CSS usar con los listados.
#define CONF_PLANTILLA               301
/// Esto indica que idioma utilizar 'es' 'ca' 'en' 'fr' 'de' o 'locales' (coger el idioma de
/// las locales).
#define CONF_TRADUCCION              305
/// Indica en que directorio se encuentran los archivos de traduccion.
#define CONF_DIR_TRADUCCION          306
/// Indica cual va a ser el archivo de log del programa
#define CONF_ARCHIVO_LOG             307
/// Indica donde se hayan ubicados los archivos de formato de REPORTS (obsoleto)
#define CONF_DIR_REPORTS             308
/// Indica la ruta donde se encuentran los archivos de formato de KUGAR
#define CONF_DIR_KUGAR               309
/// Indica cual va a ser el tipo de letra por defecto.
#define CONF_FONTFAMILY_BULMAGES     310
/// Indica cual va a ser el tamanyo de fuente por defecto.
#define CONF_FONTSIZE_BULMAGES       311
/// Indica cuales son los plugins para la contabilidad que se cargan.
#define CONF_PLUGINS_BULMACONT       312
#define CONF_PLUGINS_BULMAFACT       313
#define CONF_PLUGINS_BULMAGES        314
#define CONF_PLUGINS_BULMATPV        315

/// Indica cuales son los directorios de plugins
#define CONF_DIR_PLUGINS             316

/// Indica el numero de caracteres para activar el filtrado
#define CONF_NUMCHAR_RELOAD_FILTRO   327

/// Indica donde estan los datos del programa. Por defecto /usr/share/bulmages.
#define CONF_PROGDATA                321
/// Determina el directorio donde se hayan ubicados los archivos de datos para OPENREPORTS.
#define CONF_DIR_OPENREPORTS         330

/// Determina el directorio de los asientos inteligentes
#define CONF_DIR_AINTELIGENTES       331

/// Determina si se va a usar el modo experto o no
#define CONF_MODO_EXPERTO            332

/// Determina el directorio donde estan las configuraciones de tablas por defecto. Usado en caso de que no existan.
#define CONF_DIR_DEFAULT_CONFS       333

/// Dictamina cuales son los privilegios del usuario.
#define CONF_PRIVILEGIOS_USUARIO     420
/// ALERTAS_DB Determina si los errores de la base de datos deben ser alertados via
/// messagebox o no.
#define CONF_ALERTAS_DB              421
/// CONF_LOGIN_USER    Determina el login de usuario. Inicialmente vale "".
#define CONF_LOGIN_USER              422
/// CONF_PASSWORD_USER     Determina el password del usuario. Inicialmente vale "".
#define CONF_PASSWORD_USER           423
/// Esta variable indica el directorio de trabajo del usuario.
#define CONF_DIR_USER                424
/// Esta variable indica donde se van a guardar los archivos de imagenes de los articulos
/// en BulmaFact.
#define CONF_DIR_IMG_ARTICLES        425
#define CONF_DIR_IMG_PERSONAL        426
#define CONF_DIR_CANUALES            427
#define CONF_DIR_THUMB_ARTICLES      428
#define CONF_DIR_IMG_ALUMNOS         429

/// Hoja de estilo a usar con bulmages
#define CONF_STYLESHEET              430
#define CONF_CUPS_DEFAULT_PRINTER    440
/// Impresora de tickets
#define CONF_TICKET_PRINTER_FILE     450
/// Visor de TPV
#define CONF_TPV_VISOR_FILE          451
/// Modo Full Screen para el TPV
#define CONF_TPV_FULLSCREEN          452
#define CONF_TPV_PRINTER_LOGO        453
/// Cajon Portamonedas
#define CONF_CASHBOX_FILE            454
#define CONF_CASHBOX_OPEN_CODE       455
/// Configuracion para articulos varios
#define CONF_ARTICULOS_VARIOS        456
/// Ancho en pixeles de la celda
#define CONF_TPV_CELL_WIDTH          457
/// Numero de celdas por fila
#define CONF_TPV_CELLS_PER_ROW       458
/// Puerto para la sincronizacion de TPVs
#define CONF_TPV_SERVER_SYNC_IP      459
/// Puerto serie de una posible bascula
#define CONF_TPV_BASCULA_FILE        460
/// Impresion al cobrar de tickets ya impresos antes 
#define CONF_TPV_REIMPRIMIR          461
/// El texto a escribir cuando pulsamos el boton MESA
#define CONF_TPV_TEXTOMESA           462

#define CONF_TPV_CATEGORY_TREE       463

#define CONF_PLUGINBT_MODIFICADORES_EDIT_ARTICLE_DESCRIPTION    464
#define CONF_PLUGINBT_MODIFICADORES_EDIT_MODIFICADORES          465

/// CONF_NUM_AUT_* determina si se utilizaran numeradores automaticos para presupuestos,
/// albaranes y facturas inicialmente vale "".
#define CONF_NUM_AUT_PRESUP          500
#define CONF_NUM_AUT_ALBARAN         501
#define CONF_NUM_AUT_FACTURA         502
/// CONF_MOSTRAR_ALMACEN determina si se visualizaran el almacen en los listados y si
/// aparecera inhibido en las entradas de datos inicialmente vale "".
#define CONF_MOSTRAR_ALMACEN         510

/// COnfiguraciones para el TPV
#define CONF_IDCLIENTE_DEFECTO       520
#define CONF_IDALMACEN_DEFECTO       521
#define CONF_IDARTICULO_DEFECTO      522
#define CONF_IDTRABAJADOR_DEFECTO    523
#define CONF_IDFORMA_PAGO_VISA       524
#define CONF_IDFORMA_PAGO_CONTADO    525
#define CONF_SERIE_FACTURA_DEFECTO   526
#define CONF_IDFORMA_PAGO_DEFECTO    527

#define CONF_BACKGROUND_COLOR        528
#define CONF_BACKGROUND_IMAGE        529

#define CONF_BLOCK_WINDOWS           530

/// CONF_DEBUG indica el modo de debug.
#define CONF_DEBUG                   550
///Indica el color de fondo alternativo del pijama del libro diario.
#define CONF_BG_ALT_DIARIO           551

/// Sistema de configuracion en un directorio global
#define CONF_DIR_CONFIG              560
#define CONF_GLOBAL_CONFIG_USER      561
#define CONF_GLOBAL_CONFIG_COMPANY   562

#define CONF_CENTER_TEXT_FIELDS      563
#define CONF_ASK_BEFORE_EXIT         564
#define CONF_TPV_CATEGORIES_COLUMNS  565
#define CONF_MAIN_WINDOW_TITLE       566
#define CONF_REPLACE_STRING          567
#define CONF_DOCUMENTATION           568
#define CONF_PREVIEW                 569
#define CONF_DEFAULT_ARTICLE_LEVEL   570
#define CONF_TOTAL_DECIMAL_PLACES    571
#define CONF_DIR_IMG_CLIENTS         572
#define CONF_LAST_USED_DATABASE      573

#define CONF_BGP_DISABLE_UPDATE                 900

#define CONF_BGP_ENABLE_API_SERVER              901 // WebSocket
#define CONF_BGP_API_SERVER_USE_TLS             902 // WebSocket
#define CONF_BGP_API_SERVER_LISTEN_ADDRESSES    903 // WebSocket
#define CONF_BGP_API_SERVER_PORT                904 // WebSocket
#define CONF_BGP_API_SERVER_TLS_CERT_FILE       905 // WebSocket
#define CONF_BGP_API_SERVER_TLS_KEY_FILE        906 // WebSocket
#define CONF_BGP_API_SERVER_TLS_CA_FILE         907 // WebSocket

typedef QMap<int, QString> mapa;


/// Se encarga de recoger todos los parámetros de configuración de la aplicación y los
/// centraliza en un único objeto que luego es instanciado como objeto global \ref confpr.
/// De este modo cualquier objeto de la aplicación puede consultar (sin demasiados problemas
/// cual es la configuración que le corresponde).
/// NOTA: también ha adoptado la capacidad de ser un paso de parámetros configurables de
/// la aplicación. Esto se traduce en que algunos parámetros se pueden modificar en tiempo
/// de ejecución.
class BL_EXPORT BlConfiguration
{
  private:
    /// La forma de almacenar los campos va a ser mediante un mapa de valores.
    /// De esta forma no tendremos memoria desaprovechada.
    QMap<int, QString> m_valores;

    QString m_dirGlobalConf;
    QString m_dirLocalConf;
    QString m_genericGlobalConfFile;
    QString m_programGlobalConfFile;
    QString m_genericLocalConfFile;
    QString m_programLocalConfFile;

  public:
    explicit BlConfiguration(QString nombreprograma, QString confReplaceString = "");
    ~BlConfiguration();
    QString value(int);
    void setValue(int, QString);
    bool readConfig(QString);
    void saveConfig();
    QString name(int);
    bool updateProgramLocalConf(QList<QPair<QString, QString>> & programLocalConf, const QString & fileName = ""); /// List of key-value.
};


/// ATENCION.- Este es un objeto global, y va a existir durante toda la aplicación así que
/// Se va a llamar muy a menudo a este objeto en los demás archivos.
/// Existe una instancia del objeto global g_confpr.
extern "C" BL_EXPORT BlConfiguration *g_confpr;
void BL_EXPORT initConfiguration(QString, QString = "");

#endif

