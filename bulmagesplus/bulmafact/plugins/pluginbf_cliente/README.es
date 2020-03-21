#######
#Plugins
#######
#README.es
#20080501

Nombre: Clientes

Biblioteca: libpluginbf_cliente.so

Descripción: <H1><IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client.svg" WIDTH="100"/><BR>Gestion de clientes</H1>
<P> Permite realizar la gestion de clientes de la empresa. Mostrando el listado de clientes y permitiendo
hacer altas, bajas y modificaciones</P>
<P> Este plugin es imprescindible para todos aquellos modelos de negocio donde se trabaje con clientes</P>
<P> Tambien sirve como base muchos otros plugins que extienden la capacidad de éste</P>
<P> Una vez instalado el plugin tendremos dos nuevas opciones en el menú <B>Listado de clientes</B> y <B>Nuevo Cliente</B></P>
<H2> <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client_list.svg" WIDTH="22"/>Listado de Clientes</H2>
<P> Esta pantalla nos muestra los clientes dados de alta en la empresa. Desde aqui podremos crear, modificar o borrar los clientes mediante los botones
situados en la parte superior de la ventana. Además podremos obtener en forma de listado los datos más importantes
de nuestros clientes pudiendo configurar su presentación para ser impresos o exportados a otros formatos.</P>
<P> Desde esta ventana además podremos establecer opciones de filtrado con las que podremos ver aquellos
clientes que cumplan unas características determinadas.</P>
<H2> <IMG SRC="@CMAKE_INSTALL_PREFIX@/share/bulmages/imagesrc/images/oxygen/actions/client.svg" WIDTH="22"/>Nuevo Cliente</H2>
<P>Mediante esta opción podremos crear un nuevo cliente. Al iniciar esta acción aparecerá una nueva ventana que nos
presentará todos los campos a rellenar del cliente. Es imprescindible poner el cif del cliente para que el cliente esté
bien creado. Una vez rellenados los campos podremos guardar la ficha o darle al botón aceptar para tener los datos 
introducidos en el sistema. Si por el contrario pulsamos sobre cancelar podremos cerrar la ventana sin guardar los cambios</P>
<B>Author:</B>Tomeu Borrás (tborras@conetxia.com)<BR>
<HR>

ParmBD:

ArchivoSQLpatch:

ArchivoSQLdeins: 

Dependencias:

Incompatibles:

Orden: 300

Reports: inf_clientes.rml; informecliente.rml; informeclientes.rml; pedidocliente.rml; blform_ClienteView.qs; inf_detalleventascliente.pys; inf_clientes.pys

Categorias: Facturacion generica;

Comentarios:
