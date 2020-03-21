/***************************************************************************
 *   Copyright (C) 2009 by Aron Galdón.                                    *
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

#include "blformlistfilter.h"


///
/**
**/
BlFormListFilter::BlFormListFilter ( QWidget *parent ) : BlWidget ( parent )
{
   BL_FUNC_DEBUG

   setupUi ( this );
   mui_subform_list = NULL;
   m_columna_actual = -1;
   hide();

   /// Disparamos los plugins.
   int res = g_plugins->run ( "BlFormListFilter_BlFormListFilter", this );
   if ( res != 0 ) {
       return;
   } // end if

   g_plugins->run ( "BlFormListFilter_BlFormListFilter", this );

   
}


/// Generar la cl&aacute;usula WHERE del listado con las opciones de filtrado especificadas
/**
  \return
**/
QString BlFormListFilter::generarFiltro()
{
   BL_FUNC_DEBUG

   if ( m_columna_actual < 0 || isHidden() ) {
      return "";
   } // end if

   QString filtro = "";

   switch ( mui_subform_list->fieldTypeByColumnId ( m_columna_actual ) )
   {

      /// Filtro para valores numéricos
      case BlDbField::DbInt:
      case BlDbField::DbNumeric:

         if ( mui_filtro_texto->text().isEmpty() ) {
            return "";
         } // end if

         filtro = generateNumericFilter ( mui_subform_list->dbFieldNameByColumnId ( m_columna_actual ), mui_filtro_texto->text() );
         break;

      case BlDbField::DbBoolean:
         filtro = generateBooleanFilter ( mui_subform_list->dbFieldNameByColumnId ( m_columna_actual ), mui_filtro_boolean->checkState() );
         break;

      case BlDbField::DbDate:
         filtro = generateDateFilter ( mui_subform_list->dbFieldNameByColumnId ( m_columna_actual ), mui_filtro_texto->text() );
         break;

      case BlDbField::DbVarChar:
      default:

         if ( mui_filtro_texto->text().isEmpty() ) {
            return "";
         } // end if

         filtro = generateTextFilter ( mui_subform_list->dbFieldNameByColumnId ( m_columna_actual ), mui_filtro_texto->text() );
         break;
   } // end switch

   /// Si hay una condici&oacute;n, agregamos AND para que se acople bien en la consulta
   if ( !filtro.isEmpty() ) {
      filtro.prepend ( " AND " );
   } // end if

   
   return ( filtro );
}


/// Prepara una condici&oacute;n WHERE para filtrar por fecha
/**
  \param campo
**/
QString BlFormListFilter::generateDateFilter ( QString campo, QString texto )
{
   BL_FUNC_DEBUG

   QString criterio = QString ( "\"%1\" " ).arg ( campo );

   /// Si hay dos fechas, el criterio es un intervalo cerrado
   if ( !mui_filtro_fecha_ini->text().isEmpty() && !mui_filtro_fecha_fin->text().isEmpty() ) {
      criterio += QString ( " BETWEEN '%1' AND '%2'" ).arg ( mui_filtro_fecha_ini->text() ).arg ( mui_filtro_fecha_fin->text() );
   } // end if

   /// Si s&oacute;lo est&aacute; la fecha inicial, ser&aacute; la menor del intervalo
   if ( mui_filtro_fecha_fin->text().isEmpty() ) {
      criterio += QString ( " >= '%1'" ).arg ( mui_filtro_fecha_ini->text() );
   } // end if

   /// Si s&oacute;lo est&aacute; la fecha final, ser&aacute; la mayor del intervalo
   if ( mui_filtro_fecha_ini->text().isEmpty() ) {
      criterio += QString ( " <= '%1'" ).arg ( mui_filtro_fecha_fin->text() );
   } // end if

   /// Sin fechas, no hay filtrado
   if ( mui_filtro_fecha_ini->text().isEmpty() && mui_filtro_fecha_fin->text().isEmpty() ) {
      criterio = "";
   } // end if

   
   return criterio;
}


