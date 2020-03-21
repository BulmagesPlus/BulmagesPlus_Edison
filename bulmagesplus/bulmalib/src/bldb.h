/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borrás Riera.                             *
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

#ifndef BLDB_H
#define BLDB_H

#include <QString>
#include <QList>
#include "blpostgresqlclient.h"
#include "blfunctions.h"
#include "blwidget.h"


/// Manejo de campos de un registro de la base de datos
/**
Describe el comportamiento programado de un campo de registro de base de datos.
Se usa en conjunción con BlDbRecord y suele ser invocada mediante el metodo \ref addDbField de esta.
Almacena toda la información relativa a un campo de un registro de base de datos, nombre, valor y
comportamiento.
También prepara el campo para ser almacenado en la base de datos aunque el guardado real lo hace la
clase \ref BlDbRecord
\todo Esta clase debería derivar de \ref BlMainCompanyPointer
*/
class BL_EXPORT BlDbField
{
public:
    /**
    Indica el tipo de dato al que pertenece el campo.
    - DbInt     = Entero.
    - DbVarChar = Cadena de texto de longitud indeterminada.
    - DbDate    = Fecha en formato Europeo (dd/mm/yyyy).
    - DbNumeric = Número en punto fijo.
    - DbBoolean = Valor Booleano (true / false) (t / f).
    - DbTime    = Hora.
    */
    enum DbType {DbInt = 1, DbVarChar = 2, DbDate = 3, DbNumeric = 4, DbBoolean = 5, DbTime = 6};
    /** Indica las restricciones que soporta el dato.
    - DbNothing           = Ninguna.
    - DbNotNull           = No puede ser nulo.
    - DbPrimaryKey        = Es clave primaria.
    - DbNoSave            = No se debe almacenenar en la base de datos.
    - DbAuto              = Es autonumérico.
    - DbDupPrimaryKey     = Duplicado de la clave primaria (útil para aquellos casos en que la clave principal
                            puede ser modificada). En este caso el valor del campo comentario es el de la llave
                            a la que duplica.
    - DbRequired          = Requerido.
    - DbNoLoad            = No debe cargarse desde la base de datos.
    - DbUnique            = La clave deve ser única en su grupo. Se usa en los subformularios para indicar que no
                            puede haber campos repetidos.
    - DbNotNullAutoFilled = No puede ser nulo pero si no se rellena el programa calculará y asignará su valor.
    **/
    enum DbRestrict {DbNothing  =  0, DbNotNull =  1, DbPrimaryKey    =   2,
                     DbNoSave   =  4, DbAuto    =  8, DbDupPrimaryKey =  16,
                     DbRequired = 32, DbNoLoad  = 64, DbUnique        = 128,
                     DbNotNullAutoFilled = 256};

private:
    QString m_fieldName;                         /// El nombre del campo.
    QString m_fieldValue;                        /// El valor actual del campo, en el registro, no en la base de datos.
    QString m_fieldTitle;                        /// El nombre que se mostrara en los mensajes de error y presentación.
    int     m_fieldRestrictions;                 /// Las restricciones del campo.
    DbType  m_fieldType;                         /// El tipo de dato almacenado.
    QString m_fieldPreviousValue;                /// Indica el valor del campo en un estado anterior para determinar si
                                                 ///   ha habido cambios.
    BlPostgreSqlClient *m_dbConnection{nullptr}; /// Puntero a la base de datos a utilizar.

public:
    BlDbField(BlPostgreSqlClient * dbConnection, QString fieldName, DbType fieldType, int fieldRestrictions, QString fieldTitle);
    virtual ~BlDbField();

    void setDbConnection(BlPostgreSqlClient * dbConnection);
    BlPostgreSqlClient *dbConnection();
    QString fieldName();
    DbType fieldType();
    int fieldRestrictions();
    QString fieldTitle();
    virtual int set(QString val);
    QString fieldValue();
    bool isFieldChanged();
    void resetCambio();
    QString valorcampoprep(int &error);
    virtual QString exportXML();
    virtual void syncXML(const QString &);
};


