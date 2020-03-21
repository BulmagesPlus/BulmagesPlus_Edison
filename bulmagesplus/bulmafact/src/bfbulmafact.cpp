/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
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


#ifdef Q_OS_WIN32
  #include <windows.h>
#endif

#include <QImage>
#include <QPixmap>
#include <QToolButton>
#include <QMenuBar>
#include <QFile>
#include <QStatusBar>
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QApplication>
#include <QTextStream>
#include <QPainter>
#include <QMainWindow>
#include <QProgressBar>
#include <QStringList>
#include <QProcessEnvironment>

#include "blworkspace.h"
#include "bfbulmafact.h"
#include "blwindowlistdock.h"
#include "bfaboutview.h"
#include "blperiodicitycombobox.h"
#include "blplugins.h"
#include "blfunctions.h"
#include "../../bulmagesplus_version.h"

#include "blupdate.h"
#include "blupdateconfig.h"
#include "blbannerbar.h"
#include "blsupportemail.h"


/** Prepara la pantalla principal para que tenga todos los componentes.
    Crea el workspace y lo inicializa.
    Crea la instancia de company y la inicializa y almacena esta en el puntero m_company.
    Crea la lista de ventanas.
*/
/**
\param bd
**/
BfBulmaFact::BfBulmaFact ( QString bd ) : BlMainWindow(0, Qt::Window)
{
    BL_FUNC_DEBUG
    setupUi ( this );
    setAnimated ( false );
    actionAcerca_de->setText("&Acerca de BulmagesPlus Factu " + QString(BULMAGESPLUS_VERSION_SHORT) + " ...");
    actionActualizaciones->setVisible(false); /// TODO: Oculto hasta que funcione.

    setUpdatesEnabled ( true );
    pWorkspace = new BlWorkspace ( this );

    /// Es Base de datos local o remota??
    /// Menú no visible si la base de datos es remota.
    if (g_confpr->value(CONF_SERVIDOR) != QString("--"))
    {
      menuMantenimiento->hide();
    } // end if

    pWorkspace->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    pWorkspace->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QFrame *m_frame1 = new QFrame();
    m_pb = new QProgressBar();
    m_pb->setMaximum ( 100 );
    m_pb->setMinimum ( 0 );
    m_pb->setValue ( 0 );
    /// Hacemos que el ProgressBar est&eacute; invisible hasta que se seleccione una empresa.
    m_pb->setVisible ( false );
    setCentralWidget ( m_frame1 );
    /// Creamos un VerticalLayout donde metemos el contenido central del QMainWindow.
    QVBoxLayout *vboxlayout = new QVBoxLayout ( this->centralWidget() );
    vboxlayout->setSpacing ( 0 );
    vboxlayout->setMargin ( 0 );
    vboxlayout->addWidget ( pWorkspace );
    vboxlayout->addWidget ( m_pb );

    m_company = new BfCompany ( this );
    m_company->setProgressBar ( m_pb );
//     m_company->init->append("BulmaFact");
//     m_company->init->append("Bulmatpv");

    QStringList tipos;
    tipos.append("BulmaFact");
    tipos.append("BulmaTpv");

    m_company->init (tipos, bd);
    m_company->setWorkspace ( pWorkspace );

    connect ( pWorkspace, SIGNAL ( subWindowActivated ( QMdiSubWindow * ) ), this, SLOT ( informaindexador ( QMdiSubWindow * ) ) );

    /// Aqui creamos la ventana dock para meter las distintas ventanas.
    m_list = new BlWindowListDock ( 0 );
    m_list->setVisible ( false );
    /// Iniciamos el listventanas con el workspace para que pueda operar con el.
    m_list->setWorkspace ( pWorkspace );

    connect ( m_list, SIGNAL ( visibilityStateChanged ( bool ) ), this, SLOT ( setActionIndexador ( bool ) ) );
    addDockWidget ( Qt::LeftDockWidgetArea, m_list );

    m_company->setListVentanas ( m_list );

    m_list->setVisible ( true );
    m_pb->setVisible ( false );
    statusBar() ->showMessage ( bd, 2000 );


    BlUpdate *update = new BlUpdate(this);
    update->setProgramRunningName("factu"); /// Cambiar para cada version.

    m_update = update;

    BlBannerBar *banner = new BlBannerBar(bulmagesplusBar);
    banner->setProgramName("factu");
    banner->initBar();

    QAction *accionA = new QAction( QIcon ( ":/imagesplus/support-inactive.png"), _("Contactar"), 0);
    accionA->setObjectName("mui_left");
    connect(accionA, SIGNAL( triggered() ), this, SLOT( supportWindow() ) );

    bulmagesplusBar->addAction(accionA);
}


