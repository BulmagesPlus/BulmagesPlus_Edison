/***************************************************************************
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

#include <QDebug>
#include <QTimer>
#include <QScriptEngine>
#include "blscriptglobalobject.h"


BlScriptGlobalObject::BlScriptGlobalObject(QScriptEngine *scriptEngine)
{
  m_scriptEngine = scriptEngine;
}


BlScriptGlobalObject::~BlScriptGlobalObject()
{
  QListIterator<QPair<QString, QWidget*>*> i(m_widgetList);
  while (i.hasNext())
  {
    QPair<QString, QWidget*> *pair = i.next();
 
    QString type = pair->first;

    if (type == QString("QToolButton"))
    {
        QToolButton *obj = dynamic_cast<QToolButton *>(pair->second);
        delete obj;
    } // end if

  } // end while
  
  m_widgetList.clear();
  
}


void BlScriptGlobalObject::setTimeout(QScriptValue fn, int milliseconds)
{
  if (fn.isFunction())
  {
    QTimer *timer = new QTimer(0);
    qScriptConnect(timer, SIGNAL(timeout()), QScriptValue(), fn);
    connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));
    timer->setSingleShot(true);
    timer->start(milliseconds);
  } else {
    context()->throwError(tr("Passed parameter '%1' is not a function.").arg(fn.toString()));
  } // end if
}


void BlScriptGlobalObject::msgInfo(QString s)
{
    blMsgInfo(s);
}


QScriptValue BlScriptGlobalObject::newWidget(QString type, QScriptValue parentObject)
{
    bool finded = false;
    QScriptValue result;
    
    QWidget *parent = 0;
    if (parentObject.isObject())
    {
      parent = dynamic_cast<QWidget *>(parentObject.toQObject());
    } // end if


    QPair<QString, QWidget*> *pair = new QPair<QString, QWidget*>;

    if (type == QString("QToolButton"))
    {
      finded = true;
      QToolButton *obj = new QToolButton(parent);
      /// Default setting
      obj->resize(32, 32);
      obj->setVisible(true);
      result = m_scriptEngine->newQObject(obj);
      
      pair->first = type;
      pair->second = dynamic_cast<QWidget *>(obj);
    } // end if
    
    
    if (finded)
    {
      m_widgetList.push_back(pair);
    } // end if

    return result;
}

