/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
 *   http://www.iglues.org                                                 *
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

#ifndef BFBULMAFACT_H
#define BFBULMAFACT_H

#include <QMdiSubWindow>
#include <QStringList>

#include "blfunctions.h"
#include "blmainwindow.h"
#include "blworkspace.h"
#include "ui_bfbulmafactbase.h"
#include "bfcompany.h"
#include "blwindowlistdock.h"
#include "bfupgradepreview.h"



/** Esta es la clase principal del programa ya que es la que deriva de QMainWindow.
    Su funcionalidad es servir de base como aplicacion Qt y inicializar los componentes clave.
    Tambien sirve de soporte al dise&ntilde;o especificado en bfbulmafactbase.ui con sus menus
    y ToolBoxes.
    Hace todo el traspaso de mensajes de los menus a company y captura algunos que no son
    propiamente de la facturacion como pueda ser el FullScreen o el About.
*/
class BF_EXPORT BfBulmaFact: public BlMainWindow, public Ui_BfBulmaFactBase
{
    Q_OBJECT

protected:
    virtual void resizeEvent(QResizeEvent * event);

private:
    /// El workSpace que se va a usar con la aplicacion.
    BlWorkspace *pWorkspace;
    /// La clase corazon de la aplicacion y centralizadora de mensajes y componentes.
    BfCompany *m_company;
    /// El listado de ventanas abiertas que esta en forma de dock en la parte izquierda.
    BlWindowListDock *m_list;
    
    void *m_update;
    BfUpgradePreview *m_upgradePreview;


public:
    explicit BfBulmaFact(QString bd);
    ~BfBulmaFact();
    BlWorkspace *workspace() const;
    BfCompany *company() const;
    void createMainWindows(BlSplashScreen *);
    QProgressBar *m_pb;
    
    BfUpgradePreview *upgradePreview() const;

public slots:
    virtual void closeEvent(QCloseEvent *event);
    virtual void s_FPago();
    virtual void s_ventanaCompleta();
    virtual void s_About();
//    virtual void on_actionDocumentacion_triggered();
    virtual void on_actionPaises_triggered();
    virtual void on_actionSeries_de_Factura_triggered();
    virtual void on_actionTrabajadores_triggered();
    virtual void on_actionBancos_triggered();
    virtual void on_actionVentana_Completa_triggered();
    virtual void on_actionAcerca_de_triggered();
    virtual void on_actionAlmacenes_triggered();
    virtual void on_actionParametros_triggered();
    virtual void on_actionFormas_de_Pago_triggered();
    virtual void on_actionIndexador_triggered();
    virtual void on_actionTipoIVA_triggered();
    virtual void on_actionModo_Experto_triggered();
    virtual void on_actionTasaIVA_triggered();
    virtual void on_actionOrdenar_Ventanas_triggered();
    virtual void on_actionOrganizaci_n_en_Cascada_triggered();
    virtual void on_actionSalir_triggered();
    virtual void on_actionActualizador_triggered();
    virtual void on_actionBases_de_datos_triggered();
    virtual void on_actionRegistrar_triggered();
    virtual void on_actionSoporte_remoto_triggered();

    void on_actionActualizaciones_triggered();

private slots:
    /// Este SLOT sirve para actualizar el Indexador cuando se activa una ventana
    void informaindexador(QMdiSubWindow *w);
    void setActionIndexador(bool);
    void documentacionError(const QString);
    void supportWindow();

signals:
    void resizeEventCalled();
};

#endif

