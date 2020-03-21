/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
 *   Copyright (C) 2006 by Fco. Javier M. C.                               *
 *   fcojavmc@todo-redes.com                                               *
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

#include <QString>

#include "bfaboutview.h"
#include "blplugins.h"
#include "../../bulmagesplus_version.h"


/// Rellena con datos los QTextEdit y muestra el formulario.
/**
\param parent
**/
BfAboutView::BfAboutView ( QDialog *parent ) : QDialog ( parent )
{
    QString str_sobre;
//    QString str_autores;
    QString str_soporte;
    QString str_licencia;
    QString str_sistema;
    
    setupUi ( this );
    blCenterOnScreen ( this );

    //mui_lbversion->setText ( mui_lbversion->text() + " " +   _ ( "Compilado usando la version de QT:" ) + " " + QT_VERSION_STR );
    setWindowTitle(_("Acerca de") + " BulmagesPlus Factu " + QString(BULMAGESPLUS_VERSION_SHORT));
    mui_lbversion->setText ( "BulmagesPlus Factu " + QString(BULMAGESPLUS_VERSION_FULL) );

    str_sobre = "<br /><br /><center><h2>BulmagesPlus Factu " + QString(BULMAGESPLUS_VERSION_FULL) + "</h2>" \
	    "<p>Copyright&copy; 2011-2020 BulmagesPlus.</p>" \
	    "Se agradece el trabajo de todos los desarrolladores, especialmente los nombrados en " \
	    "https://bulmagesplus.com/index.php/es/listado-de-desarrolladores" \
	    "</p></center>";
    
    //str_autores = "";

    str_soporte = "<center><p>" \
                  "<p><h2>Soporte:</h2></p>" \
                  "<b>BulmagesPlus:</b> <a href=\"https://bulmagesplus.com\">" \
                  "https://bulmagesplus.com</a>" \
                  "<p></p>" \
		  "<p>Por correo: <a href=\"mailto:info@bulmagesplus.com\">info@bulmagesplus.com</a><br>" \
                  "</p></center>";


    str_licencia = "<P><H2>ACUERDO DE LICENCIA DE USO<br /> \
PARA USUARIOS FINALES DE BULMAGESPLUS</H2></P> \
<br /> \
<br /> \
Este Acuerdo de Licencia para el Usuario Final (End-User License Agreement - \"EULA\") es un contrato legal entre usted (en adelante el LICENCIATARIO) y BULMAGESPLUS (en adelante el \"LICENCIANTE\").<br /> \
<br /> \
La descarga, instalación y/o uso que el LICENCIATARIO realice de BULMAGESPLUS FACTU, CONTA O TPV queda sujeta a la concesión de la licencia incluida en este Acuerdo, y asimismo esta sujeta y condicionada a todo lo descrito a continuación.<br /> \
<br /> \
<br /> \
1. Licencia Limitada para el uso de BULMAGESPLUS FACTU, CONTA O TPV.<br /> \
Por el presente acuerdo, el \"LICENCIANTE\" le concede una licencia al LICENCIATARIO, de caracter limitado y no exclusiva, para descargar, instalar y usar BULMAGESPLUS FACTU, CONTA O TPV, la cual está sometida a las condiciones establecidas en este acuerdo. La Licencia otorgada esta limitada a la versión actual tal cual, y no incluye derecho a futuras versiones a excepción de que el LICENCIATARIO haya adquirido la LICENCIA correspondiente y/o haya suscrito un contrato de mantenimiento permanente.<br /> \
<br /> \
2. Distribucion.<br /> \
El \"LICENCIATARIO\" puede distribuir copias de la versión actual (no registrada) del programa y su documentación con las condiciones estipuladas en la licencia GPL (ver más abajo).<br /> \
<br /> \
3. Versión registrada.<br /> \
Una vez que el \"LICENCIATARIO\" registre la copia del programa a su nombre recibira un código de usuario que deberá guardar el lugar seguro. Este código es personal e intransferible y permite acceder a servicios adicionales. El código de usuario sólo pude ser usado en el número de equipos contratado. No se permite la cesión o venta de licencia a terceras partes. Esta acción puede invalidar su licencia o código de usuario y, por tanto, perderá sus derechos de servicios adicionales, actualizaciones, etc.<br /> \
<br /> \
4. Exclusión de Garantía.<br /> \
El \"LICENCIANTE\" EXCLUYE TODA RESPONSABILIDAD DERIVADA DE LA DESCARGA, INSTALACIÓN, USO O DESINSTALACIÓN POR EL LICENCIATARIO DE BULMAGESPLUS FACTU, CONTA O TPV, O SU USO DE LOS SERVICIOS DE BULMAGESPLUS.<br /> \
BULMAGESPLUS FACTU, CONTA O TPV SE PROPORCIONA \"COMO ESTÁ\" SIN GARANTÍA DE NINGUNA CLASE. EL \"LICENCIANTE\" EXCLUYE CUALQUIER GARANTÍA, EXPRESA, TÁCITA O ESTATUTORIA, INCLUYENDO, SIN LIMITACIÓN, NINGUNA GARANTÍA DE CALIDAD SATISFACTORIA Y NINGUNA GARANTÍA DE COMERCIABILIDAD Y/O DE APTITUD PARA UN PROPÓSITO DETERMINADO, Y DE NO INFRACCIÓN DE DERECHOS.<br /> \
<br /> \
5. Limitación de Responsabilidad.<br /> \
EL LICENCIATARIO ES CONSCIENTE Y ACEPTA VOLUNTARIAMENTE QUE LA DESCARGA, INSTALACIÓN Y/O UTILIZACIÓN DE BULMAGESPLUS FACTU, CONTA O TPV TIENE LUGAR BAJO SU ÚNICA Y EXCLUSIVA RESPONSABILIDAD. SIN PERJUICIO DE LO ANTERIOR, EN NINGÚN CASO EL \"LICENCIANTE\" SERÁ RESPONSABLE POR CUALESQUIERA DAÑOS O PERJUICIOS DIRECTOS, INDIRECTOS, ESPECIALES, INCIDENTALES, CONSECUENCIALES, EJEMPLARES O PUNITIVOS, DERIVADOS DE O EN RELACIÓN CON LA DESCARGA, INSTALACIÓN, USO, MAL USO O DESINSTALACIÓN QUE EL LICENCIATARIO REALICE DE BULMAGESPLUS FACTU, CONTA O TPV.<br /> \
<br /> \
6. Exclusión en relación con las Aplicaciones de Terceros.<br /> \
POR EL PRESENTE ACUERDO EL LICENCIATARIO ES CONSCIENTE Y ACEPTA QUE CUALQUIER APLICACION DE TERCEROS QUE PUEDA ESTAR INTEGRADA CON BULMAGESPLUS FACTU, CONTA O TPV, LE HA SIDO FACILITADA POR UN TERCERO PROVEEDOR Y NO POR EL \"LICENCIANTE\". EL LICENCIATARIO ES CONSCIENTE Y ACEPTA QUE LA DESCARGA, INSTALACIÓN Y USO DE CUALQUIERA DE DICHAS APLICACIONES DE TERCEROS NO ESTÁN SUJETAS AL PRESENTE ACUERDO, PERO PUEDEN ESTAR SUJETAS A LOS TÉRMINOS Y CONDICIONES DE USO, LICENCIA, POLÍTICA DE PRIVACIDAD, O CUALQUIER OTRO TIPO DE ACUERDO DE DICHAS APLICACIONES DE TERCEROS. EL \"LICENCIANTE\" EXPRESAMENTE EXCLUYE TODA RESPONSABILIDAD EN RELACIÓN CON CUALQUIERA DE DICHAS APLICACIONES DE TERCEROS O EL USO QUE EL LICENCIATARIO HAGA DE LAS MISMAS. <br /> \
<br /> \
7. Indemnización.<br /> \
AL DESCARGAR, INSTALAR, USAR O DESINSTALAR BULMAGESPLUS FACTU, CONTA O TPV, EL LICENCIATARIO ACEPTA, EN SU PROPIA CUENTA Y RIESGO, RESARCIR, DEFENDER Y MANTENER INDEMNE AL \"LICENCIANTE\", SUS MATRICES, FILIALES Y SUCURSALES, SUS SOCIOS DE NEGOCIO, LOS TERCEROS PROVEEDORES, Y SUS RESPECTIVOS DIRECTORES, OFICIALES, EMPLEADOS, DISTRIBUIDORES Y AGENTES (EN ADELANTE, \"PARTES INDEMNES\"), FRENTE Y POR CUALQUIER DAÑO, PERJUICIO, DAÑOS PERSONALES, ACCIONES, RECLAMACIONES, DEMANDAS Y GASTOS, INCLUYENDO HONORARIOS Y COSTAS, TODOS ELLOS DE CUALQUIER CLASE O NATURALEZA, DERIVADOS DE, EN RELACIÓN CON O RESULTANTES DE CUALESQUIERA RECLAMACIÓN CONTRA CUALESQUIERA DE DICHAS PARTES INDEMNES O TODAS JUNTAS, QUE SEAN CONSECUENCIA DE O EN RELACIÓN CON: (I) CUALQUIER INCUMPLIMIENTO DE ESTE ACUERDO POR EL LICENCIATARIO; O (II) LA DESCARGA, INSTALACIÓN, USO O DESINSTALACIÓN QUE EL LICENCIATARIO REALICE DE BULMAGESPLUS FACTU, CONTA O TPV, INCLUYENDO, EN PARTICULAR PERO SIN LIMITACIÓN, CUALQUIER RECLAMACIÓN POR DAÑOS PERSONALES (INCLUYENDO MUERTE), DAÑOS A LA REPUTACIÓN, VIOLACIÓN DE LA INTIMIDAD O DAÑOS A BIENES MUEBLES, INTANGIBLES, O DATOS (INCLUYENDO, SIN LIMITACIÓN, PÉRDIDA DE BIENES O PÉRDIDA DEL USO DE BIENES MUEBLES, INMUEBLES, INTANGIBLES, O DATOS). <br /> \
<br /> \
8. Varios.<br /> \
Serán competentes en los litigios derivados del presente acuerdo los juzgados y tribunales de la ciudad de Palma de Mallorca, Illes Balears, Espana. La Ley aplicable al presente acuerdo sera la española, salvaguardando en todo caso las normas imperativas del lugar cuyos tribunales conozcan de la controversia, así como las normas imperativas de defensa y protección de los consumidores.<br /> \
Este acuerdo, conjuntamente con las partes de las Condiciones Generales del Servicio en la medida en que sean coherentes con el mismo, constituirán conjuntamente la totalidad del acuerdo entre el LICENCIATARIO y el LICENCIANTE en relación con la descarga, instalación, uso o desinstalación que el LICENCIATARIO realice de BULMAGESPLUS FACTU, CONTA O TPV, y reemplaza y sustituye todo acuerdo o contrato, verbal o escrito, anterior o contemporaneo, en relación con dicho objeto. Ninguna mención en este Acuerdo excluye o limita la responsabilidad de ninguna de las partes por representación fraudulenta realizada en su nombre.<br /> \
Si alguna parte o cláusula de este Acuerdo es o fuese declarada nula o sin efecto, las restantes estipulaciones conservaran su validez.<br /> \
Cualquier modificación que el LICENCIANTE realice en este Acuerdo estará disponible en https://bulmagesplus.com/licencia.php y entrará en vigor inmediatamente después de su publicación. El uso que el LICENCIATARIO realice de BULMAGESPLUS FACTU, CONTA O TPV estara siempre sujeto a los términos y condiciones del acuerdo vigente en cada momento.<br /> \
<br /> \
9. LOPD.<br /> \
En cumplimiento de lo establecido en la Ley Organica 15/1999, de 13 de diciembre, de Protección de Datos de Carácter Personal le informamos que sus datos personales quedarán incorporados y serán tratados en los ficheros de BULMAGESPLUS, con el fin de poderle prestar, mejorar y ofrecer nuestros productos y servicios, así como para informarle sobre novedades y nuevos proyectos en los que se encuentre trabajando la empresa. Le informamos de la posibilidad de que ejerza los derechos de acceso, rectificación, cancelación y oposición de sus datos de carácter personal en info@bulmagesplus.com.<br /> \
<br /> \
10. El código fuente del programa esta licenciado bajo la licencia GNU GPL (o compatible) que se muestra a continuación:<br /> \
<br /> \
<br /> \
<br />" \
                   "<P><H2>GNU GENERAL PUBLIC LICENSE</H2>  " \
                   "<P>  " \
                   "Version 2, June 1991  " \
                   "</P> " \
                   "<P> " \
                   "Copyright (C) 1989, 1991 Free Software Foundation, Inc. " \
                   "51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA " \
                   "Everyone is permitted to copy and distribute verbatim copies " \
                   "of this license document, but changing it is not allowed. " \
                   "</P> " \
                   "<H2>Preamble</H2> " \
                   "<P> " \
                   "  The licenses for most software are designed to take away your " \
                   "freedom to share and change it.  By contrast, the GNU General Public " \
                   "License is intended to guarantee your freedom to share and change free " \
                   "software--to make sure the software is free for all its users.  This " \
                   "General Public License applies to most of the Free Software " \
                   "Foundation's software and to any other program whose authors commit to " \
                   "using it.  (Some other Free Software Foundation software is covered by " \
                   "the GNU Lesser General Public License instead.)  You can apply it to " \
                   "your programs, too. " \
                   "</P> " \
                   "<P> " \
                   "  When we speak of free software, we are referring to freedom, not " \
                   "price.  Our General Public Licenses are designed to make sure that you " \
                   "have the freedom to distribute copies of free software (and charge for " \
                   "this service if you wish), that you receive source code or can get it " \
                   "if you want it, that you can change the software or use pieces of it " \
                   "in new free programs; and that you know you can do these things. " \
                   "</P> " \
                   "<P> " \
                   "  To protect your rights, we need to make restrictions that forbid " \
                   "anyone to deny you these rights or to ask you to surrender the rights. " \
                   "These restrictions translate to certain responsibilities for you if you " \
                   "distribute copies of the software, or if you modify it. " \
                   "</P> " \
                   "<P> " \
                   "  For example, if you distribute copies of such a program, whether " \
                   "gratis or for a fee, you must give the recipients all the rights that " \
                   "you have.  You must make sure that they, too, receive or can get the " \
                   "source code.  And you must show them these terms so they know their " \
                   "rights. " \
                   "</P> " \
                   "<P> " \
                   "  We protect your rights with two steps: (1) copyright the software, and " \
                   "(2) offer you this license which gives you legal permission to copy, " \
                   "distribute and/or modify the software. " \
                   "</P> " \
                   "<P> " \
                   "  Also, for each author's protection and ours, we want to make certain " \
                   "that everyone understands that there is no warranty for this free " \
                   "software.  If the software is modified by someone else and passed on, we " \
                   "want its recipients to know that what they have is not the original, so " \
                   "that any problems introduced by others will not reflect on the original " \
                   "authors' reputations. " \
                   "</P> " \
                   "<P> " \
                   "  Finally, any free program is threatened constantly by software " \
                   "patents.  We wish to avoid the danger that redistributors of a free " \
                   "program will individually obtain patent licenses, in effect making the " \
                   "program proprietary.  To prevent this, we have made it clear that any " \
                   "patent must be licensed for everyone's free use or not licensed at all. " \
                   "</P> " \
                   "<P> " \
                   "  The precise terms and conditions for copying, distribution and " \
                   "modification follow. " \
                   "</P> " \
                   "<H2>TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION</H2> " \
                   "<P> " \
                   "<STRONG>0.</STRONG> " \
                   " This License applies to any program or other work which contains " \
                   "a notice placed by the copyright holder saying it may be distributed " \
                   "under the terms of this General Public License.  The \"Program\", below, " \
                   "refers to any such program or work, and a \"work based on the Program\" " \
                   "means either the Program or any derivative work under copyright law: " \
                   "that is to say, a work containing the Program or a portion of it, " \
                   "either verbatim or with modifications and/or translated into another " \
                   "language.  (Hereinafter, translation is included without limitation in " \
                   "the term \"modification\".)  Each licensee is addressed as \"you\". " \
                   "<P> " \
                   "Activities other than copying, distribution and modification are not " \
                   "covered by this License; they are outside its scope.  The act of " \
                   "running the Program is not restricted, and the output from the Program " \
                   "is covered only if its contents constitute a work based on the " \
                   "Program (independent of having been made by running the Program). " \
                   "Whether that is true depends on what the Program does. " \
                   "<P> " \
                   "<STRONG>1.</STRONG> " \
                   " You may copy and distribute verbatim copies of the Program's " \
                   "source code as you receive it, in any medium, provided that you " \
                   "conspicuously and appropriately publish on each copy an appropriate " \
                   "copyright notice and disclaimer of warranty; keep intact all the " \
                   "notices that refer to this License and to the absence of any warranty; " \
                   "and give any other recipients of the Program a copy of this License " \
                   "along with the Program. " \
                   "<P> " \
                   "You may charge a fee for the physical act of transferring a copy, and " \
                   "you may at your option offer warranty protection in exchange for a fee. " \
                   "<P> " \
                   "<STRONG>2.</STRONG> " \
                   " You may modify your copy or copies of the Program or any portion " \
                   "of it, thus forming a work based on the Program, and copy and " \
                   "distribute such modifications or work under the terms of Section 1 " \
                   "above, provided that you also meet all of these conditions: " \
                   "<P> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>a)</STRONG> " \
                   "     You must cause the modified files to carry prominent notices " \
                   "     stating that you changed the files and the date of any change. " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>b)</STRONG> " \
                   "     You must cause any work that you distribute or publish, that in " \
                   "     whole or in part contains or is derived from the Program or any " \
                   "     part thereof, to be licensed as a whole at no charge to all third " \
                   "     parties under the terms of this License. " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>c)</STRONG> " \
                   "     If the modified program normally reads commands interactively " \
                   "     when run, you must cause it, when started running for such " \
                   "     interactive use in the most ordinary way, to print or display an " \
                   "     announcement including an appropriate copyright notice and a " \
                   "     notice that there is no warranty (or else, saying that you provide " \
                   "     a warranty) and that users may redistribute the program under " \
                   "     these conditions, and telling the user how to view a copy of this " \
                   "     License.  (Exception: if the Program itself is interactive but " \
                   "     does not normally print such an announcement, your work based on " \
                   "     the Program is not required to print an announcement.) " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "These requirements apply to the modified work as a whole.  If " \
                   "identifiable sections of that work are not derived from the Program, " \
                   "and can be reasonably considered independent and separate works in " \
                   "themselves, then this License, and its terms, do not apply to those " \
                   "sections when you distribute them as separate works.  But when you " \
                   "distribute the same sections as part of a whole which is a work based " \
                   "on the Program, the distribution of the whole must be on the terms of " \
                   "this License, whose permissions for other licensees extend to the " \
                   "entire whole, and thus to each and every part regardless of who wrote it. " \
                   "<P> " \
                   "Thus, it is not the intent of this section to claim rights or contest " \
                   "your rights to work written entirely by you; rather, the intent is to " \
                   "exercise the right to control the distribution of derivative or " \
                   "collective works based on the Program. " \
                   "<P> " \
                   "In addition, mere aggregation of another work not based on the Program " \
                   "with the Program (or with a work based on the Program) on a volume of " \
                   "a storage or distribution medium does not bring the other work under " \
                   "the scope of this License. " \
                   "<P> " \
                   "<STRONG>3.</STRONG> " \
                   " You may copy and distribute the Program (or a work based on it, " \
                   "under Section 2) in object code or executable form under the terms of " \
                   "Sections 1 and 2 above provided that you also do one of the following: " \
                   "<!-- we use this doubled UL to get the sub-sections indented, --> " \
                   "<!-- while making the bullets as unobvious as possible. --> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>a)</STRONG> " \
                   "     Accompany it with the complete corresponding machine-readable " \
                   "     source code, which must be distributed under the terms of Sections " \
                   "     1 and 2 above on a medium customarily used for software interchange; or, " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>b)</STRONG> " \
                   "     Accompany it with a written offer, valid for at least three " \
                   "     years, to give any third party, for a charge no more than your " \
                   "     cost of physically performing source distribution, a complete " \
                   "     machine-readable copy of the corresponding source code, to be " \
                   "     distributed under the terms of Sections 1 and 2 above on a medium " \
                   "     customarily used for software interchange; or, " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>c)</STRONG> " \
                   "     Accompany it with the information you received as to the offer " \
                   "     to distribute corresponding source code.  (This alternative is " \
                   "     allowed only for noncommercial distribution and only if you " \
                   "     received the program in object code or executable form with such " \
                   "     an offer, in accord with Subsection b above.) " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "The source code for a work means the preferred form of the work for " \
                   "making modifications to it.  For an executable work, complete source " \
                   "code means all the source code for all modules it contains, plus any " \
                   "associated interface definition files, plus the scripts used to " \
                   "control compilation and installation of the executable.  However, as a " \
                   "special exception, the source code distributed need not include " \
                   "anything that is normally distributed (in either source or binary " \
                   "form) with the major components (compiler, kernel, and so on) of the " \
                   "operating system on which the executable runs, unless that component " \
                   "itself accompanies the executable. " \
                   "<P> " \
                   "If distribution of executable or object code is made by offering " \
                   "access to copy from a designated place, then offering equivalent " \
                   "access to copy the source code from the same place counts as " \
                   "distribution of the source code, even though third parties are not " \
                   "compelled to copy the source along with the object code. " \
                   "<P> " \
                   "<STRONG>4.</STRONG> " \
                   " You may not copy, modify, sublicense, or distribute the Program " \
                   "except as expressly provided under this License.  Any attempt " \
                   "otherwise to copy, modify, sublicense or distribute the Program is " \
                   "void, and will automatically terminate your rights under this License. " \
                   "However, parties who have received copies, or rights, from you under " \
                   "this License will not have their licenses terminated so long as such " \
                   "parties remain in full compliance. " \
                   "<P> " \
                   "<STRONG>5.</STRONG> " \
                   " You are not required to accept this License, since you have not " \
                   "signed it.  However, nothing else grants you permission to modify or " \
                   "distribute the Program or its derivative works.  These actions are " \
                   "prohibited by law if you do not accept this License.  Therefore, by " \
                   "modifying or distributing the Program (or any work based on the " \
                   "Program), you indicate your acceptance of this License to do so, and " \
                   "all its terms and conditions for copying, distributing or modifying " \
                   "the Program or works based on it. " \
                   "<P> " \
                   "<STRONG>6.</STRONG> " \
                   " Each time you redistribute the Program (or any work based on the " \
                   "Program), the recipient automatically receives a license from the " \
                   "original licensor to copy, distribute or modify the Program subject to " \
                   "these terms and conditions.  You may not impose any further " \
                   "restrictions on the recipients' exercise of the rights granted herein. " \
                   "You are not responsible for enforcing compliance by third parties to " \
                   "this License. " \
                   "<P> " \
                   "<STRONG>7.</STRONG> " \
                   " If, as a consequence of a court judgment or allegation of patent " \
                   "infringement or for any other reason (not limited to patent issues), " \
                   "conditions are imposed on you (whether by court order, agreement or " \
                   "otherwise) that contradict the conditions of this License, they do not " \
                   "excuse you from the conditions of this License.  If you cannot " \
                   "distribute so as to satisfy simultaneously your obligations under this " \
                   "License and any other pertinent obligations, then as a consequence you " \
                   "may not distribute the Program at all.  For example, if a patent " \
                   "license would not permit royalty-free redistribution of the Program by " \
                   "all those who receive copies directly or indirectly through you, then " \
                   "the only way you could satisfy both it and this License would be to " \
                   "refrain entirely from distribution of the Program. " \
                   "<P> " \
                   "If any portion of this section is held invalid or unenforceable under " \
                   "any particular circumstance, the balance of the section is intended to " \
                   "apply and the section as a whole is intended to apply in other " \
                   "circumstances. " \
                   "<P> " \
                   "It is not the purpose of this section to induce you to infringe any " \
                   "patents or other property right claims or to contest validity of any " \
                   "such claims; this section has the sole purpose of protecting the " \
                   "integrity of the free software distribution system, which is " \
                   "implemented by public license practices.  Many people have made " \
                   "generous contributions to the wide range of software distributed " \
                   "through that system in reliance on consistent application of that " \
                   "system; it is up to the author/donor to decide if he or she is willing " \
                   "to distribute software through any other system and a licensee cannot " \
                   "impose that choice. " \
                   "<P> " \
                   "This section is intended to make thoroughly clear what is believed to " \
                   "be a consequence of the rest of this License. " \
                   "<P> " \
                   "<STRONG>8.</STRONG> " \
                   " If the distribution and/or use of the Program is restricted in " \
                   "certain countries either by patents or by copyrighted interfaces, the " \
                   "original copyright holder who places the Program under this License " \
                   "may add an explicit geographical distribution limitation excluding " \
                   "those countries, so that distribution is permitted only in or among " \
                   "countries not thus excluded.  In such case, this License incorporates " \
                   "the limitation as if written in the body of this License. " \
                   "<P> " \
                   "<STRONG>9.</STRONG> " \
                   " The Free Software Foundation may publish revised and/or new versions " \
                   "of the General Public License from time to time.  Such new versions will " \
                   "be similar in spirit to the present version, but may differ in detail to " \
                   "address new problems or concerns. " \
                   "<P> " \
                   "Each version is given a distinguishing version number.  If the Program " \
                   "specifies a version number of this License which applies to it and \"any " \
                   "later version\", you have the option of following the terms and conditions " \
                   "either of that version or of any later version published by the Free " \
                   "Software Foundation.  If the Program does not specify a version number of " \
                   "this License, you may choose any version ever published by the Free Software " \
                   "Foundation. " \
                   "<P> " \
                   "<STRONG>10.</STRONG> " \
                   " If you wish to incorporate parts of the Program into other free " \
                   "programs whose distribution conditions are different, write to the author " \
                   "to ask for permission.  For software which is copyrighted by the Free " \
                   "Software Foundation, write to the Free Software Foundation; we sometimes " \
                   "make exceptions for this.  Our decision will be guided by the two goals " \
                   "of preserving the free status of all derivatives of our free software and " \
                   "of promoting the sharing and reuse of software generally. " \
                   "<P><STRONG>NO WARRANTY</STRONG></P> " \
                   "<P> " \
                   "<STRONG>11.</STRONG> " \
                   " BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY " \
                   "FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN " \
                   "OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES " \
                   "PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED " \
                   "OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF " \
                   "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS " \
                   "TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE " \
                   "PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, " \
                   "REPAIR OR CORRECTION. " \
                   "<P> " \
                   "<STRONG>12.</STRONG> " \
                   " IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING " \
                   "WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR " \
                   "REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR " \
                   "DAMAGES, " \
                   "INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES " \
                   "ARISING " \
                   "OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED " \
                   "TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY " \
                   "YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER " \
                   "PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE " \
                   "POSSIBILITY OF SUCH DAMAGES. " \
                   "<P> " \
                   "<H2>END OF TERMS AND CONDITIONS</H2>";

		  
    /*
    str_licencia = "<P> " \
                   "<H2>GNU GENERAL PUBLIC LICENSE</H2>  " \
                   "<P>  " \
                   "Version 2, June 1991  " \
                   "</P> " \
                   "<P> " \
                   "Copyright (C) 1989, 1991 Free Software Foundation, Inc. " \
                   "51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA " \
                   "Everyone is permitted to copy and distribute verbatim copies " \
                   "of this license document, but changing it is not allowed. " \
                   "</P> " \
                   "<H2>Preamble</H2> " \
                   "<P> " \
                   "  The licenses for most software are designed to take away your " \
                   "freedom to share and change it.  By contrast, the GNU General Public " \
                   "License is intended to guarantee your freedom to share and change free " \
                   "software--to make sure the software is free for all its users.  This " \
                   "General Public License applies to most of the Free Software " \
                   "Foundation's software and to any other program whose authors commit to " \
                   "using it.  (Some other Free Software Foundation software is covered by " \
                   "the GNU Lesser General Public License instead.)  You can apply it to " \
                   "your programs, too. " \
                   "</P> " \
                   "<P> " \
                   "  When we speak of free software, we are referring to freedom, not " \
                   "price.  Our General Public Licenses are designed to make sure that you " \
                   "have the freedom to distribute copies of free software (and charge for " \
                   "this service if you wish), that you receive source code or can get it " \
                   "if you want it, that you can change the software or use pieces of it " \
                   "in new free programs; and that you know you can do these things. " \
                   "</P> " \
                   "<P> " \
                   "  To protect your rights, we need to make restrictions that forbid " \
                   "anyone to deny you these rights or to ask you to surrender the rights. " \
                   "These restrictions translate to certain responsibilities for you if you " \
                   "distribute copies of the software, or if you modify it. " \
                   "</P> " \
                   "<P> " \
                   "  For example, if you distribute copies of such a program, whether " \
                   "gratis or for a fee, you must give the recipients all the rights that " \
                   "you have.  You must make sure that they, too, receive or can get the " \
                   "source code.  And you must show them these terms so they know their " \
                   "rights. " \
                   "</P> " \
                   "<P> " \
                   "  We protect your rights with two steps: (1) copyright the software, and " \
                   "(2) offer you this license which gives you legal permission to copy, " \
                   "distribute and/or modify the software. " \
                   "</P> " \
                   "<P> " \
                   "  Also, for each author's protection and ours, we want to make certain " \
                   "that everyone understands that there is no warranty for this free " \
                   "software.  If the software is modified by someone else and passed on, we " \
                   "want its recipients to know that what they have is not the original, so " \
                   "that any problems introduced by others will not reflect on the original " \
                   "authors' reputations. " \
                   "</P> " \
                   "<P> " \
                   "  Finally, any free program is threatened constantly by software " \
                   "patents.  We wish to avoid the danger that redistributors of a free " \
                   "program will individually obtain patent licenses, in effect making the " \
                   "program proprietary.  To prevent this, we have made it clear that any " \
                   "patent must be licensed for everyone's free use or not licensed at all. " \
                   "</P> " \
                   "<P> " \
                   "  The precise terms and conditions for copying, distribution and " \
                   "modification follow. " \
                   "</P> " \
                   "<H2>TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION</H2> " \
                   "<P> " \
                   "<STRONG>0.</STRONG> " \
                   " This License applies to any program or other work which contains " \
                   "a notice placed by the copyright holder saying it may be distributed " \
                   "under the terms of this General Public License.  The \"Program\", below, " \
                   "refers to any such program or work, and a \"work based on the Program\" " \
                   "means either the Program or any derivative work under copyright law: " \
                   "that is to say, a work containing the Program or a portion of it, " \
                   "either verbatim or with modifications and/or translated into another " \
                   "language.  (Hereinafter, translation is included without limitation in " \
                   "the term \"modification\".)  Each licensee is addressed as \"you\". " \
                   "<P> " \
                   "Activities other than copying, distribution and modification are not " \
                   "covered by this License; they are outside its scope.  The act of " \
                   "running the Program is not restricted, and the output from the Program " \
                   "is covered only if its contents constitute a work based on the " \
                   "Program (independent of having been made by running the Program). " \
                   "Whether that is true depends on what the Program does. " \
                   "<P> " \
                   "<STRONG>1.</STRONG> " \
                   " You may copy and distribute verbatim copies of the Program's " \
                   "source code as you receive it, in any medium, provided that you " \
                   "conspicuously and appropriately publish on each copy an appropriate " \
                   "copyright notice and disclaimer of warranty; keep intact all the " \
                   "notices that refer to this License and to the absence of any warranty; " \
                   "and give any other recipients of the Program a copy of this License " \
                   "along with the Program. " \
                   "<P> " \
                   "You may charge a fee for the physical act of transferring a copy, and " \
                   "you may at your option offer warranty protection in exchange for a fee. " \
                   "<P> " \
                   "<STRONG>2.</STRONG> " \
                   " You may modify your copy or copies of the Program or any portion " \
                   "of it, thus forming a work based on the Program, and copy and " \
                   "distribute such modifications or work under the terms of Section 1 " \
                   "above, provided that you also meet all of these conditions: " \
                   "<P> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>a)</STRONG> " \
                   "     You must cause the modified files to carry prominent notices " \
                   "     stating that you changed the files and the date of any change. " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>b)</STRONG> " \
                   "     You must cause any work that you distribute or publish, that in " \
                   "     whole or in part contains or is derived from the Program or any " \
                   "     part thereof, to be licensed as a whole at no charge to all third " \
                   "     parties under the terms of this License. " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>c)</STRONG> " \
                   "     If the modified program normally reads commands interactively " \
                   "     when run, you must cause it, when started running for such " \
                   "     interactive use in the most ordinary way, to print or display an " \
                   "     announcement including an appropriate copyright notice and a " \
                   "     notice that there is no warranty (or else, saying that you provide " \
                   "     a warranty) and that users may redistribute the program under " \
                   "     these conditions, and telling the user how to view a copy of this " \
                   "     License.  (Exception: if the Program itself is interactive but " \
                   "     does not normally print such an announcement, your work based on " \
                   "     the Program is not required to print an announcement.) " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "These requirements apply to the modified work as a whole.  If " \
                   "identifiable sections of that work are not derived from the Program, " \
                   "and can be reasonably considered independent and separate works in " \
                   "themselves, then this License, and its terms, do not apply to those " \
                   "sections when you distribute them as separate works.  But when you " \
                   "distribute the same sections as part of a whole which is a work based " \
                   "on the Program, the distribution of the whole must be on the terms of " \
                   "this License, whose permissions for other licensees extend to the " \
                   "entire whole, and thus to each and every part regardless of who wrote it. " \
                   "<P> " \
                   "Thus, it is not the intent of this section to claim rights or contest " \
                   "your rights to work written entirely by you; rather, the intent is to " \
                   "exercise the right to control the distribution of derivative or " \
                   "collective works based on the Program. " \
                   "<P> " \
                   "In addition, mere aggregation of another work not based on the Program " \
                   "with the Program (or with a work based on the Program) on a volume of " \
                   "a storage or distribution medium does not bring the other work under " \
                   "the scope of this License. " \
                   "<P> " \
                   "<STRONG>3.</STRONG> " \
                   " You may copy and distribute the Program (or a work based on it, " \
                   "under Section 2) in object code or executable form under the terms of " \
                   "Sections 1 and 2 above provided that you also do one of the following: " \
                   "<!-- we use this doubled UL to get the sub-sections indented, --> " \
                   "<!-- while making the bullets as unobvious as possible. --> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>a)</STRONG> " \
                   "     Accompany it with the complete corresponding machine-readable " \
                   "     source code, which must be distributed under the terms of Sections " \
                   "     1 and 2 above on a medium customarily used for software interchange; or, " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>b)</STRONG> " \
                   "     Accompany it with a written offer, valid for at least three " \
                   "     years, to give any third party, for a charge no more than your " \
                   "     cost of physically performing source distribution, a complete " \
                   "     machine-readable copy of the corresponding source code, to be " \
                   "     distributed under the terms of Sections 1 and 2 above on a medium " \
                   "     customarily used for software interchange; or, " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "<DL> " \
                   "<DT> " \
                   "<DD> " \
                   "<STRONG>c)</STRONG> " \
                   "     Accompany it with the information you received as to the offer " \
                   "     to distribute corresponding source code.  (This alternative is " \
                   "     allowed only for noncommercial distribution and only if you " \
                   "     received the program in object code or executable form with such " \
                   "     an offer, in accord with Subsection b above.) " \
                   "</DD> " \
                   "</DT> " \
                   "</DL> " \
                   "<P> " \
                   "The source code for a work means the preferred form of the work for " \
                   "making modifications to it.  For an executable work, complete source " \
                   "code means all the source code for all modules it contains, plus any " \
                   "associated interface definition files, plus the scripts used to " \
                   "control compilation and installation of the executable.  However, as a " \
                   "special exception, the source code distributed need not include " \
                   "anything that is normally distributed (in either source or binary " \
                   "form) with the major components (compiler, kernel, and so on) of the " \
                   "operating system on which the executable runs, unless that component " \
                   "itself accompanies the executable. " \
                   "<P> " \
                   "If distribution of executable or object code is made by offering " \
                   "access to copy from a designated place, then offering equivalent " \
                   "access to copy the source code from the same place counts as " \
                   "distribution of the source code, even though third parties are not " \
                   "compelled to copy the source along with the object code. " \
                   "<P> " \
                   "<STRONG>4.</STRONG> " \
                   " You may not copy, modify, sublicense, or distribute the Program " \
                   "except as expressly provided under this License.  Any attempt " \
                   "otherwise to copy, modify, sublicense or distribute the Program is " \
                   "void, and will automatically terminate your rights under this License. " \
                   "However, parties who have received copies, or rights, from you under " \
                   "this License will not have their licenses terminated so long as such " \
                   "parties remain in full compliance. " \
                   "<P> " \
                   "<STRONG>5.</STRONG> " \
                   " You are not required to accept this License, since you have not " \
                   "signed it.  However, nothing else grants you permission to modify or " \
                   "distribute the Program or its derivative works.  These actions are " \
                   "prohibited by law if you do not accept this License.  Therefore, by " \
                   "modifying or distributing the Program (or any work based on the " \
                   "Program), you indicate your acceptance of this License to do so, and " \
                   "all its terms and conditions for copying, distributing or modifying " \
                   "the Program or works based on it. " \
                   "<P> " \
                   "<STRONG>6.</STRONG> " \
                   " Each time you redistribute the Program (or any work based on the " \
                   "Program), the recipient automatically receives a license from the " \
                   "original licensor to copy, distribute or modify the Program subject to " \
                   "these terms and conditions.  You may not impose any further " \
                   "restrictions on the recipients' exercise of the rights granted herein. " \
                   "You are not responsible for enforcing compliance by third parties to " \
                   "this License. " \
                   "<P> " \
                   "<STRONG>7.</STRONG> " \
                   " If, as a consequence of a court judgment or allegation of patent " \
                   "infringement or for any other reason (not limited to patent issues), " \
                   "conditions are imposed on you (whether by court order, agreement or " \
                   "otherwise) that contradict the conditions of this License, they do not " \
                   "excuse you from the conditions of this License.  If you cannot " \
                   "distribute so as to satisfy simultaneously your obligations under this " \
                   "License and any other pertinent obligations, then as a consequence you " \
                   "may not distribute the Program at all.  For example, if a patent " \
                   "license would not permit royalty-free redistribution of the Program by " \
                   "all those who receive copies directly or indirectly through you, then " \
                   "the only way you could satisfy both it and this License would be to " \
                   "refrain entirely from distribution of the Program. " \
                   "<P> " \
                   "If any portion of this section is held invalid or unenforceable under " \
                   "any particular circumstance, the balance of the section is intended to " \
                   "apply and the section as a whole is intended to apply in other " \
                   "circumstances. " \
                   "<P> " \
                   "It is not the purpose of this section to induce you to infringe any " \
                   "patents or other property right claims or to contest validity of any " \
                   "such claims; this section has the sole purpose of protecting the " \
                   "integrity of the free software distribution system, which is " \
                   "implemented by public license practices.  Many people have made " \
                   "generous contributions to the wide range of software distributed " \
                   "through that system in reliance on consistent application of that " \
                   "system; it is up to the author/donor to decide if he or she is willing " \
                   "to distribute software through any other system and a licensee cannot " \
                   "impose that choice. " \
                   "<P> " \
                   "This section is intended to make thoroughly clear what is believed to " \
                   "be a consequence of the rest of this License. " \
                   "<P> " \
                   "<STRONG>8.</STRONG> " \
                   " If the distribution and/or use of the Program is restricted in " \
                   "certain countries either by patents or by copyrighted interfaces, the " \
                   "original copyright holder who places the Program under this License " \
                   "may add an explicit geographical distribution limitation excluding " \
                   "those countries, so that distribution is permitted only in or among " \
                   "countries not thus excluded.  In such case, this License incorporates " \
                   "the limitation as if written in the body of this License. " \
                   "<P> " \
                   "<STRONG>9.</STRONG> " \
                   " The Free Software Foundation may publish revised and/or new versions " \
                   "of the General Public License from time to time.  Such new versions will " \
                   "be similar in spirit to the present version, but may differ in detail to " \
                   "address new problems or concerns. " \
                   "<P> " \
                   "Each version is given a distinguishing version number.  If the Program " \
                   "specifies a version number of this License which applies to it and \"any " \
                   "later version\", you have the option of following the terms and conditions " \
                   "either of that version or of any later version published by the Free " \
                   "Software Foundation.  If the Program does not specify a version number of " \
                   "this License, you may choose any version ever published by the Free Software " \
                   "Foundation. " \
                   "<P> " \
                   "<STRONG>10.</STRONG> " \
                   " If you wish to incorporate parts of the Program into other free " \
                   "programs whose distribution conditions are different, write to the author " \
                   "to ask for permission.  For software which is copyrighted by the Free " \
                   "Software Foundation, write to the Free Software Foundation; we sometimes " \
                   "make exceptions for this.  Our decision will be guided by the two goals " \
                   "of preserving the free status of all derivatives of our free software and " \
                   "of promoting the sharing and reuse of software generally. " \
                   "<P><STRONG>NO WARRANTY</STRONG></P> " \
                   "<P> " \
                   "<STRONG>11.</STRONG> " \
                   " BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY " \
                   "FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN " \
                   "OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES " \
                   "PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED " \
                   "OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF " \
                   "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS " \
                   "TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE " \
                   "PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, " \
                   "REPAIR OR CORRECTION. " \
                   "<P> " \
                   "<STRONG>12.</STRONG> " \
                   " IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING " \
                   "WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR " \
                   "REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR " \
                   "DAMAGES, " \
                   "INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES " \
                   "ARISING " \
                   "OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED " \
                   "TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY " \
                   "YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER " \
                   "PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE " \
                   "POSSIBILITY OF SUCH DAMAGES. " \
                   "<P> " \
                   "<H2>END OF TERMS AND CONDITIONS</H2>";
*/

    str_sistema = "<h2>" + _("Idiomas y traducciones:") + "</h2>";

    str_sistema += _("Idioma:") + " " + QString(getenv("LANG")) + "<br><br>";
    
    str_sistema += "<h2>" + _("Plugins:") + "</h2>";
    
    for (int i = 0; i < g_plugins->pluginsLoaded().size(); ++i) {
	str_sistema += g_plugins->pluginsLoaded().at(i)->fileName() + "<br>";
    }

    textEdit_sobre->setHtml ( str_sobre );
//    textEdit_autores->setHtml ( str_autores );
    textEdit_soporte->setHtml ( str_soporte );
    textEdit_licencia->setHtml ( str_licencia );
    textEdit_sistema->setHtml ( str_sistema );
}


/// Destructor de la clase
/**
**/
BfAboutView::~BfAboutView()
{}