/// Prepara una condici&oacute;n WHERE para filtrar por n&uacute;mero entero o con decimales
/**
  \return
**/
QString BlFormListFilter::generateNumericFilter ( QString campo, QString texto )
{
   BL_FUNC_DEBUG

   QString criterio = QString ( "\"%1\" %2" ).arg ( campo ).arg ( mui_filtro_compara->currentText() );

   /// Valdrá false si el texto no se pudo traducir como un número
   bool ok = true;

   /// Es posible que el usuario utilice comas para separar los decimales:
   /// PostgreSQL prefiere usar puntos
   double num = texto.replace ( ",", "." ).toDouble ( &ok );

   /// Si no se pudo traducir a n&uacute;mero, dejar el criterio vac&iacute;o
   if ( !ok ) {
      return "";
   } // end if

   criterio += QString ( "%1" ).arg ( num );

   
   return criterio;
}


/// Prepara una condici&oacute;n WHERE para filtrar por verdadero/falso
/*
*/
QString BlFormListFilter::generateBooleanFilter ( QString campo, Qt::CheckState estado )
{
   BL_FUNC_DEBUG

   QString valor;

   if ( estado == Qt::Checked ) {
      valor = "true";
   } // end if

   if ( estado == Qt::Unchecked ) {
      valor = "false";
   } // end if

   QString criterio;

   /// No hay criterio si el CheckBox est&aacute; en el estado indefinido
   if ( !valor.isEmpty() )
   {
      criterio = QString ( "\"%1\" = %2" ).arg ( campo ).arg ( valor );
   }

   
   return criterio;
}


/// Prepara una condici&oacute;n WHERE para filtrar una cadena de texto
/**
  \return
**/
QString BlFormListFilter::generateTextFilter ( QString campo, QString texto )
{
   BL_FUNC_DEBUG

   /// Con ILIKE conseguimos que no se diferencie entre mayúsculas y minúsculas al comparar
   QString criterio = QString ( "\"%1\" ILIKE " ).arg ( campo );

   switch ( mui_filtro_coincidencia->currentIndex() ) {

      case 2: /// Hacer coincidir todo el campo
         criterio += QString ( "'%1'" ).arg ( texto );
         break;

      case 1: /// Comienzo del campo
         criterio += QString ( "'%1%'" ).arg ( texto );
         break;

      case 0: /// Cualquier parte del campo
      default:
         criterio += QString ( "'%%1%'" ).arg ( texto );

   } // end switch

   
   return criterio;
}


/// Oculta los elementos del cuadro filtrador
/**
  Ocultamos los elementos uno por uno y no el grupo completo debido a que
  seg&uacute;n el tipo de campo se mostrar&aacute;n unos campos u otros.
**/
void BlFormListFilter::hideFilterWidgets()
{
   BL_FUNC_DEBUG

   mui_filtro_texto->hide();
   mui_filtro_columna->hide();
   mui_filtro_compara->hide();
   mui_filtro_coincidencia->hide();
   mui_filtro_boolean->hide();
   mui_filtro_fecha_ini->hide();
   mui_filtro_fecha_fin->hide();

   
}


/// Configura el cuadro del filtrador al tipo de la columna actual
/**
**/
void BlFormListFilter::configureFilterToType()
{
   BL_FUNC_DEBUG

   /// Antes de mostrar los widgets adecuados al tipo, los ocultamos todos
   hideFilterWidgets();

   if ( m_columna_actual < 0 || isHidden() ) {
      emit cerrar_filtro();
      return;
   } // end if

   /// Mostrar la etiqueta con el nombre de la columna activa para el filtro
   mui_filtro_columna->show();
   mui_filtro_columna->setText ( "\"" + mui_subform_list->dbFieldViewNameByColumnId ( m_columna_actual ) + "\":" );

   switch ( mui_subform_list->fieldTypeByColumnId ( m_columna_actual ) ) {

      /// Mostrar widgets para filtrar fechas
      case BlDbField::DbDate:
         mui_filtro_fecha_ini->show();
         mui_filtro_fecha_fin->show();
         break;

      /// Mostrar widgets para filtrar números
      case BlDbField::DbInt:
      case BlDbField::DbNumeric:
         mui_filtro_compara->setVisible ( true );
         mui_filtro_texto->show();
         break;

      /// Mostrar widgets para filtrar booleanos
      case BlDbField::DbBoolean:
         mui_filtro_boolean->show();
         break;

      /// Mostrar widgets para filtrar textos
      case BlDbField::DbVarChar:
      default:
         mui_filtro_texto->show();
         mui_filtro_coincidencia->show();
         break;

   } // end switch

   
}


