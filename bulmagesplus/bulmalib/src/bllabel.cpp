#include "bllabel.h"


BlLabel::BlLabel(QWidget * parent, Qt::WindowFlags f) : QLabel(parent, f)
{
}


BlLabel::BlLabel(const QString & text, QWidget * parent, Qt::WindowFlags f) : QLabel(text, parent, f)
{
}


BlLabel::~BlLabel()
{
}


void BlLabel::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}