/** Hacemos la creacion de las ventanas principales para que enten en plugins
*/
/**
\param splashScr
**/
void BfBulmaFact::createMainWindows ( BlSplashScreen *splashScr )
{
    BL_FUNC_DEBUG

    if (((BlUpdate *)m_update)->isUserRegistered()) {
        /// Mira si es versión PRO o ELITE

        QString eliteUntil = ((BlUpdate *)m_update)->eliteUntil();
        
        if (!eliteUntil.isEmpty())
        {
          QDate currentDate = QDate::currentDate();
          QDate eliteUntilDate = QDate::fromString(eliteUntil, "yyyy-MM-dd");
          
          if (currentDate >= eliteUntilDate)
          {
            setWindowTitle ( "BulmagesPlus Factu " + QString(BULMAGESPLUS_VERSION_SHORT_ELITE) + " :: " + m_company->currentUser() + "@" + m_company->dbName() );
          } else {
            setWindowTitle ( "BulmagesPlus Factu " + QString(BULMAGESPLUS_VERSION_SHORT_PRO) + " :: " + m_company->currentUser() + "@" + m_company->dbName() );
          } // end if 
          
        } else {
          setWindowTitle ( "BulmagesPlus Factu " + QString(BULMAGESPLUS_VERSION_SHORT_PRO) + " :: " + m_company->currentUser() + "@" + m_company->dbName() );
        } // end if

        /// Oculto menú de registro
        actionRegistrar->setVisible(false);
    } else {
        setWindowTitle ( "BulmagesPlus Factu " + QString(BULMAGESPLUS_VERSION_SHORT_BASIC) + " :: " + m_company->currentUser() + "@" + m_company->dbName() );

        /// Si no estamos registrados, carga el los menus de preview
        m_upgradePreview = new BfUpgradePreview(this);
        m_upgradePreview->loadActions();
    } // end if
    
    m_company->createMainWindows ( splashScr );
    
}


/** Libera memoria destruyendo todos los objetos creados.
*/
/**
**/
BfBulmaFact::~BfBulmaFact()
{
    BL_FUNC_DEBUG
    /// Borramos el WorkSpace
    delete pWorkspace;
    
}


/// \TODO: Revisar este metodo.
/**
**/
void BfBulmaFact::s_FPago()
{
    BL_FUNC_DEBUG
    m_company->s_FPago();
    
}


/** Intercambia entre el modo ventana completa y el modo ventana normal
*/
/**
**/
void BfBulmaFact::s_ventanaCompleta()
{
    BL_FUNC_DEBUG
    if ( isFullScreen() ) {
        showNormal();
    } else {
        showFullScreen();
    } // end if
    
}


/** Metodo que responde a la pulsacion de About en el menu.
    Inicializa la ventana de About y la muestra.
*/
/**
**/
void BfBulmaFact::s_About()
{
    BL_FUNC_DEBUG
    BfAboutView about;
    about.exec();
    
}


