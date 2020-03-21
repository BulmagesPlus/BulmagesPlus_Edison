/***************************************************************************
 *   Copyright (C) 2012 by Daniel Ripoll Osma.                             *
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

#include <QLibrary>
#include <QDir>
#include <QGridLayout>
#include <QIODevice>
#include "blupgradepreview.h"
#include "blplugins.h"
#include "../../bulmagesplus_version.h"


BlUpgradePreviewView::BlUpgradePreviewView(QWidget * parent) : QDialog{parent}
{
  BL_FUNC_DEBUG

  QGridLayout *layout{new QGridLayout{}};
  this->setLayout(layout);
  this->setMinimumSize(QSize(650, 450));
  this->setMaximumSize(QSize(650, 450));
  g_view = new QWebView{};
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(g_view);
  m_parent = parent;
}


BlUpgradePreviewView::~BlUpgradePreviewView()
{
  BL_FUNC_DEBUG

  delete g_view;
}


void BlUpgradePreviewView::setHtml(QString content)
{
  BL_FUNC_DEBUG

  g_view->settings()->clearMemoryCaches();
  g_view->setHtml(content);
  g_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  connect(g_view, SIGNAL(linkClicked(const QUrl)), this, SLOT(detectUrl(const QUrl)), Qt::DirectConnection);
  QString title{g_view->title()};
  this->setWindowTitle(title);
  g_view->show();
}

void BlUpgradePreviewView::detectUrl(const QUrl &url)
{
  BL_FUNC_DEBUG

  if (url.isValid())
  {
    blWebBrowser(url.toString());
  } // end if
}






BlUpgradePreview::BlUpgradePreview()
{
    BL_FUNC_DEBUG
}

BlUpgradePreview::BlUpgradePreview( BlSubForm *parent ) : QObject ( parent )
{
    BL_FUNC_DEBUG
}

BlUpgradePreview::BlUpgradePreview( BlTreeWidget *parent ) : QObject ( parent )
{
    BL_FUNC_DEBUG
}


BlUpgradePreview::~BlUpgradePreview()
{
    BL_FUNC_DEBUG
}

///
/**
\param menu
**/
void BlUpgradePreview::paintMenu ( QMenu *menu )
{
    BL_FUNC_DEBUG

    /// Checkeamos si nuestro demo-action existe como plugin
    if ( !unloadPreview("libpluginbl_subform2ods"))
    {
        /// Creamos nuestra accion y separador correspondiente y la metemos en el menu que le pasamos por referencia, siendo este su parent
        menu->addSeparator();
        QAction *accionA = new QAction ( _ ( "Exportar a hoja de calculo (ODS)") , menu );
        accionA->setIcon ( QIcon(":/Images/exportods.png") );
        //accionA->setObjectName ( "mui_actionUpgradeSubform2ODS" + QString::number(m_iter) );
        accionA->setObjectName ( "mui_actionUpgradeSubform2ODS" );
        //m_iter++;
        m_listActions << accionA;
        menu->addAction ( accionA );
    } // end if

}

///
/**
\param sub
\return
**/
int BlUpgradePreview::BlSubForm_Post ( BlSubForm *sub )
{
    BL_FUNC_DEBUG

    /// Checkeamos si nuestro demo-action existe como plugin
    if ( !unloadPreview("libpluginbl_subform2ods"))
    {
        /// Realizamos los connects correspondientes
        sub->QObject::connect ( sub, SIGNAL ( pintaMenu ( QMenu * ) ), this, SLOT ( paintMenu ( QMenu * ) ) );
        sub->QObject::connect ( sub, SIGNAL ( trataMenu ( QAction * ) ), this, SLOT ( launchODSPreview(QAction *) ) );
    } // end if

    return 0;
}

///
/**
\param sub
\return
**/
int BlUpgradePreview::BlTreeWidget_Post ( BlTreeWidget *sub )
{
    BL_FUNC_DEBUG

    /// Checkeamos si nuestro demo-action existe como plugin
    if ( !unloadPreview("libpluginbl_subform2ods" ))
    {
        /// Realizamos los connects correspondientes
        sub->QObject::connect ( sub, SIGNAL ( pintaMenu ( QMenu * ) ), this, SLOT ( paintMenu ( QMenu * ) ) );
        sub->QObject::connect ( sub, SIGNAL ( trataMenu ( QAction * ) ), this, SLOT ( launchODSPreview(QAction *) ) );
    }

    return 0;
}


int BlUpgradePreview::BlSubForm_preparaMenu ( BlSubForm *sub )
{
    BL_FUNC_DEBUG

    /// Checkeamos si nuestro demo-action existe como plugin
    if ( !unloadPreview("libpluginbl_subform2ods"))
    {
        /// Creamos el hboxlayout1 si no existe
        QHBoxLayout *m_hboxLayout1 = sub->mui_menusubform->findChild<QHBoxLayout *> ( "hboxLayout1" );
        if ( !m_hboxLayout1 ) {
            m_hboxLayout1 = new QHBoxLayout ( sub->mui_menusubform );
            m_hboxLayout1->setSpacing (0 );
            m_hboxLayout1->setMargin ( 0 );
            m_hboxLayout1->setObjectName ( QString::fromUtf8 ( "hboxLayout1" ) );
        } // end if

        /// Creamos el QToolButtton para exportar a ODS

        QAction *accionA = new QAction ( _ ( "Exportar a hoja de calculo (ODS)") , m_hboxLayout1 );
        //accionA->setObjectName ( "mui_actionUpgradeSubform2ODS" + QString::number(m_iter) );
        accionA->setObjectName ( "mui_actionUpgradeSubform2ODS" + QString::number(m_iter) );
        
        QToolButton *sel = new QToolButton ( sub->mui_menusubform );
        sel->setStatusTip ( _("Exportar a hoja de calculo (ODS)") );
        sel->setToolTip ( _("Exportar a hoja de calculo (ODS)") );
        sel->setMinimumSize ( QSize ( 18, 18 ) );
        sel->setIcon ( QIcon ( ":/Images/exportods.png" ) );
        sel->setIconSize ( QSize ( 18, 18 ) );
        //m_iter++;

        m_listActions << accionA;
        m_hboxLayout1->addWidget ( sel );

        sel->connect (sel, SIGNAL(clicked(bool)), this, SLOT( launchODSPreview(bool) ));
    }

    return 0;
}


void BlUpgradePreview::launchODSPreview(bool released)
{
    /// Salta nuestra ventana de adquisicion del pack de exportaciones
    QString parametersBulmagesPlus = QString("?name=pluginbl_subform2ods&product_auth_id=" + QString(PRODUCT_AUTH_ID) + "&" + "os=" + QString(OPERATING_SYSTEM) + "&" + "arch=" + QString(ARCH) + "&" + "build_version=" + QString(BUILD_VERSION));
    showUpgradePreviewInfo(parametersBulmagesPlus);
}

void BlUpgradePreview::launchODSPreview(QAction *action)
{
    /// Salta nuestra ventana de adquisicion del pack de exportaciones solo si es nuestra accion
    if (action->objectName() == "mui_actionUpgradeSubform2ODS")
    {
        QString parametersBulmagesPlus = QString("?name=pluginbl_subform2ods&product_auth_id=" + QString(PRODUCT_AUTH_ID) + "&" + "os=" + QString(OPERATING_SYSTEM) + "&" + "arch=" + QString(ARCH) + "&" + "build_version=" + QString(BUILD_VERSION));
        showUpgradePreviewInfo(parametersBulmagesPlus);
    }
}


void BlUpgradePreview::showUpgradePreviewInfo(const QString &parameters, const QString &templateName, const QString &zoneName)
{
    
    BL_FUNC_DEBUG


    /// Checkeamos nuestra local actual
    QString currentLocale = QLocale().name();

    /// Creamos un QString con el archivo routeado
    QString routedFile = QDir::fromNativeSeparators (g_confpr->value( CONF_DOCUMENTATION )) + "/" + QDir::fromNativeSeparators (zoneName) + "/"  + currentLocale + "/" + templateName  + ".html";


    /// Esto es solo para debugging, en la release no debe incluirse
    fprintf(stderr, "\n----------------------\n");
    fprintf(stderr, routedFile.toLatin1() );
    fprintf(stderr, "\n----------------------\n");

    /// Creamos un QUrl en base al archivo routeado
    QUrl url = QUrl::fromLocalFile(routedFile);

    QString htmlcontent = "<!DOCTYPE html><html><head><meta charset=\"utf-9\"/><title>Empty</title></head><body></body></html>";

    /// Hacemos uso de BlUpgradePreviewView para mostrar nuestra ventana de adquisicion de plugins

    QFile file(url.toLocalFile());
    if (file.open (QIODevice::ReadOnly))
    {
        QTextStream stream ( &file );
        htmlcontent = stream.readAll();
        htmlcontent.replace("${PARAMETERS}", parameters);
        fprintf(stderr, parameters.toUtf8());
        htmlcontent.replace("${CONF_DOCUMENTATION}", g_confpr->value( CONF_DOCUMENTATION) );
        file.close();
    }

    BlUpgradePreviewView *pv = new BlUpgradePreviewView;
    //pv->loadUrl(url);
    QString cssfile = g_confpr->value( CONF_DOCUMENTATION) + "/upgrade_preview/resources/style.css";
    pv->g_view->settings()->setUserStyleSheetUrl(QUrl::fromLocalFile(cssfile));
    pv->setHtml(htmlcontent);
    pv->exec();

}


bool BlUpgradePreview::detectPlugins(const QString &theplugin)
{
    BL_FUNC_DEBUG

    QStringList loaded;
    bool value{false};

    /// Checkeamos si el plugin deseado esta o no cargado
    for (int i = 0; i < g_plugins->pluginsLoaded().size(); ++i)
    {
        /// Convertimos el nombre en un QFileInfo para poder saber el nombre real, ignorando si es un .dll o un .so (deteccion multiplataforma)
        QFileInfo stringplugin(g_plugins->pluginsLoaded().at(i)->fileName());
        loaded << stringplugin.baseName();
        loaded.removeDuplicates();
    }

    /// Si existe el plugin, devuelve verdadero
    if ( loaded.contains(theplugin) )
    {
        value = true;
    }

    return value;
}


bool BlUpgradePreview::unloadPreview(const QString &theplugin)
{
    BL_FUNC_DEBUG

    /// Metodo para saber si debemos o no cargar el preview
    bool value{true};

    /// Si no existe el plugin...
    if (!detectPlugins(theplugin))
    {
        /// Y esta activada la preview...
        if ( (g_confpr->value( CONF_PREVIEW )) == "TRUE")
        {
            /// Devuelve false
            value = false;
        }
    }
    return value;
}

