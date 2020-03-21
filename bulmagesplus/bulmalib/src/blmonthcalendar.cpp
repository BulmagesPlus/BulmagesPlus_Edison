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

#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QTextFrame>
#include <QTextTableCell>
#include "blmonthcalendar.h"


BLMonthCalendar::BLMonthCalendar ( QWidget * parent, Qt::WindowFlags f ) : QWidget ( parent, f )
{
    selectedDate = QDate::currentDate();
    fontSize = 10;

//        QWidget *centralWidget = new QWidget;

/// Algunas pruebas de insercion de datos de prueba.
    m_textos.insert ( selectedDate.toString(), "La madre\n" );
    m_textos.insert ( selectedDate.toString(), "del cordero feliz.\n" );

    QLabel *dateLabel = new QLabel ( tr ( "Date:" ) );
    QComboBox *monthCombo = new QComboBox;

    for ( int month = 1; month <= 12; ++month )
        monthCombo->addItem ( QDate::longMonthName ( month ) );

    QDateTimeEdit *yearEdit = new QDateTimeEdit;
    yearEdit->setDisplayFormat ( "yyyy" );
    yearEdit->setDateRange ( QDate ( 1753, 1, 1 ), QDate ( 8000, 1, 1 ) );

    monthCombo->setCurrentIndex ( selectedDate.month() - 1 );
    yearEdit->setDate ( selectedDate );

    QLabel *fontSizeLabel = new QLabel ( tr ( "Font size:" ) );
    QSpinBox *fontSizeSpinBox = new QSpinBox;
    fontSizeSpinBox->setRange ( 1, 64 );
    fontSizeSpinBox->setValue ( 10 );

    editor = new QTextBrowser;
    insertCalendar();

    connect ( monthCombo, SIGNAL ( activated ( int ) ), this, SLOT ( setMonth ( int ) ) );
    connect ( yearEdit, SIGNAL ( dateChanged ( QDate ) ), this, SLOT ( setYear ( QDate ) ) );
    connect ( fontSizeSpinBox, SIGNAL ( valueChanged ( int ) ),
              this, SLOT ( setFontSize ( int ) ) );

    QHBoxLayout *controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget ( dateLabel );
    controlsLayout->addWidget ( monthCombo );
    controlsLayout->addWidget ( yearEdit );
    controlsLayout->addSpacing ( 24 );
    controlsLayout->addWidget ( fontSizeLabel );
    controlsLayout->addWidget ( fontSizeSpinBox );
    controlsLayout->addStretch ( 1 );

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addLayout ( controlsLayout );
    centralLayout->addWidget ( editor, 1 );
    setLayout ( centralLayout );

}

BLMonthCalendar::~BLMonthCalendar()
{
}

void BLMonthCalendar::insertCalendar()
{
    editor->clear();
    QTextCursor cursor = editor->textCursor();

    QDate date ( selectedDate.year(), selectedDate.month(), 1 );

    QTextTableFormat tableFormat;
    tableFormat.setAlignment ( Qt::AlignHCenter );
    tableFormat.setBackground ( QColor ( "#e0e0e0" ) );
    tableFormat.setCellPadding ( 2 );
    tableFormat.setCellSpacing ( 4 );
    QVector<QTextLength> constraints;
    constraints << QTextLength ( QTextLength::PercentageLength, 14 )
    << QTextLength ( QTextLength::PercentageLength, 14 )
    << QTextLength ( QTextLength::PercentageLength, 14 )
    << QTextLength ( QTextLength::PercentageLength, 14 )
    << QTextLength ( QTextLength::PercentageLength, 14 )
    << QTextLength ( QTextLength::PercentageLength, 14 )
    << QTextLength ( QTextLength::PercentageLength, 14 );
    tableFormat.setColumnWidthConstraints ( constraints );

    QTextTable *table = cursor.insertTable ( 1, 7, tableFormat );

    QTextFrame *frame = cursor.currentFrame();
    QTextFrameFormat frameFormat = frame->frameFormat();
    frameFormat.setBorder ( 1 );
    frame->setFrameFormat ( frameFormat );

    QTextCharFormat format = cursor.charFormat();
    format.setFontPointSize ( fontSize );

    QTextCharFormat boldFormat = format;
    boldFormat.setFontWeight ( QFont::Bold );

    QTextCharFormat highlightedFormat = boldFormat;
    highlightedFormat.setBackground ( Qt::yellow );

    for ( int weekDay = 1; weekDay <= 7; ++weekDay ) {
        QTextTableCell cell = table->cellAt ( 0, weekDay - 1 );
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.insertText ( QString ( "%1" ).arg ( QDate::longDayName ( weekDay ) ),
                                boldFormat );
    }

    table->insertRows ( table->rows(), 1 );

    while ( date.month() == selectedDate.month() ) {
        int weekDay = date.dayOfWeek();
        QTextTableCell cell = table->cellAt ( table->rows() - 1, weekDay - 1 );
        QTextCursor cellCursor = cell.firstCursorPosition();

        if ( date == QDate::currentDate() )
            cellCursor.insertText ( QString ( "%1" ).arg ( date.day() ) + " \n", highlightedFormat );
        else
            cellCursor.insertText ( QString ( "%1" ).arg ( date.day() ), format );

        QList<QString> values = m_textos.values ( date.toString() );
        for ( int i = 0; i < values.size(); ++i ) {
            cellCursor.insertText ( values.at ( i ), format );
        } // end for


        date = date.addDays ( 1 );
        if ( weekDay == 7 && date.month() == selectedDate.month() )
            table->insertRows ( table->rows(), 1 );
    }

    setWindowTitle ( tr ( "Calendar for %1 %2"
                        ).arg ( QDate::longMonthName ( selectedDate.month() )
                              ).arg ( selectedDate.year() ) );
}

void BLMonthCalendar::setFontSize ( int size )
{
    fontSize = size;
    insertCalendar();
}

void BLMonthCalendar::setMonth ( int month )
{
    selectedDate = QDate ( selectedDate.year(), month + 1, selectedDate.day() );
    insertCalendar();
}

void BLMonthCalendar::setYear ( QDate date )
{
    selectedDate = QDate ( date.year(), selectedDate.month(), selectedDate.day() );
    insertCalendar();
}