/** Evento de cerrar la ventana principal.
    Libera parte de la memoria reservada. Ya que sabemos que se va a cerrar el programa.
*/
/**
**/
void BfBulmaFact::closeEvent ( QCloseEvent *event )
{
    BL_FUNC_DEBUG


    /// Antes de salir hacemos un mensaje de advertencia.
    if ( g_confpr->value( CONF_ASK_BEFORE_EXIT ) == "TRUE" ) {
	 QMessageBox msgBox;
	 msgBox.setText(_("Seguro que desea abandonar el programa "));
	 msgBox.setInformativeText(_("Se perderan los cambios no guardados"));
	 QPushButton *aceptarButton = msgBox.addButton(_("&Si"), QMessageBox::ActionRole);
     QPushButton *cancelarButton = msgBox.addButton(_("&Cancelar"), QMessageBox::ActionRole);
	 msgBox.setDefaultButton(cancelarButton);
	 msgBox.setIcon(QMessageBox::Question);

	 int ret = msgBox.exec();

     if (msgBox.clickedButton() == cancelarButton)
     {
	   event->ignore();
	   
	   return;
	 }
    } // END IF

    g_plugins->run ( "BfBulmaFact_closeEvent", this );

    /// Borramos el BfCompany
    delete m_company;
    m_company = NULL;
    /// Borramos el list_ventanas
    delete m_list;
    m_list = NULL;
	
    
}


/** Captura el evento de cambio de ventana en el workSpace y actua sobre el
    listado de ventanas para que seleccione la ventana adecuada.
*/
/**
\param w
\return
**/
void BfBulmaFact::informaindexador ( QMdiSubWindow *w )
{
    BL_FUNC_DEBUG

    /// Si no esta inicializado company no se le puede informar.
    if ( m_company == NULL ) {
        
        return;
    } // end if

    /// No existe una ventana que activar.
    if ( w == NULL ) {
        m_company->deselectWindow();
        
        return;
    } // end if
    m_company->selectWindow ( w->windowTitle(), w );
}


/** Muestra la ayuda del programa.
*/
/**
**/
/*
void BfBulmaFact::on_actionDocumentacion_triggered()
{

  
    BL_FUNC_DEBUG
    blWebBrowser("http://www.iglues.org/wiki_users/doku.php");

    
}
*/

///
/**
\param docError
**/
void BfBulmaFact::documentacionError ( const QString docError )
{
    BL_FUNC_DEBUG
    
}


///
/**
\param visible
**/
void BfBulmaFact::setActionIndexador ( bool visible )
{
    BL_FUNC_DEBUG
    if ( visible == true ) {
        actionIndexador->setChecked ( true );
    } else {
        actionIndexador->setChecked ( false );
    } // end if
    
}


///
/**
 * Activa o desactiva el modo Experto.
**/
void BfBulmaFact::on_actionModo_Experto_triggered()
{
    BL_FUNC_DEBUG
    if ( actionModo_Experto->isChecked() == true ) {
       g_confpr->setValue(CONF_MODO_EXPERTO, "TRUE");
    } else {
       g_confpr->setValue(CONF_MODO_EXPERTO, "FALSE");
    } // end if
    
}

void BfBulmaFact::on_actionSalir_triggered()
{
  close();
}

///
/**
**/
void BfBulmaFact::on_actionIndexador_triggered()
{
    BL_FUNC_DEBUG
    if ( actionIndexador->isChecked() == true ) {
        m_company->s_indexadorCambiaEstado ( true );
    } else {
        m_company->s_indexadorCambiaEstado ( false );
    } // end if
    
}


///
/**
**/
void BfBulmaFact::on_actionTipoIVA_triggered()
{
    BL_FUNC_DEBUG
    m_company->s_TipoIVAView();
    
}


///
/**
**/
void BfBulmaFact::on_actionTasaIVA_triggered()
{
    BL_FUNC_DEBUG
    m_company->s_TasaIVAView();
    
}


///
/**
\return
**/
BlWorkspace * BfBulmaFact::workspace() const
{
    BL_FUNC_DEBUG
    
    return pWorkspace;
}

///
/**
\return
**/
BfCompany * BfBulmaFact::company() const
{
    BL_FUNC_DEBUG
    
    return m_company;
}

///
/**
**/
void BfBulmaFact::on_actionPaises_triggered()
{
    BL_FUNC_DEBUG
    m_company->muestraPaises();
    
}


///
/**
**/
void BfBulmaFact::on_actionSeries_de_Factura_triggered()
{
    BL_FUNC_DEBUG
    m_company->s_seriesFactura();
    
}


///
/**
**/
void BfBulmaFact::on_actionTrabajadores_triggered()
{
    BL_FUNC_DEBUG
    m_company->s_trabajadores();
    
}


///
/**
**/
void BfBulmaFact::on_actionBancos_triggered()
{
    BL_FUNC_DEBUG
    m_company->s_bancos();
    
}


///
/**
**/
void BfBulmaFact::on_actionVentana_Completa_triggered()
{
    BL_FUNC_DEBUG
    s_ventanaCompleta();
    
}


///
/**
**/
void BfBulmaFact::on_actionAcerca_de_triggered()
{
    BL_FUNC_DEBUG
    s_About();
    
}


///
/**
**/
void BfBulmaFact::on_actionAlmacenes_triggered()
{
    BL_FUNC_DEBUG
    m_company->s_almacenes();
    
}


///
/**
**/
void BfBulmaFact::on_actionParametros_triggered()
{
    BL_FUNC_DEBUG
    m_company->s_newBfConfiguracionView();
    
}


///
/**
**/
void BfBulmaFact::on_actionFormas_de_Pago_triggered()
{
    BL_FUNC_DEBUG
    s_FPago();
    
}


///
/**
**/
void BfBulmaFact::on_actionOrdenar_Ventanas_triggered()
{
    BL_FUNC_DEBUG

    pWorkspace->tileSubWindows();
}


///
/**
**/
void BfBulmaFact::on_actionOrganizaci_n_en_Cascada_triggered()
{
    BL_FUNC_DEBUG

    pWorkspace->cascadeSubWindows();
}


void BfBulmaFact::on_actionActualizaciones_triggered()
{
    BlUpdateConfig updateConfig;
    updateConfig.exec();
}


void BfBulmaFact::resizeEvent ( QResizeEvent * event )
{
    BlMainWindow::resizeEvent(event);
    emit resizeEventCalled();
}

void BfBulmaFact::supportWindow()
{
    BlSupportEmail *emailWindow = new BlSupportEmail(this);
    emailWindow->show();
}


BfUpgradePreview * BfBulmaFact::upgradePreview() const
{
    return m_upgradePreview;
}


void BfBulmaFact::on_actionActualizador_triggered()
{
  ((BlUpdate *)m_update)->executeUpdater();
}


void BfBulmaFact::on_actionBases_de_datos_triggered()
{
    QList<QString> checkExistingFiles;
    QString appDirPath {g_confpr->value(CONF_EJECUTABLES)};

#ifdef Q_OS_WIN32
    WCHAR winPath[MAX_PATH];
    GetWindowsDirectoryW(winPath, MAX_PATH);
    QString path = appDirPath + QString("\\..\\..\\bash\\bin\\bash.exe");
    QString path2 = QString("\"") + path + QString("\"");
    QString command = path2 + QString(" --login -c \"p=$(cygpath -u \"\"") + g_confpr->value(CONF_EJECUTABLES).replace(QString("\""), QString("")) + QString("\"\"); cd $p; ./admin_databases_") + QString(PRODUCT_LOWER_CODENAME) + QString(".sh --calledfromprogram") + QString("\"");
    checkExistingFiles.append(appDirPath + QString("\\admin_databases_" + QString(PRODUCT_LOWER_CODENAME) + QString(".sh")));
    bool forceNewConsole = true;

    BlDebug::blDebug(command);
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
    if (env.value("LD_LIBRARY_PATH").isEmpty())
    {
      env.insert("LD_LIBRARY_PATH", appDirPath + QString("../../libs"));
    }
    else
    {
      env.insert("LD_LIBRARY_PATH", appDirPath + QString("../../libs:") + env.value("LD_LIBRARY_PATH"));
    } // end if

    QString loaderLinux {blGetEnv("BGP_LD_LINUX")};
    if (!loaderLinux.isEmpty()) loaderLinux += QString(" ");

    QString path {""};
    QString command {loaderLinux + appDirPath + QString("/bgpterminal.bin /bin/bash -c \"") + appDirPath + QString("/admin_databases_") + QString(PRODUCT_LOWER_CODENAME) + QString(".sh --calledfromprogram") + QString("\"")};
    checkExistingFiles.append(appDirPath + QString("/admin_databases_" + QString(PRODUCT_LOWER_CODENAME) + QString(".sh")));
    checkExistingFiles.append(appDirPath + QString("/bgpterminal.bin"));
    bool forceNewConsole {false};

    BlDebug::blDebug(command);
    blStartDetachedProcess(path, command, checkExistingFiles, forceNewConsole, &env);
#endif
}


