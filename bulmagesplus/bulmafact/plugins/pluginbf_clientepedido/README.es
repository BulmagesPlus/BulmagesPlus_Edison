#######
#Plugins
#######
#README.es
#20080501

Nombre: Pedidos de clientes

Biblioteca: libpluginbf_clientepedido.so

Descripción:  <H1><IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_delivery_order.svg" WIDTH="100"/><BR>Pedidos de clientes</H1>
<P>Soporte para emisión de pedidos de clientes.</P>
<P>Al cargar el plugin tendremos dos nuevas entradas de meńu <B>Listado de Pedidos <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_delivery_order_list.svg" WIDTH="22"/></B> desde la que accederemos al listado de pedidos emitidos y <B>Nuevo Pedido <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_delivery_order.svg" WIDTH="22"/></B> desde el que podremos emitir un nuevo pedido.</P>
<H2><IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_delivery_order.svg" WIDTH="22"/>Listado de Pedidos</H2>
<P> Mediante el listado de pedidos podremos controlar todos los pedidos emitidos, editarlos y borrarlos. Para ello podemos utilizar la barra de botones superior del listado.</P>
<P> Es común en todos los listados de BulmaGés poder configurarlos presentando los campos que nos interese. Para ello tenemos el <B>menú del subformulario <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/48x48/toggleconfig.png" WIDTH="22"/></B> o la opción <B>Configurar Listado <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/document_properties.svg" WIDTH="22"/></B> que nos presenta en el lateral derecho del listado todos los campos mostrables por el mismo, opciones de paginación y la consulta de base de datos que genera dicho listado</P>
<P><B>Nota:</B>Puede haber opciones ocultas para usted que se activan mediante la pulsación del botón derecho del ratón en determinadas zonas. Estas opciones pueden verse de forma permanente como una barra de herramientas mediante la acción <B>Mostrar Menu SubFormulario <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/48x48/togglemenu.png" WIDTH="22"/></B> Recuerde que el botón derecho del ratón puede ser su mejor aliado en la búsqueda de determinadas acciones.</P>
<H2><IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_delivery_order.svg" WIDTH="32"/>Nuevo Pedido</H2>
<P>Al crear un nuevo pedido los datos necesarios a rellenar son el cliente y el almacen. El resto de campos pueden
dejarse en blanco para ser rellenados posteriormente. Los campos que aparecen en la pantalla pueden variar según la configuración
de plugins que tengamos y que precisen de información adicional en los pedidos, dicha información puede ser
obligatoria o no dependiendo del plugin en cuestión</P>
<P>Una vez rellenados los datos podemos guardar el pedido o darle al boton <B>Aceptar</B> para almacenar los cambios</P>
<P>Todos los pedidos tienen una plantilla de impresión por defecto llamada <B>pedido.rml</B> que conforma la forma en que se
imprime el pedido al pulsar sobre el botón imprimir. Además pueden existir informes adicionales que aparecen al pulsar
con el botón derecho del ratón en cualquier espacio vacio de la pantalla del pedido así como acciones ocultas no
accesibles de otro modo</P>
<B>Author:</B>Tomeu Borrás (tborras@conetxia.com)<BR>
<HR>

ParmBD:

ArchivoSQLpatch:

ArchivoSQLdeins: 

Dependencias: libpluginbf_cliente.so

Incompatibles:

Categorias: Facturacion generica;

Reports: blform_PedidoClienteView.qs

Orden: 621

Comentarios:
