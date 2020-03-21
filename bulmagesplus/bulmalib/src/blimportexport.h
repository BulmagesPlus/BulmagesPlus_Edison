/***************************************************************************
 *   Copyright (C) 2003 by Santiago Capel Torres.                          *
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

#ifndef BLIMPORTEXPORT_H
#define BLIMPORTEXPORT_H

#include <QFile>
#include <QString>

#include "blpostgresqlclient.h"
#include "blfunctions.h"

#define IMPORT_TODO 0xFFFFFFFF

/// Tipos para BulmaCont:
#define IMPORT_CUENTAS                   1
#define IMPORT_TIPOSIVA                  2
#define IMPORT_ASIENTOS                  4
#define IMPORT_AINTELIGENTES             8
#define IMPORT_BALANCES                 16
#define IMPORT_COBROS                   32
#define IMPORT_FACTURAS                 64

/// Tipos para BulmaFact:
#define IMPORT_FAMILIAS                  1
#define IMPORT_ALBARANESCLIENTE          2
#define IMPORT_CLIENTES                128
#define IMPORT_PROVEEDORES             256
#define IMPORT_ARTICULOS               512
#define IMPORT_FACTURASCLIENTE        1024
#define IMPORT_ALMACENES              2048
#define IMPORT_TRABAJADORES           4096
#define IMPORT_FORMAS_PAGO            8192
#define IMPORT_PRESUPUESTOSCLIENTE   16384
#define IMPORT_PEDIDOSCLIENTE            4


/// Clase para importaci&oacute;n y exportaci&oacute;n a distintos formatos de archivo de datos.
/** */
class BL_EXPORT BlImportExport
{
private:
    /// Base de datos con la que trabaja la clase y de la que se hace
    /// importaci&oacute;n/exportaci&oacute;n.
    BlPostgreSqlClient *dbConnection;
    /// La clase puede hacer una simulaci&oacute;n o no dependiendo del valor de esta
    /// variable.
    bool m_modoTest;
    /// Las importaciones y exportaciones pueden ser entre dos fechas, m_fInicial indica la
    /// fecha inicial a partir de la que hacer la importaci&oacute;n.
    QString m_fInicial;
    /// Las importaciones y exportaciones pueden ser entre dos fechas, m_fFinal indica la
    /// fecha final a partir de la que hacer la importaci&oacute;n.
    QString m_fFinal;

public:
    /// Para que la clase pueda emitir el estado de completitud se inicializa con un puntero
    /// a funci&oacute;n.
    virtual void alerta ( int, int );
    /// Para que la clase pueda emitir mensajes de error o de alerta se inicializa con un
    /// puntero de funci&oacute;n.
    virtual void mensajeria ( QString );
    void setFInicial ( QString f );
    void setFFinal ( QString f );
    void setModoTest();
    void setModoNormal();
    bool modoTest();
    BlImportExport ( BlPostgreSqlClient * );
    virtual ~BlImportExport();
    int contaplus2Bulmages ( QFile &, QFile & );
    int bulmages2Contaplus ( QFile &, QFile & );
    /// Esta funci&oacute;n pasa datos de una empresa al formato XML.
    int bulmages2XML ( QFile &, unsigned long long int tipo = IMPORT_TODO );
    int bulmafact2XML ( QFile &, unsigned long long int tipo = IMPORT_TODO );
    /// Esta funci&oacute;n pasa datos de XML a bulmag&eacute;s.
    int XML2Bulmages ( QFile &, unsigned long long int tip = IMPORT_TODO );
    int XML2BulmaFact ( QFile &, unsigned long long int tip = IMPORT_TODO );
    QString searchParent ( QString );
};

#endif

