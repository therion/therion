::msgcat::mcset en "xtherion_help_control" [encoding convertfrom utf-8 {

MAP EDITOR SHORTCUTS
  
General shortcuts
 * Ctrl+Z = undo
 * Ctrl+Y = redo
 * F9 = compile current project
 * to select object in the listbox using keyboard:
    switch using "Tab" into desired listbox;
    move with underlined cursor to desired object;
    press "Space"

Drawing area and background images
 * RightClick = scroll drawing area
 * Double RightClick on the image = move the image
 
Inserting scrap
 * press "Ctrl-r" or "Edit" > "Insert" > "scrap" to insert new scrap
 * new scrap is inserted just after the current one

Inserting point
 * Ctrl+P = switch to `insert point' mode
 * LeftClick = insert point at given position
 * Ctrl+LeftClick = insert point very close to existing point (normally it
    will be inserted right above the closest point)
 * Esc = escape from the `inset point' mode

Editing point
 * LeftClick + drag = move point
 * Ctrl+LeftClick + drag = move point close to the existing
    point (normally it is moved right above closest existing point)
 * LeftClick + drag on point arrows = change point orientation or
    sizes (according to given switches in Point cotrol panel)

Inserting line
 * Crtl+L = insert new line and enter an `insert line point' mode
 * LeftClick = insert line point (without control points)
 * Ctrl+LeftClick = insert line point very close to existing point 
    (normally it's inserted right above closest existing point)
 * LeftClick + drag = insert line point (with control points)
 * hold Ctrl while dragging = fix the distance of previous control point
 * LeftClick + drag on the control point = move its position
 * RightClick on one of the previous points = selects the previous point while 
    in insert mode (useful if you want to change also the direction of
    previous control point)
 * Esc or LeftClick on the last point = end the line insertion
 * LeftClick on the first line point = close the line and end line insertion

Editing line
 * LeftClick + drag = move line point
 * Ctrl+LeftClick + drag = move line point close to the existing
    point (normally it is moved right above closest existing point)
 * LeftClick on control point + drag = move control point

Adding line point
 * select the point before which you want to insert points;
    insert required points;
    press Esc or left-click on the point you selected at the begining
  
Deleting line point
 * select the point you want to delete;
    press "Edit line" > "Delete point" in the Line control panel
    
Splitting line
 * select the point at which you want to split the line;
    press "Edit line" > "Split line" in the Line control panel
    
Inserting area
 * press "Ctrl-a" or "Edit" > "Insert" > "area" to switch to 
    the "insert area border" mode
 * RightClick on the lines, that suround desired area
 * Esc to finish area border lines insertion

Editing area
 * select area you want to edit
 * pres "Insert" in the area control to insert other border lines
    at current cursor position
 * pres "Insert ID" to insert border with given ID at current cursor position
 * pres "Delete" to remove selected area border line
    
Selecting an existing object
 * LeftClick = select object on the top
 * RightClick = select object right below the top object (useful when several
    points lie above each other)
}]

::msgcat::mcset es "xtherion_help_control" [encoding convertfrom utf-8 {

ATAJOS DE TECLADO PARA EDITAR MAPAS

Atajos generales
 * Ctrl+Z = Deshacer
 * Ctrl+Y = Rehacer
 * F9 = Compilar proyecto actual
 * para seleccionar en los menús usando el teclado:
   Pulsar "Tab" hasta seleccionar el menú deseado;
   Moverse con las flechas hasta el objeto deseado;
   pulsar "Espacio"
   
Area de dibujo e imáges de fondo
 * Click Dcho = desplazar área de dibujo
 * Doble click dcho en la imagen = mover la imagen

Insertar croquis
 * Pulsar "Ctrl-r" o "Editar" > "Insertar" > "Croquis" para insertar un croquis nuevo
 * el nuevo croquis se insertará justo detrás del actual

Insertar punto
 * CTRL+P = cambiar a modo 'insertar punto'
 * Click = insertar punto en esa posición
 * Ctrl+Click = insertar punto muy cerca de otro punto
   (normalmente se insertará justo encima del punto más cercano)
 * Esc = salir del modo 'insertar punto'

Editar punto
 * Click en punto + arrastrar = mover el punto
 * Ctrl + Click en punto + Arrastrar = mover el punto cerca de otro
 * normalmente es movido justo encima del punto más cercano
 * Click en la punta de una flecha + arrastrar = cambiar la orientación o
   el tamaño (segun lo indicado en el panel de control Punto)

Insertar línea
 * Ctrl+L = insertar nueva línea y pasar al modo "insertar puntos de línea"
 * Click = insertar punto de línea (sin puntos de control)
 * Ctrl+Click = Insertar punto de línea muy cerca de otro
   (Normalmente se inserta justo encima del punto más cercano)
 * Click + arrastrar = insertar punto de línea (con puntos de control)
 * Pulsar Ctrl mientras se arrastra = fijar la distancia al punto de control anterior
 * Arrastra punto de control = cambir su posición
 * Click dcho en uno de los puntos anteriores = seleccionar ese punto anterior cuando
   estamos en modo inserción (Util si se quiere cambiar también la dirección
   del punto de control anterior)
 * Esc o Click en último punto de la línea = finaliza la inserción de línea
 * Click en el primer punto de la línea = cierra la línea y finaliza la inserción de línea

Editar línea
 * Click en punto + arrastrar = mover punto de línea
 * Ctrl+Click+arrastrar = mover punto de línea cerca de otro punto)
   (normalmente se mueve justo encima del punto más cercano)
 * Click en punto de control + arrastrar = mover punto de control

Añadir puntos de línea
 * seleccionar el punto antes del cual se quieren insertar puntos
 * insertar los puntos que hagan falta
 * pulsar Esc o hacer click en el punto que se seleccionó al principio

Borrar puntos de línea
 * seleccionar el punto que se quiere borrar
 * pulsar "Editar" > "Borrar punto" en el panel de control Líneas

Partir líneas
 * seleccionar el punto en que se quiere cortar la línea
 * pulsar "Editar" > "Cortar línea" en el panel de control Líneas

Insertar áreas
 * pulsar Ctrl-a o "Editar" > "Insertar" > área en menú principal
 * para cambiar al modo "insertar límite de área"
 * Click dcho en las líneas que limitarán el área
 * pulsar Esc para teminar la inserción de líneas límite

Editar áreas
 * seleccionar el área que se quiere editar
 * pulsar "Insertar" en el control area para insertar otras líneas límite
   en la posición del cursor
 * pulsar "Insertar ID" para insertar un límite con el ID que se indique en l posición del cursor
 * Pulsar "Borrar" para eliminae la línea límite seleccionada

Seleccionar objetos
 * Click = seleccionar el objeto que está sobre los demás
 * Click dcho = seleccionar el objeto que está en segundo plano
   (útil cuando varios puntos están superpuestos)
}]

