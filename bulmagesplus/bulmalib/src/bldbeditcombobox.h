#ifndef BLDBEDITCOMBOBOX_H
#define BLDBEDITCOMBOBOX_H

#include <QObject>
#include <QEvent>
#include "blcombobox.h"


class BL_EXPORT BlDbEditComboBox : public BlComboBox
{
    Q_OBJECT

  private:
    QObject *m_view = nullptr;

  public:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    /// Almacena el id de la tabla seleccionada.
    QString mdb_id{""};
    /// Este cursor almacena el listado de elementos.
    BlDbRecordSet *m_cursorcombo;
    /// Texto entrado por el usuario (para uso de los plugins)
    QString m_entrada{""}; 

  public:
    explicit BlDbEditComboBox(QWidget *parent = 0);
    ~BlDbEditComboBox();
    QString entrada();
    QString unicaEleccion(void) ;
    QString eligeUnico(void) ;
    virtual void setQuery ( const QString & );

  public slots:
    virtual void s_editTextChanged ( const QString & );

  protected:
    virtual bool event(QEvent * event) override;
    virtual void contextMenuEvent(QContextMenuEvent *event) override;

  signals:
    void focusOut();
    void pintaMenu ( QMenu * );
    void trataMenu ( QAction * );
    void keyF2();
};

#endif