void BfBulmaFact::on_actionRegistrar_triggered()
{
    QString appDirPath {g_confpr->value(CONF_EJECUTABLES)};
    QList<QString> checkExistingFiles;

#ifdef Q_OS_WIN32
    WCHAR winPath[MAX_PATH];
    GetWindowsDirectoryW(winPath, MAX_PATH);
    QString path {appDirPath + QString("\\..\\..\\bash\\bin\\bash.exe")};
    QString path2 {QString("\"") + path + QString("\"")};
    QString command {path2 + QString(" --login -c \"p=$(cygpath -u \"\"") + g_confpr->value(CONF_EJECUTABLES).replace(QString("\""), QString("")) + QString("\"\"); cd $p; ./register_") + QString(PRODUCT_LOWER_CODENAME) + QString(".sh --calledfromprogram") + QString("\"")};
    checkExistingFiles.append(appDirPath + QString("\\register_") + QString(PRODUCT_LOWER_CODENAME) + QString(".sh"));
    bool forceNewConsole = true;

    BlDebug::blDebug(command);
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
    if (env.value("LD_LIBRARY_PATH").isEmpty())
    {
      env.insert("LD_LIBRARY_PATH", appDirPath + QString("../../libs"));
    }
    else
    {
      env.insert("LD_LIBRARY_PATH", appDirPath + QString("../../libs:") + env.value("LD_LIBRARY_PATH"));
    } // end if

    QString loaderLinux {blGetEnv("BGP_LD_LINUX")};
    if (!loaderLinux.isEmpty()) loaderLinux += QString(" ");

    QString path {""};
    QString command {loaderLinux + appDirPath + QString("/bgpterminal.bin /bin/bash -c \"") + appDirPath + QString("/register_") + QString(PRODUCT_LOWER_CODENAME) + QString(".sh --calledfromprogram") + QString("\"")};
    checkExistingFiles.append(appDirPath + QString("/register_" + QString(PRODUCT_LOWER_CODENAME) + QString(".sh")));
    checkExistingFiles.append(appDirPath + QString("/bgpterminal.bin"));
    bool forceNewConsole {false};

    BlDebug::blDebug(command);
    blStartDetachedProcess(path, command, checkExistingFiles, forceNewConsole, &env);
#endif
}


void BfBulmaFact::on_actionSoporte_remoto_triggered()
{
    BL_FUNC_DEBUG

    /// Ejecuta TeamLIDER.
    QProcess process{this};
    QList<QString> checkExistingFiles;
    QString appDirPath {g_confpr->value(CONF_EJECUTABLES)};

#ifdef Q_OS_WIN32
    QString path{appDirPath + QString{"\\TeamLIDER.exe"}};
    QString path2{QString("\"") + path + QString("\"")};
    QString command{path2 + QString(" --calledfromprogram")};
    checkExistingFiles.append(appDirPath + QString{"\\TeamLIDER.exe"});
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
    if (env.value("LD_LIBRARY_PATH").isEmpty())
    {
      env.insert("LD_LIBRARY_PATH", appDirPath + QString("../../libs"));
    }
    else
    {
      env.insert("LD_LIBRARY_PATH", appDirPath + QString("../../libs:") + env.value("LD_LIBRARY_PATH"));
    } // end if

    QString loaderLinux {blGetEnv("BGP_LD_LINUX")};
    if (!loaderLinux.isEmpty()) loaderLinux += QString(" ");

    QString command{loaderLinux + appDirPath + QString{"/TeamLIDER.bin --calledfromprogram"}};
    process.setWorkingDirectory(appDirPath);
    process.setProcessEnvironment(env);
    process.startDetached(command);
#endif
}

