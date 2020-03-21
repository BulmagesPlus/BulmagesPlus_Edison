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

/// Fichero de definiciones de las clases 'BlDbRecordSet' y 'BlPostgreSqlClient'.
/// Definen la interficie de comunicacion con el sistema motor de bases de datos.
/// Estas clases se basan en la clase libpq.
#ifndef BLPOSTGRESQLCLIENT_H
#define BLPOSTGRESQLCLIENT_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>

#ifdef DISTRO_DEBIAN
#include <postgresql/libpq-fe.h>
#elif DISTRO_DEBIAN_8_0
#include <postgresql/8.0/libpq-fe.h>
#elif  DISTRO_RED_HAT
#include <pgsql/libpq-fe.h>
#elif  DISTRO_GENTOO
#include <postgresql/pgsql/libpq-fe.h>
#elif  DISTRO_NO_SE_QUE
#include <no_se_que/pgsql/libpq-fe.h>
#else
#include <libpq-fe.h>
#endif

#include "blconfiguration.h"
#include "blfunctions.h"


/// Esta clase provee toda la interacci&oacute;n necesaria para las consultas de base de datos.
/** Se carga normalmente a partir de la clase 'BlPostgreSqlClient' con el m&eacute;todo 'loadQuery'.
    Crea las estructuras necesarias para recorrer un recordset de postgres. */
class BL_EXPORT BlDbRecordSet
{
private:
    /// El nombre del cursor, (OBSOLETE).
    QString nomcursor;
    /// Estructura en la que se almacena el resultado.
    PGresult *result;
    /// Estructura en la que se almacenan los datos de conexi&oacute;n.
    PGconn *conn;
    /// Para recorrer una consulta (query) simulamos una cola, esta variable global indica el
    /// registro en que nos encontramos situados.
    int registroactual;
    /// Indica el n&uacute;mero de registros que tiene la consulta. (La componente vertical del query).
    int nregistros;
    /// Indica el n&uacute;mero de campos que tiene la consulta. (La componente horizontal del query).
    int ncampos;
    bool m_error;
    QString m_query;
    QString m_pristineQuery;

    QHash<QString, int> m_campos;

    void inicializa ( QString nombre, PGconn *conn1, QString SQLQuery, int numParams,
                       const char * const *paramValues, QString pristineQuery = NULL
                       );
public:
    /// Constructor, inicializa la estructura y realiza la consulta.
    BlDbRecordSet ( QString nombre, PGconn *conn1, QString SQLQuery, QString pristineQuery=NULL );

    /**
     * Constructor para consultas parametrizadas (menos escapes y formateos, 
     * algo más de seguridad y dejamos puertas abiertas para futuras optimizaciones
     * con consultas precompiladas)
     * @param nombre nombre para el cursor
     * @param conn1 conexión de BD
     * @param SQLQuery sentencia SQL a enviar a base de datos. Contendrá tantos 
     *                  $1, $2, etc. como numParams. Pueden incluir sufijo con el
     *                   tipo de datos psql, como $1::int4, $2::varchar etc.
     * @param numParams número de parámetros ($) en la consulta y número de elementos de 
     *                  paramValues.
     * @param paramValues Valores para los parámetros. Sólo aceptamos formato texto
     *                    no el formato binario
     */
     BlDbRecordSet ( QString nombre, PGconn *conn1, QString SQLQuery, int numParams,
                       const QString *paramValues, QString pristineQuery=NULL
                       ) ;
  

    /**
     * Constructor para consultas parametrizadas (menos escapes y formateos, 
     * algo más de seguridad y dejamos puertas abiertas para futuras optimizaciones
     * con consultas precompiladas)
     * @param nombre nombre para el cursor
     * @param conn1 conexión de BD
     * @param SQLQuery sentencia SQL a enviar a base de datos. Contendrá tantos 
     *                  $1, $2, etc. como numParams. Pueden incluir sufijo con el
     *                   tipo de datos psql, como $1::int4, $2::varchar etc.
     * @param numParams número de parámetros ($) en la consulta y número de elementos de 
     *                  paramValues.
     * @param paramValues Valores para los parámetros. Sólo aceptamos formato texto
     *                    no el formato binario. Sólo los leemos, no los destruimos.
     */
    BlDbRecordSet ( QString nombre, PGconn *conn1, QString SQLQuery, int numParams,
                       const char *const * const paramValues, QString pristineQuery=NULL);