/// Pasamos un puntero al subformulario que contiene la lista
/**
  Esta clase necesita consultar el nombre visual de la columna, su tipo y posicionar la lista.
**/
void BlFormListFilter::setSubFormList ( BlSubForm *list )
{
   BL_FUNC_DEBUG
   
   mui_subform_list = list;
   connect ( mui_subform_list->mui_list, SIGNAL ( currentCellChanged ( int, int, int, int ) ), this, SLOT ( updatePosition ( int, int, int, int ) ) );
   
   
}


/// Al pulsar el bot&oacute;n, muestra y oculta el filtrador alternadamente
/**
**/
void BlFormListFilter::showHideFilter()
{
   BL_FUNC_DEBUG

   /// Si el fitrador estaba oculto, mostrarlo
   if ( isHidden() ) {
      show();
      m_columna_actual = mui_subform_list->currentColumn();
      configureFilterToType();
   }

   /// Si el fitrador era visible, ocultarlo y desactivar el filtro
   else {
      cleanFilter();
      hide();
      emit aplicar_filtro(); /// Para que se restablezca el listado
   } // end if

   
}


/// Borra los datos del filtrador
/**
**/
void BlFormListFilter::cleanFilter()
{
   BL_FUNC_DEBUG

   m_columna_actual = -1;
   mui_filtro_texto->clear();
   mui_filtro_texto->setStyleSheet ( "" );
   mui_filtro_columna->clear();
   mui_filtro_compara->setCurrentIndex ( 0 );
   mui_filtro_coincidencia->setCurrentIndex ( 0 );
   mui_filtro_boolean->setCheckState ( Qt::PartiallyChecked );
   mui_filtro_fecha_ini->setText ( "" );
   mui_filtro_fecha_fin->setText ( "" );

   
}


/// Responde al cambio de campo actual en la tabla
/**
**/
void BlFormListFilter::updatePosition ( int fila, int columna, int fila_anterior, int columna_anterior )
{
   BL_FUNC_DEBUG

   /// No seguir si la columna no ha cambiado o el filtrado est&aacute; desactivado
   if ( ( columna == columna_anterior && !mui_filtro_columna->text().isEmpty() ) || isHidden() ) {
      return;
   } // end if

   /// Si la nueva columna no es correcta, significa que el listado ha sido actualizado y no hay un campo activo
   if ( columna < 0 || ( m_columna_actual >= 0 && columna == 0 && columna_anterior < 0 ) ) {

      /// Nos quedamos con la columna sobre la que se estaba trabajando hasta ahora y la fila anteriormente usada
      mui_subform_list->setCurrentItem ( fila, m_columna_actual );

   }

   /// Si la nueva columna es correcta, cambiar de campo actual
   else {
      m_columna_actual = columna;
   } // end if

   configureFilterToType();

   
}


/// Responde a la pulsaci&oacute;n del bot&oacute;n de aplicar filtro
/**
  Tras lanzar la se&ntilde;al de que debe aplicarse el filtro,
  cambia el color de fondo si no hay resultados
**/
void BlFormListFilter::on_mui_filtro_aplicar_clicked()
{
   BL_FUNC_DEBUG

   emit aplicar_filtro();

   if ( mui_subform_list->rowCount() <= 0 && mui_filtro_texto->isVisible() ) {
      mui_filtro_texto->setStyleSheet(" QLineEdit { background-color: #ffcccc } ");
   }
   else {
     mui_filtro_texto->setStyleSheet ( "" );
   }

   
}


/// Responde a la pulsaci&oacute;n del bot&oacute;n de limpiar filtro
/**
**/
void BlFormListFilter::on_mui_filtro_limpiar_clicked()
{
   BL_FUNC_DEBUG

   cleanFilter();
   emit cerrar_filtro(); /// Para que se oculte este filtrador
   emit aplicar_filtro(); /// Para que se restablezca el listado

   
}
