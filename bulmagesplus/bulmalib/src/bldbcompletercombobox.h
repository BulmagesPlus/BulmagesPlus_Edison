#ifndef BLDBCOMPLETERCOMBOBOX_H
#define BLDBCOMPLETERCOMBOBOX_H

#include <QMap>
#include <QString>
#include <QVariant>
#include <QFocusEvent>
#include <QPoint>
#include <QComboBox>

#include "blfunctions.h"
#include "blwidget.h"
#include "blcomboboxdelegate.h"
#include "ui_bldbcompletercomboboxbase.h"
#include "bldbeditcombobox.h"
#include "blsubform.h"


class BL_EXPORT BlDbCompleterComboBox : public QComboBox /*public BlComboBoxDelegate*/, public Ui_BlDbCompleterComboBoxBase
{
  Q_OBJECT

  private:

    BlSubFormDelegate *m_subFormDelegate;

  public:
    explicit BlDbCompleterComboBox(BlSubFormDelegate *subFormDelegate = nullptr, QWidget *parent = nullptr);
    ~BlDbCompleterComboBox();

    QString currentText();
    void setEditText(const QString &text);
    BlDbEditComboBox *comboBox();

  private slots:
    void emitFocusOut();
    void emitKeyF2();

  protected:
    virtual bool event(QEvent * event);

};

#endif