    /// Destructor, elimina la memoria ocupada.
    ~BlDbRecordSet();
    /// Devuelve el n&uacute;mero de registros del cursor.
    int numregistros();
    /// Devuelve el valor de una determinada posici&oacute;n del query.
    QString value( int posicion, int registro = -1, bool localeformat = false );
    /// Devuelve el tipo de dato de una determinada posicion de un query
    int fieldType(int posicion);
    bool error();
    QString query();
    QString pristineQuery();

    /// Esta funci&oacute;n devuelve el valor entero del campo posicion del registro
    /// pasado, o siNull si el campo es null .
    int valorInt ( int posicion, int registro = -1, int siNull = 0);

    /// Esta funci&oacute;n devuelve el valor entero del campo especificado 
    /// (por nombre) del registro pasado, o siNull en caso que el campo sea null.
    int valorInt ( const QString &campo, int registro=-1 , int siNull = 0);

public:
    /// Devuelve el valor de una determinada posici&oacute;n del query.
    QString value( const QString &campo, int registro = -1, bool localeformat = false );
    /// Avanza el puntero de recorrido en forma de cola una posici&oacute;n.
    int nextRecord();
    /// Retrocede el puntero de recorrido en forma de cola una posici&oacute;n.
    int previousRecord();
    /// Establece la posici&oacute;n del puntero de recorrido de cola al primer registro.
    int firstRecord();
    /// Establece la posici&oacute;n del puntero de recorrido de cola en el &uacute;ltimo registro.
    int lastRecord();
    /// Devuelve la posici&oacute;n del registro actual.
    int currentRecord();
    /// Libera la memoria que se ha ocupado en las consultas.
    void cerrar();
    /// Devuelve el nombre del campo de una columna determinada de la consulta.
    QString fieldName ( int );
    /// Devuelve el n&uacute;mero de columna que tiene una columna referenciada por el nombre. Si no existe devuelve -1
    int numcampo ( const QString & );
    /// Devuelve el n&uacute;mero total de columnas que tiene la consulta.
    int numcampos();
    /// Indica que se ha alcanzado el primer registro de la consulta.
    bool eof();
    /// Indica que se ha alcanzado el &uacute;ltimo registro de la consulta.
    bool bof();
    /// Informa sobre si el registro actual es el &uacute;ltimo registro de la consulta.
    bool isLastRecord();
    /// Informa sobre si el registro actual es el primer registro de la consulta.
    bool isFirstRecord();
};


