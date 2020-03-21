#include <QEvent>
#include <QMenu>
#include <QDebug>
#include "bldbcompletercombobox.h"
#include "blplugins.h"


BlDbCompleterComboBox::BlDbCompleterComboBox(BlSubFormDelegate *subFormDelegate, QWidget *parent) : QComboBox(parent), m_subFormDelegate(subFormDelegate)
{
  BL_FUNC_DEBUG

  /// No usar para los widgets 'mui_nombre' porque luego peta el programa al no entontrar el widget BlDialogChanges.

  setupUi(this);
  setFocusPolicy(Qt::StrongFocus);
  m_comboBox->setEditable(true);
  /// Desconectamos el activated ya que en los subformularios no tiene que funcionar.
  disconnect(m_comboBox, SIGNAL ( activated ( int ) ), 0, 0);

  setFocusProxy(m_comboBox);
  connect(m_comboBox, SIGNAL(focusOut()), this, SLOT(emitFocusOut()));
  connect(m_comboBox, SIGNAL(keyF2()), this, SLOT(emitKeyF2()));
}


BlDbCompleterComboBox::~BlDbCompleterComboBox()
{
  BL_FUNC_DEBUG
}


QString BlDbCompleterComboBox::currentText()
{
  BL_FUNC_DEBUG

  return m_comboBox->currentText();
}


void BlDbCompleterComboBox::setEditText(const QString &text)
{
  BL_FUNC_DEBUG

  m_comboBox->setEditText(text);
}


void BlDbCompleterComboBox::emitFocusOut()
{
  BL_FUNC_DEBUG

  if (m_subFormDelegate)
  {
    m_subFormDelegate->readyEdit(this);
  } // end if
}


BlDbEditComboBox *BlDbCompleterComboBox::comboBox()
{
  BL_FUNC_DEBUG

  return m_comboBox;
}


bool BlDbCompleterComboBox::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab)
        {
            return true;
        } // end if
    } // end if

    return QWidget::event(event);
}



void BlDbCompleterComboBox::emitKeyF2()
{
  BL_FUNC_DEBUG

  QKeyEvent *event = new QKeyEvent ( QEvent::KeyRelease, Qt::Key_F2, Qt::NoModifier);
  QCoreApplication::postEvent (parent(), event);
}

