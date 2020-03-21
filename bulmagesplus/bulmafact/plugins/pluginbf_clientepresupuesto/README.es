bu#######
#Plugins
#######
#README.es
#20080501

Nombre: Presupuestos a clientes

Biblioteca: libpluginbf_clientepresupuesto.so

Descripción:  <H1><IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_quote.svg" WIDTH="100"/><BR>Presupuestos a clientes</H1>
<P>Soporte para emisión de presupuestos a clientes.</P>
<P>Al cargar el plugin tendremos dos nuevas entradas de meńu <B>Listado de Presupuestos <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_quote_list.svg" WIDTH="22"/></B> desde la que accederemos al listado de presupuestos emitidos y <B>Nuevo Presupuesto <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_quote.svg" WIDTH="22"/></B> desde el que podremos emitir un nuevo presupuesto.</P>
<P> Mediante el listado de presupuestos podremos controlar todos los presupuestos emitidos, editarlos y borrarlos. Para ello podemos utilizar la barra de botones superior del listado.</P>
<P> Es común en todos los listados de BulmaGés poder configurarlos presentando los campos que nos interese. Para ello tenemos el <B>menú del subformulario <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/48x48/toggleconfig.png" WIDTH="22"/></B> o la opción <B>Configurar Listado <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/document_properties.svg" WIDTH="22"/></B> que nos presenta en el lateral derecho del listado todos los campos mostrables por el mismo, opciones de paginación y la consulta de base de datos que genera dicho listado</P>
<P><B>Nota:</B>Puede haber opciones ocultas para usted que se activan mediante la pulsación del botón derecho del ratón en determinadas zonas. Estas opciones pueden verse de forma permanente como una barra de herramientas mediante la acción <B>Mostrar Menu SubFormulario <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/48x48/togglemenu.png" WIDTH="22"/></B> Recuerde que el botón derecho del ratón puede ser su mejor aliado en la búsqueda de determinadas acciones.</P>
<P>Al crear un nuevo presupuesto los datos necesarios a rellenar son el cliente y el almacen. El resto de campos pueden
dejarse en blanco para ser rellenados posteriormente. Los campos que aparecen en la pantalla pueden variar según la configuración
de plugins que tengamos y que precisen de información adicional en los presupuestos, dicha información puede ser
obligatoria o no dependiendo del plugin en cuestión</P>
<P>Una vez rellenados los datos podemos guardar el presupuesto o darle al boton <B>Aceptar</B> para almacenar los cambios</P>
<P>Todos los presupuestos tienen una plantilla de impresión por defecto llamada <B>presupuesto.rml</B> que conforma la forma en que se
imprime el presupuesto al pulsar sobre el botón imprimir. Además pueden existir informes adicionales que aparecen al pulsar
con el botón derecho del ratón en cualquier espacio vacio de la pantalla del presupuesto así como acciones ocultas no
accesibles de otro modo</P>
<B>Author:</B>Tomeu Borrás (tborras@conetxia.com)<BR>
<HR>

ParmBD:

ArchivoSQLpatch:

ArchivoSQLdeins: 

Dependencias: libpluginbf_trabajador.so; libpluginbf_almacen.so; libpluginbf_formasdepago.so; libpluginbf_cliente.so

Incompatibles:

Reports: presupuesto.rml; blform_PresupuestoView.qs; impers_presupuesto_fproforma.rml

Categorias: Facturacion generica;

Orden: 620

Comentarios:
