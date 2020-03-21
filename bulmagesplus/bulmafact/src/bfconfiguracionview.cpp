/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
 *   Copyright (C) 2006 by Fco. Javier M. C. (Porting to QT4)              *
 *   fcojavmc@todo-redes.com                                               *
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

#include <QFileDialog>
#include <QMessageBox>
#include <QWidget>
#include <QComboBox>
#include <QToolButton>
#include <QTextStream>
#include <QLayout>
#include <QMessageBox>
#include <QFile>

#include "bfconfiguracionview.h"
#include "bfcompany.h"
#include "blconfiguration.h"
#include "bldatesearch.h"
#include "blfunctions.h"


/** La ventana se construye como derivada de Ficha, inicializa el subformulario  y mete la ventana en el visor de ventanas
*/
/**
\param comp
\param parent
**/
BfConfiguracionView::BfConfiguracionView ( BfCompany *company, QWidget *parent ) : BfForm ( company, parent )
{
    BL_FUNC_DEBUG
    setupUi ( this );

    
    setTitleName ( _ ( "Configuracion" ) );
    setDbTableName ( "configuracion" );
    setDbFieldId ( "nombre" );

    this->setAttribute ( Qt::WA_DeleteOnClose );
    mui_listado->setMainCompany ( mainCompany() );
    mui_listado->load();
    if (g_confpr->value(CONF_MODO_EXPERTO) != "TRUE") {
     tabWidget->removeTab(tabWidget->indexOf(tab_2));
    } // end if


    m_telefono->setText("--");
    m_fax->setText("--");
    m_mail->setText("--");
    m_web->setText("--");
    m_ciudad->setText("--");
    m_codArticuloGenerico->setText("--");
    m_serieFacturaDefecto->setText("--");
    m_irpf->setValue(0.00);
    m_almacenDefecto->setText("--");


    QString query = "SELECT * FROM configuracion WHERE nombre = 'NombreEmpresa'";
    BlDbRecordSet *cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_nombre->setText(cur->value("valor"));
    } // end if
    delete cur;

    query = "SELECT * FROM configuracion WHERE nombre = 'CIF'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_cif->setText(cur->value("valor"));
    } // end if
    delete cur;
    
    query = "SELECT * FROM configuracion WHERE nombre = 'DireccionCompleta'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_direccion->setText(cur->value("valor"));
    } // end if
    delete cur;

    query = "SELECT * FROM configuracion WHERE nombre = 'Municipio'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_municipio->setText(cur->value("valor"));
    } // end if
    delete cur;

    query = "SELECT * FROM configuracion WHERE nombre = 'CodPostal'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_cp->setText(cur->value("valor"));
    } // end if
    delete cur;

    query = "SELECT * FROM configuracion WHERE nombre = 'Provincia'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_provincia->setText(cur->value("valor"));
    } // end if
    delete cur;

    query = "SELECT * FROM configuracion WHERE nombre = 'Pais'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_pais->setText(cur->value("valor"));
    } // end if
    delete cur;

    query = "SELECT * FROM configuracion WHERE nombre = 'Telefono'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_telefono->setText(cur->value("valor"));
    } // end if
    delete cur;

    query = "SELECT * FROM configuracion WHERE nombre = 'Fax'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_fax->setText(cur->value("valor"));
    } // end if
    delete cur;
    
    query = "SELECT * FROM configuracion WHERE nombre = 'Email'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_mail->setText(cur->value("valor"));
    } // end if
    delete cur;
    
    query = "SELECT * FROM configuracion WHERE nombre = 'Web'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_web->setText(cur->value("valor"));
    } // end if
    delete cur;
   
    query = "SELECT * FROM configuracion WHERE nombre = 'Ciudad'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_ciudad->setText(cur->value("valor"));
    } // end if
    delete cur;
    
    query = "SELECT * FROM configuracion WHERE nombre = 'CodArticuloGenerico'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_codArticuloGenerico->setText(cur->value("valor"));
    } // end if
    delete cur;
    
    query = "SELECT * FROM configuracion WHERE nombre = 'SerieFacturaDefecto'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_serieFacturaDefecto->setText(cur->value("valor"));
    } // end if
    delete cur;

    query = "SELECT * FROM configuracion WHERE nombre = 'IRPF'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_irpf->setValue(cur->value("valor").toDouble());
    } // end if
    delete cur;

    query = "SELECT * FROM configuracion WHERE nombre = 'AlmacenDefecto'";
    cur = mainCompany() ->loadQuery ( query );
    if (!cur->eof()) {
      m_almacenDefecto->setText(cur->value("valor"));
    } // end if
    delete cur;

    int res = g_plugins->run ( "BfConfiguracionView_BfConfiguracionView_Post", this );
    if ( res != 0 ) {
          return;
    } // end if

    /// Muestra imagen 'logo.jpg' del directorio 'openreports' en mui_logoEmpresa.
    mui_logoEmpresa->load_picture(g_confpr->value(CONF_DIR_OPENREPORTS) + "logo.jpg");


    /// WIP
    tabWidget->removeTab(tabWidget->indexOf(tab_3));


    insertWindow ( windowTitle(), this, false );
    
}


/** La destruccion de la clase no requiere de acciones especificas
*/
/**
**/
BfConfiguracionView::~BfConfiguracionView()
{
    BL_FUNC_DEBUG
    
}


void BfConfiguracionView::on_mui_aceptar_clicked()
{
  if (saveConf() != 0)
  {
    blMsgError(_("Error inesperado al guardar"));
  } else {
    close();
  } // end if
}


int BfConfiguracionView::saveConf()
{
    mainCompany()->begin();

    int res;

    res = g_plugins->run ( "BfConfiguracionView_saveConf_Pre", this );
    if ( res != 0 ) {
          mainCompany()->rollback();
          return -1;
    } // end if

    mui_listado->save();

	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='NombreEmpresa'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('NombreEmpresa','"+ mainCompany()->sanearCadena(m_nombre->text())+"')");

	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='CodPostal'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('CodPostal','"+ mainCompany()->sanearCadena(m_cp->text())+"')");
	
	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='CIF'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('CIF','"+ mainCompany()->sanearCadena(m_cif->text())+"')");

	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='DireccionCompleta'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('DireccionCompleta','"+ mainCompany()->sanearCadena(m_direccion->text())+"')");
	
	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='Municipio'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('Municipio','"+ mainCompany()->sanearCadena(m_municipio->text())+"')");

	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='Provincia'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('Provincia','"+ mainCompany()->sanearCadena(m_provincia->text())+"')");
	
	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='Pais'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('Pais','"+ mainCompany()->sanearCadena(m_pais->text())+"')");

	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='Telefono'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('Telefono','"+ mainCompany()->sanearCadena(m_telefono->text())+"')");
	
	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='Fax'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('Fax','"+ mainCompany()->sanearCadena(m_fax->text())+"')");
	
	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='Email'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('Email','"+ mainCompany()->sanearCadena(m_mail->text())+"')");

	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='Web'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('Web','"+ mainCompany()->sanearCadena(m_web->text())+"')");
	
	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='Ciudad'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('Ciudad','"+ mainCompany()->sanearCadena(m_ciudad->text())+"')");
	
	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='CodArticuloGenerico'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('CodArticuloGenerico','"+ mainCompany()->sanearCadena(m_codArticuloGenerico->text())+"')");

	
	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='SerieFacturaDefecto'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('SerieFacturaDefecto','"+ mainCompany()->sanearCadena(m_serieFacturaDefecto->text())+"')");
	
	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='IRPF'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('IRPF','"+ mainCompany()->sanearCadena(QString::number(m_irpf->value()))+"')");
	
	mainCompany()->runQuery("DELETE FROM configuracion WHERE nombre='AlmacenDefecto'");
	mainCompany()->runQuery("INSERT INTO configuracion (nombre,valor) VALUES ('AlmacenDefecto','"+ mainCompany()->sanearCadena(m_almacenDefecto->text())+"')");

    res = g_plugins->run ( "BfConfiguracionView_saveConf_Post", this );
    if ( res != 0 ) {
          mainCompany()->rollback();
          return -1;
    } // end if

    mainCompany()->commit();
    
    return 0;
}


void BfConfiguracionView::on_mui_cambiarLogoEmpresa_clicked()
{
    QPixmap imagenPixMap;

    /// 1) Seleccionar imagen JPG
    QString filter = _("Imagenes JPG") + QString(" (*.jpg *.jpeg)");
    QString selectedFilter = filter;

    QString imagen = QFileDialog::getOpenFileName (
                          this,
                          _ ( "Seleccione un archivo de imagen" ),
                          "",
                          filter, &selectedFilter);

    /// Comprueba si se ha seleccionado un archivo.
    if (!imagen.isNull())
    {
        /// Comprueba que la imagen del archivo es valida.
        if (imagenPixMap.load(imagen) == false)
        {
            blMsgError(_("No se ha podido cargar la imagen.\nCompruebe que la imagen sea valida."));
            return;
        } // end if

        /// 2) Actualizar imagen en mui_logoEmpresa
        mui_logoEmpresa->setPixmap(imagenPixMap);

        /// 3) Sobreescribir 'logo.jpg' del directorio 'openreports'.
        mui_logoEmpresa->save_picture(g_confpr->value(CONF_DIR_OPENREPORTS) + "logo.jpg", "JPG", 100);
    } // end if
}



/// ===================================== SUBFORMULARIO ===============================================
/** Inicializacion del subformulario donde se indican todos los campos que deben aparecer
*/
///
/**
\param parent
**/
BfConfiguracionSubForm::BfConfiguracionSubForm ( QWidget *parent ) : BfSubForm ( parent )
{
    BL_FUNC_DEBUG
    setDbTableName ( "configuracion" );
    setDbFieldId ( "nombre" );
    addSubFormHeader ( "nombreorig", BlDbField::DbVarChar, BlDbField::DbDupPrimaryKey | BlDbField::DbNoSave, BlSubFormHeader::DbHideView | BlSubFormHeader::DbDisableView, "nombre" );
    addSubFormHeader ( "nombre", BlDbField::DbVarChar, BlDbField::DbNotNull, BlSubFormHeader::DbNoWrite, _ ( "Nombre" ) );
    addSubFormHeader ( "valor", BlDbField::DbVarChar, BlDbField::DbNothing, BlSubFormHeader::DbNone, _ ( "Valor" ) );
    setInsert ( false );
    setDelete ( false );
    
}

