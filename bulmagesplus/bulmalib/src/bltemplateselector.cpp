/***************************************************************************
 *   Copyright (C) 2019 by Fco. Javier Miró Carmona.                       *
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
#include <QListWidgetItem>
#include <QWidget>
#include "bltemplateselector.h"


BlTemplateSelector::BlTemplateSelector(QWidget *parent) : QDialog(parent)
{
    BL_FUNC_DEBUG

    setupUi(this);
    mui_select->setEnabled(false);
    mui_title->setText(_("Plantillas disponibles:"));
    connect(mui_templateList, SIGNAL(itemSelectionChanged()), this, SLOT(templateListItemSelectionChanged()));

}


BlTemplateSelector::~BlTemplateSelector()
{
    BL_FUNC_DEBUG
}


QString BlTemplateSelector::value()
{
    BL_FUNC_DEBUG

    return m_value;
}


void BlTemplateSelector::setValues(const QStringList & values)
{
    BL_FUNC_DEBUG

    /// Fill the list with values.
    for (int i = 0; i < values.size(); ++i)
    {
        QString value = values.at(i);
        new QListWidgetItem(value, mui_templateList);
    } // end for
}


void BlTemplateSelector::templateListItemSelectionChanged()
{
    BL_FUNC_DEBUG

    mui_select->setEnabled(true);
    m_value = mui_templateList->currentItem()->text();
}


void BlTemplateSelector::on_mui_select_clicked()
{
    BL_FUNC_DEBUG

    done(QDialog::Accepted);
}


void BlTemplateSelector::on_mui_cancel_clicked()
{
    BL_FUNC_DEBUG

    done(QDialog::Rejected);
}

