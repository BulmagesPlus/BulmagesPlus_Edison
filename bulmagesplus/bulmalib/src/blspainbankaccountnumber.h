/***************************************************************************
 *   Copyright (C) 2004 by David Gutiérrez Rubio.                          *
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

#ifndef BLSPAINBANKACCOUNTNUMBER_H
#define BLSPAINBANKACCOUNTNUMBER_H

#include <QString>
#include "bldefs.h"

/// Esta clase implementa el C&oacute;digo Cuenta Cliente (CCC) de un banco.
/** Su funci&oacute;n m&aacute;s importante es 'cuentaesvalida'. */
class BL_EXPORT BlSpainBankAccountNumber
{
public:
    /// Banco, entidad, dc y numcuenta.
    BlSpainBankAccountNumber ( QString, QString, QString, QString );
    /// Banco, entidad y numcuenta. El dc lo calcula autom&aacute;ticamente.
    BlSpainBankAccountNumber ( QString, QString, QString );
    /// Banco, entidad, dc y numcuenta todo seguido, con guiones opcionales.
    BlSpainBankAccountNumber ( QString );
    ~BlSpainBankAccountNumber();
    /// Calcula y devuelve los digitos de control del ccc.
    QString getdc();
    /// Comprueba si es un numero de cuenta valido.
    /// Devuelve TRUE si es un CCC correcto. FALSE en caso contrario.
    bool cuentaesvalida();
    /// Devuelve el CCC separando sus miembros por la cadena \c sep.
    QString getcodigo ( QString sep = "" );
    /// Devuelve el n&uacute;mero de cuenta.
    QString getnumcuenta();
    /// Devuelve el n&uacute;mero de banco.
    QString getbanco();
    /// Devuelve el n&uacute;mero de entidad.
    QString getentidad();
    /// Fija el n&uacute;mero de cuenta.
    /// Si el argumento tiene menos de 10 d&iacute;gitos, se rellena con ceros a la izquierda.
    void setnumcuenta ( QString cad );
    /// Fija el n&uacute;mero de banco.
    /// Si el argumento tiene menos de 4 d&iacute;gitos, se rellena con ceros a la izquierda.
    void setbanco ( QString cad );
    /// Fija el n&uacute;mero de entidad.
    /// Si el argumento tiene menos de 4 d&iacute;gitos, se rellena con ceros a la izquierda.
    void setentidad ( QString cad );

private:
    QString banco;
    QString entidad;
    QString dc;
    QString numcuenta;
};

#endif