class BL_EXPORT BlPostgreSqlClient
{
private:
    QString m_pgDbUser = "";
    QString m_pgDbPassword = "";
    QString m_pgDbHost = "";     /// Indica cual es el host de las bases de datos.
    QString m_pgDbPort = "";     /// Indica por que puerto acceder a dicho host.
    QString m_pgDbOptions = "";  /// Indica las opciones especiales que pueda tener la conexion.
    QString m_pgDbTty = "";      /// Indica algunos par&aacute;metros de la conexi&oacute;n (Desgraciadamente no se cuales).
    QString m_pgDbName = "";     /// Indica el nombre de la base de datos con la que se conecta.
    PGconn *conn; /// Representa la conexi&oacute;n.
    bool m_insideTransaction; /// Indica si estamos dentro de una transacci&oacute;n.
    QString m_currentUser;   /// Indica el usuario que se ha conectado.
    /// escapa una cadena per a posar-la a la cadena de connexió a PostgreSQL
    QString &escCadConn(QString t) ;
private:
    /// Establece en la base de datos cual va a ser el formato de fecha a utilizar.
    int formatofecha();

public:
    /// Constructor de la clase que inicializa variables.
    BlPostgreSqlClient();
    /// Destructor de la clase que libera memoria.
    ~BlPostgreSqlClient();
    const QString currentUser();
    /// La clase debe ser inicializada con algunos par&aacute;metros para poder funcionar.
    int inicializa(QString nomdb = "");
    /// Inicia una transacci&oacute;n.
    int begin();
    /// Termina una transacci&oacute;n.
    void commit();
    /// Termina una transacci&oacute;n de forma abrupta.
    void rollback();
    /// Carga el cursor con una consulta.
    BlDbRecordSet *loadQuery ( QString query, QString nomcursor = "", int limit = 0, int offset = 0 );
    BlDbRecordSet *loadQuery ( QString query, int numParams,
                       QString  * paramValues, QString nomcursor = "", 
                       int limit =0, int offset = 0 );
    /// carga el cursor con los valores de parámetros $1, $2 , $3 ... que se pasan como QString al final
    BlDbRecordSet *load ( QString query, QString dolar1="",  QString dolar2="",
                           QString dolar3="", QString dolar4="",  
                           QString dolar5="", QString dolar6="", 
                           QString dolar7="", QString dolar8="",  
                           QString dolar9="", QString dolar10="", 
                           QString dolar11="", QString dolar12="",  
                           QString dolar13="", QString dolar14="", 
                           QString dolar15="", QString dolar16="",  
                           QString dolar17="", QString dolar18="", 
                           QString dolar19="", QString dolar20=""
   );
    int run ( QString query, QString dolar1="",  QString dolar2="",
                           QString dolar3="", QString dolar4="",  
                           QString dolar5="", QString dolar6="", 
                           QString dolar7="", QString dolar8="",  
                           QString dolar9="", QString dolar10="", 
                           QString dolar11="", QString dolar12="",  
                           QString dolar13="", QString dolar14="", 
                           QString dolar15="", QString dolar16="",  
                           QString dolar17="", QString dolar18="", 
                           QString dolar19="", QString dolar20=""
   );

     int run ( QString query,  int numParams, QString params[]);
     int runQuery ( QString query ) ;

     int run ( QString Query,  int numParams, const char * const * params);

    // retorna el número de parámetris de una consulta SQL aproximado (
    // no tiene en cuenta si el $n está dentro de una cadena literal o similar,
    // solo busca subcadenas de la forma $1, $2, etc.)
    int numParams(QString query) ;

 
    /// Busca en una cadena c&oacute;digo malicioso para SQL y lo elimina (previene el SQLInjection).
    static QString sanearCadena ( QString cadena );
    QString sanearCadenaUtf8 ( QString cadena );
    void terminar();
    QString propiedadempresa ( QString );
    /// Returns the parent of a determinated account code.
    QString searchParent ( QString );

    bool setDbUser(QString dbUser);
    bool setDbPassword(QString dbPassword);
    bool setDbName(QString dbName);
    bool setDbHost(QString dbHost);
    bool setDbPort(QString dbPort);

    /// Returns the name of the database opened, if none as open then returns "".
    QString dbName();
    QString dbHost();
    QString dbPort();
    /// Returns table privileges of current user
    bool hasTablePrivilege ( QString table, QString privilege );
    /// Returns database privileges of current user
    bool hasDatabasePrivilege ( QString database, QString privilege );
    /// Returns "true" if exists at least one record that satisfies the condition
    bool existsAnyRecord ( QString table, QString cond = "" );
    /// Evalua expresiones con el motor de calculo de la base de datos
    QString PGEval(QString evalexp, int precision = 2 );

    /// TODO: FUNCIONALIDAD ERRONEA. Devuelve el numero de decimales de un campo 'numeric'. En caso de no ser de tipo 'numeric' devuelve -1.
    int numericPrecision(QString tableName, QString fieldName);
    /// TODO.
    int numericScale(QString tableName, QString fieldName, QString schema = QString("public"));
    
    

    /// It returns as a QStringList with the database tables
    QStringList listTables();
    QList<QHash<QString, QString> > tableArch(QString tablename);

    int fieldCharacterMaximumLength(QString table, QString field);
    bool columnExists(QString tableName, QString columnName, QString schemaName = "public");
};

#endif