/// Manejo de registros de la base de datos
/**
Sirve para operar de forma sencilla con registros de la base de datos.
Antes de ser utilizada debe inicializarse correctamente con la base de datos que
debe utilizar y la tabla con la que va a operar. Ademas debe describirse como
es la tabla con que se va a operar mediante \ref addDbField
El uso normal de esta clase es la carga de un registro, modificacion de sus campos
y almacenado del mismo.
\todo Esta clase deberia derivar de \ref BlMainCompanyPointer
*/
class BL_EXPORT BlDbRecord
{
protected:
    QList<BlDbField *> m_lista;             /// Lista de campos que conforman la tabla de la BD.
    QString m_tableName;                    /// Nombre de la tabla por defecto que se utiliza.
    QString m_campoid;                      /// Nombre del campo identificador en la tabla.
    bool    m_nuevoCampo;                   /// Indicador sobre si es un nuevo registro o un registro modificado.
    QMap<QString,QString> m_variables;      /// Son elementos que han sido incrustados e inicializados por programas
                                            ///   externos, generalmente el RML.
    QMap<QString,QString> m_globalvars;     /// Son elementos que han sido incrustados e inicializados por programas
                                            ///   externos, generalmente el RML.
    BlMainCompany *m_dbConnection{nullptr}; /// Puntero a la base de datos con la que se opera.

public:
    explicit BlDbRecord(BlMainCompany * mainCompany);
    virtual ~BlDbRecord();
    void setDbConnection(BlMainCompany * mainCompany);
    BlMainCompany *dbConnection();
    int DBload(BlDbRecordSet *);
    virtual int dbSave(QString & id);
    virtual int setDbValue(QString, QString);
    virtual int setDbValueNextToLast(QString nomb, int incr, QString cond = "");
    QString dbValue (QString);
    bool exists(QString);
    QString dbValueprep(QString);
    void setDbTableName(QString tableName);
    void setNuevo(bool n);
    QString tableName();
    QString fieldId();
    void setDbFieldId(QString nom);
    int addDbField(QString, BlDbField::DbType, int, QString);
    void DBclear();
    QList<BlDbField *> * lista();
    virtual int remove();
    virtual int save();
    virtual void vaciar();
    virtual void imprimir();
    virtual void imprimir(const QString & templateFile);
    virtual int load(QString);
    virtual int parseTags(QString & buff, int tipoEscape = 0);
    virtual QString story(void);
    virtual void substrConf(QString & buff);
    virtual void substrConf(QByteArray & buff);
    virtual QString templateName(void);
    virtual int generateRML(void);
    virtual int generateRML(const QString &);
    virtual QString exportXML();
    virtual void syncXML(const QString &);

    //===========
    virtual int parseTags(QByteArray & buff, int tipoEscape = 0);
    virtual int parseTagsPost ( QByteArray & buff, int tipoEscape = 0);
    virtual QByteArray parseIncludeFileTXT(const QString & file, int tipoEscape = 0);
    virtual QByteArray parseIncludeImg(const QString & file, int tipoEscape = 0);
    virtual QByteArray parsePngRaw64(const QByteArray & data, int tipoEscape = 0);
    virtual QByteArray parseSetCharacterPrintMode(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetCharacterSpacing(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetCharacterCodeTable(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetUnderlineMode(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetCharacterSize(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetSmoothing(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetDoubleStrike(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseTurnUpsideDown(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseTurn90CWRotation(const QString & param, const QString &param1, int tipoEscape = 0);
    virtual QByteArray parseQuery(const QString & query, const QByteArray &datos, int tipoEscape = 0);
    virtual QByteArray parseTurnWhiteBlack(const QString &param, int tipoEscape = 0);
    virtual QByteArray parseSetColor(const QString &param, int tipoEscape = 0);
    virtual QByteArray parseHorizontalTab(int tipoEscape = 0);
    virtual QByteArray parseSetHorizontalTabPos(const QString & param, const QString & param1, int tipoEscape = 0);
    virtual QByteArray parseSetLeftMargin(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetPrintingAreaWidth(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetJustification(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetHAbsolutePos(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetHRelativePos(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetBarcodeFormat(const QString & param, const QString & param1, const QString & param2, const QString & param3, int tipoEscape = 0);
    virtual QByteArray parsePrintBarCode(const QString & param, const QString & param1, const QString & param2, int tipoEscape = 0);
    virtual QByteArray parseSetBarCodeWidth(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetBarCodeHeight(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSelectPageMode(int tipoEscape = 0);
    virtual QByteArray parseSetPrintArea(const QString & param, const QString & param1, const QString & param2, const QString & param3, int tipoEscape = 0);
    virtual QByteArray parseSetPrintDirection(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetVAbsolutePos(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseSetVRelativePos(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseCutPaper(const QString & param, int tipoEscape = 0);
    virtual QByteArray parseCutPaperAndFeed(const QString & param,  const QString & param1, int tipoEscape = 0 );
    virtual QByteArray parseRightJustified(const QString & param, const QString & param1, const QString & param2, const QString & param3, int tipoEscape = 0);
    virtual QByteArray parseLeftJustified(const QString & param, const QString & param1, const QString & param2, const QString & param3, int tipoEscape = 0);
    QByteArray parseRecordset(BlDbRecordSet * cur, const QByteArray & datos, int tipoEscape = 0);
    virtual QByteArray parseIfQuery(const QString & query, const QByteArray & datos);
    virtual QByteArray parseIf(const QString & query, const QByteArray & datos, const QByteArray & datos1);
    virtual QByteArray parseExists(const QString & query, const QByteArray & datos);
    void substrVars(QByteArray & buff, int tipoEscape = 0);
    void setVar(const QString & varname, const QString & varvalue);
    void clearVars();
};

#endif

