/***************************************************************************
 *   Copyright (C) 2009 by Aron Galdón.                                    *
 *   Copyright (C) 2018 by Fco. Javier Miró Carmona.                       *
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

#include "blargparser.h"
#include "blconfiguration.h"
#include "blfunctions.h"


///
/**
\param argc
\param argv
**/
BlArgParser::BlArgParser(int argc, char **argv)
{
   BL_FUNC_DEBUG

   m_executable = argv[0];
   QString argument{};
   char type_arg{' '};

   for (int i{1}; i < argc; i++)
   {
      argument = argv[i];

      /// Usa como un valor si la iteración anterior ha activado un 'type_arg'.
      switch (type_arg)
      {
        case 'd':
          m_dbName = argument;
          break;

        case 'h':
          m_host = argument;
          break;

        case 'p':
          /// Captura el número de puerto sólo si es un número.
          {
            bool ok{false};
            argument.toInt(&ok, 10);
            if (ok) m_port = argument;
          }
          break;

        case 'U':
          m_userName = argument;
#ifdef Q_OS_WIN32
          QString sys_user = qgetenv("USERNAME");
#else
          QString sys_user = qgetenv("USER");
#endif
          if (m_userName.compare(sys_user) != 0)
          {
            /// Si el usuario indicado no coincide con el que ha lanzado el programa, preguntar la contraseña aunque no se indique "-W"
            m_askPassword = true;
          } // end if
          break;
      } // end switch

      /// Cuando se asigna una letra a la variable 'type_arg',
      /// se espera capturar un valor en la próxima iteración.
      type_arg = ' ';

      if (argument == "--dbname" || argument == "-d")
      {
         type_arg = 'd';

      } else if (argument == "--host" || argument == "-h")
      {
         type_arg = 'h';

      } else if (argument == "--port" || argument == "-p")
      {
         type_arg = 'p';

      } else if (argument == "--username" || argument == "-U")
      {
         type_arg = 'U';

      } else if (argument == "--replace" || argument == "-R")
      {
         /// Usa la variable de entorno CONF_REPLACE_STRING, si existe, para reemplazar el valor.
         m_confReplaceString = blGetEnv("CONF_REPLACE_STRING");

      } else if (argument == "--version" || argument == "-V")
      {
         m_showVersion = true;

      } else if (argument == "--password" || argument == "-W")
      {
         m_askPassword = true;

      } else if (argument == "--help" || argument == "-?")
      {
         m_showHelp = true;
      } // end if
   } // end for
}


/// Muestra texto de ayuda y devuelve 'true' si se ha invocado.
/**
**/
const bool & BlArgParser::showHelp() const
{
   BL_FUNC_DEBUG

   if (m_showHelp)
   {
      QTextStream(stdout)
            << "Usage: " << m_executable << " [OPTION]" << endl
            << "-d, --dbname NAME     Database name" << endl
            << "-h, --host ADDRESS    Server name or IP" << endl
            << "-p, --port PORT       Port number" << endl
            << "-U, --username NAME   User name" << endl
            << "-W, --password        Force password asking" << endl
            << "-V, --version         Show current version number, then exit" << endl
            << "-R, --replace         Use CONF_REPLACE_STRING environment variable" << endl
            << "                      to replace ${CONF_REPLACE_STRING} in .conf files" << endl
            << "-?, --help            Show this help, then exit" << endl;
   } // end if

   return m_showHelp;
}


/// Muestra la versión y devuelve 'true' si se ha invocado.
/**
**/
const bool & BlArgParser::showVersion() const
{
   BL_FUNC_DEBUG

   if (m_showVersion || m_showHelp)
   {
      QTextStream(stdout) << m_executable << " " << CONFIG_VERSION << endl;
   } // end if

   return m_showVersion;
}

