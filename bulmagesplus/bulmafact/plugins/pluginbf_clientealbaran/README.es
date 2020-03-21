#######
#Plugins
#######
#README.es
#20080501

Nombre: Albaranes a clientes

Biblioteca: libpluginbf_clientealbaran.so

Descripción:  <H1><IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_delivery_note.svg" WIDTH="100"/><BR>Albaranes a clientes</H1>
<P>Soporte para emisión de albaranes a clientes.</P>
<P>Al cargar el plugin tendremos dos nuevas entradas de meńu <B>Listado de Albaranes <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_delivery_note_list.svg" WIDTH="22"/></B> desde la que accederemos al listado de albaranes emitidos y <B>Nuevo Albaran <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_delivery_note.svg" WIDTH="22"/></B> desde el que podremos emitir un nuevo albaran.</P>
<H2><IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_delivery_note.svg" WIDTH="22"/>Listado de Albaranes</H2>
<P> Mediante el listado de albaranes podremos controlar todos los albaranes emitidos, editarlos y borrarlos. Para ello podemos utilizar la barra de botones superior del listado.</P>
<P> Es común en todos los listados de BulmaGés poder configurarlos presentando los campos que nos interese. Para ello tenemos el <B>menú del subformulario <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/48x48/toggleconfig.png" WIDTH="22"/></B> o la opción <B>Configurar Listado <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/document_properties.svg" WIDTH="22"/></B> que nos presenta en el lateral derecho del listado todos los campos mostrables por el mismo, opciones de paginación y la consulta de base de datos que genera dicho listado</P>
<P><B>Nota:</B>Puede haber opciones ocultas para usted que se activan mediante la pulsación del botón derecho del ratón en determinadas zonas. Estas opciones pueden verse de forma permanente como una barra de herramientas mediante la acción <B>Mostrar Menu SubFormulario <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/48x48/togglemenu.png" WIDTH="22"/></B> Recuerde que el botón derecho del ratón puede ser su mejor aliado en la búsqueda de determinadas acciones.</P>
<H2><IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_delivery_note.svg" WIDTH="32"/>Nuevo Albarán</H2>
<P>Al crear un nuevo albaran los datos necesarios a rellenar son el cliente y el almacen. El resto de campos pueden
dejarse en blanco para ser rellenados posteriormente. Los campos que aparecen en la pantalla pueden variar según la configuración
de plugins que tengamos y que precisen de información adicional en los albaranes, dicha información puede ser
obligatoria o no dependiendo del plugin en cuestión</P>
<P>Una vez rellenados los datos podemos guardar el albaran o darle al boton <B>Aceptar</B> para almacenar los cambios</P>
<P>Todos los albaranes tienen una plantilla de impresión por defecto llamada <B>albaran.rml</B> que conforma la forma en que se
imprime el albaran al pulsar sobre el botón imprimir. Además pueden existir informes adicionales que aparecen al pulsar
con el botón derecho del ratón en cualquier espacio vacio de la pantalla del albaran así como acciones ocultas no
accesibles de otro modo</P>
<B>Author:</B>Tomeu Borrás (tborras@conetxia.com)<BR>
<HR>

ParmBD:

ArchivoSQLpatch:

ArchivoSQLdeins: 

Dependencias: libpluginbf_cliente.so

Incompatibles:

Reports: albarancliente.rml; albaran.rml; blform_AlbaranClienteView.qs

Categorias: Facturacion generica;

Orden: 622

Comentarios:
