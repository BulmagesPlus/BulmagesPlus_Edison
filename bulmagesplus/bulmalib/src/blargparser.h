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

#ifndef BLARGPARSER_H
#define BLARGPARSER_H

#include <QString>
#include "bldefs.h"


class BL_EXPORT BlArgParser
{
  private:
    QString m_dbName{};
    QString m_executable{};
    QString m_host{};
    QString m_port{};
    QString m_userName{};
    QString m_confReplaceString{};
    bool m_askPassword{false};
    bool m_showVersion{false};
    bool m_showHelp{false};

  public:
    BlArgParser(int argc, char **argv);
    const QString & dbName() const;
    const QString & host() const;
    const QString & port() const;
    const QString & userName() const;
    const QString & confReplaceString() const;
    const bool & askPassword() const;
    const bool & showVersion() const;
    const bool & showHelp() const;
};


inline const QString & BlArgParser::confReplaceString() const
{
  return m_confReplaceString;
}


inline const QString & BlArgParser::dbName() const
{
  return m_dbName;
}


inline const QString & BlArgParser::host() const
{
  return m_host;
}


inline const QString & BlArgParser::port() const
{
  return m_port;
}


inline const QString & BlArgParser::userName() const
{
  return m_userName;
}


inline const bool & BlArgParser::askPassword() const
{
  return m_askPassword;
}

#endif
