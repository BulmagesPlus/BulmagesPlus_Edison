/****************************************************************************
 **
 ** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
 **
 ** This file is part of the documentation of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License version 2.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of
 ** this file.  Please review the following information to ensure GNU
 ** General Public Licensing requirements will be met:
 ** http://www.trolltech.com/products/qt/opensource.html
 **
 ** If you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://www.trolltech.com/products/qt/licensing.html or contact the
 ** sales department at sales@trolltech.com.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
****************************************************************************/

#ifndef BLMONTHCALENDAR_H
#define BLMONTHCALENDAR_H

#include <QDate>
#include <QWidget>
#include <QTextBrowser>
#include <QMultiHash>
#include <QString>

#include "blfunctions.h"


class BL_EXPORT BLMonthCalendar : public QWidget
{
    Q_OBJECT

public:
    explicit BLMonthCalendar(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
    virtual ~BLMonthCalendar();

public slots:
    void setFontSize(int size);
    void setMonth(int month);
    void setYear(QDate date);

private:
    void insertCalendar();
    int fontSize;
    QDate selectedDate;
    QTextBrowser *editor;
    QMultiHash <QString, QString> m_textos;
};

#endif
