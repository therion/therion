::msgcat::mcset en "xtherion_help_control" {

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
}

::msgcat::mcset es "xtherion_help_control" {

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
}

::msgcat::mcset it "xtherion_help_control" {

SHORTCUT - EDITOR DI MAPPA
  
Shortcut generali
 * Ctrl+Z = torna indietro
 * Ctrl+Y = rifai
 * F9 = compila il progetto
 * per selezionare un oggetto in una lista con la tastiera:
   muovi con il tasto "Tab" nella lista desidearta;
   muovi il cursore sull'oggetto desiderato;
   premi il tasto "Spazio"

Area di disegno e immagini di background
 * Tasto destro = scrolla l'area di disegno
 * Doppio click col tasto destro su una immagine = muovi l'immagine
 
Inserimento scrap
 * premi "Ctrl-R" oppure "Edita" > "Inserisci" > "scrap" per inserire un nuovo scrap
 * il nuovo scrap viene inserito immediatamente dopo quello attivo

Inserimento punto
 * "Ctrl-P" = cambia alla modalità di inserimento punti
 * Tasto sinistro = inserisci un punto nella posizione del mouse
 * Ctrl + tasto sinistro = inserisci un punto vicino ad uno esistente
   (di solito esso viene inserito esattamente sopra quello esistente)
 * "Esc" = esci dalla modalità di inserimento punti

Editare punti
 * Trascina con tasto sinistro = muovi il punto
 * Trascina con Ctrl + tasto sinistro = muovi il punto vicino ad uno esistente
   (solitamente viene poszionato proprio sopra quello esistente)
 * Trascina con tasto sinistro per la punta delle freccie = cambia l'orientazione
   o la dimensione (dipendentemente dallo switch nel "Controllo Punti" del pannello)

Inserimento linee
 * "Ctrl-L" = entra in modalità inserimento linee ed inserisci una nuova linea
 * Tasto sinistro = inserisci un nuovo punto della linea (senza punti di controllo)
 * Ctrl + tasto sinistro = inserisci un nuovo punto della linea vicino ad uno
   esistente (normalmente sopra di esso)
 * Trascina con tasto sinistro = inserisci un nuovo punto della linea (con punti di
   controllo)
 * tieni premuto "Ctrl" durante in trascinamento = fissa la distanza del precedente 
   punto di controllo
 * Trascina con tasto sinistro su un punto di controllo = muovi la sua posizione
 * Tasto destro su uno dei punti precedenti = seleziona il punto precedente mentre
   sei in modaliatà di inserimento (utile se vuoi cambiare anche la direzione
   del precedente punto di controllo)
 * "Esc" oppure tasto sinistro sull'ultimo punto = fine inserimento linea
 * Tasto sinistro sul primo punto = chiudi la linea e fine inserimento

Editare linee
 * Tasto sinistro e trascina = sposta il punto della linea
 * Ctrl + tasto sinistro e trascina = sposta il punto dell alinea vicino ad un
   punto esistente (di solito proprio sopra di esso)
 * Tasto sinistro su un punto di controllo e trascina = sposta il punto di controllo

Aggiungere punti di linea
 * seleziona il punto prima del quale vuoi inserire dei punti;
   inserisci i punti desiderati;
   premi "Esc" o tasto sinstro sul punto selezionato all'inizio
  
Rimuovere un punto di linea
 * seleziona il punto che vuoi rimuovere;
   premi "Edita linea" > "Rimuovi punto" nel Controllo Linee del pannello
    
Dividere una linea
 * seleziona il punto dove vuoi dividere la linea;
   premi "Edita linea" > "Dividi linea"  nel Controllo Linee del pannello
    
Inserimento aree
 * premi "Ctrl-A" oppure "Edita" > "Inserisci" > "area" per passare alla
   modalità di inserimento bordo area
 * Tasto destro sulle linee, che contornano l'area desiderata
 * "Esc" per terminare l'inserimento dell'area

Editare aree
 * seleziona l'area che vuoi editare
 * premi "Inserisci" nel Controllo Aree per inserire altre linee di bordo
   nella posizione del cursore
 * premi "Inserisci ID" per inserire una linea con un dato ID nella 
   posizione del cursore
 * premi "Rimuovi" per rimuovere dal bordo dell'area la linea di bordo 
   selezionata
    
Selezionare un oggetto
 * Tasto sinistro = selezione l'oggetto che sta sopra
 * Tasto destro = seleziona l'oggetto appena sotto quello gia` selezionato
   (utile quando parecchi punti giaciono uno sopra l'altro)
}


::msgcat::mcset ru "xtherion_help_control" {
﻿КЛАВИАТУРНЫЕ СОКРАЩЕНИЯ И ОПЕРАЦИИ В РЕДАКТОРЕ КАРТ

LeftClick         - щелчек левой кнопкой мыши
Double LeftClick  - двойной щелчек левой кнопкой мыши
RightClick        - щелчек правой кнопкой мыши
Double RightClick - двойной щелчек правой кнопкой мыши
drag              - перетаскивание: удерживая указанную кнопку мыши
                    (и/или клавиатуры) перемещайте мышь

Общее
 * Ctrl+Z = отмена действия (undo)
 * Ctrl+Y = вернуть отмененное действие (redo)
 * F9 = компиляция текущего проекта
 * Для выбора в списках:
    - переместитесь к нужному элементу управления с помощью "Tab";
    - выбирайте нужный элемент списка (он подчеркнут) стрелками;
    - подтвердите выбор нажатием "Enter"

Область рисования и фоновые изображения
 * RightClick + drag = скроллинг области рисования
 * Double RightClick на изображении + drag = перемещение изображения

Вставка скрапа
 * Нажмите "Ctrl-r" или "Действие" > "Вставить скрап" на панели "Команды в файле"
 * новый скрап вставляется в после текущего

Вставка точки
 * Ctrl+P = переключение в режим `вставка точки'
 * LeftClick = вставка точки в позицию курсора
 * Ctrl+LeftClick = вставка точки близко с другой точкой (без Ctrl
   точка "притягивается" к уже существующей точке)
 * Esc = выйти из режима `вставка точки'

Редактирование точки
 * LeftClick + drag = перемещает точку
 * Ctrl+LeftClick + drag = перемещение точки близко от существующей
   точки (без Ctrl точка "притягивается" к уже существующей точке)
 * LeftClick + drag над стрелкой = изменение ориентации точки
   (ориентация должна быть включена на панели "Точка")

Вставка линии
 * Crtl+L = Вставка новой линии и переход в режим `вставка точек линии'
 * LeftClick = вставка точки линии в позицию курсора (без точек сглаживания)
 * Ctrl+LeftClick = вставка точки линии близко от существующей (без Ctrl точка
   "притягивается" к уже существующей точке)
 * LeftClick + drag = вставка точки линии (с точками сглаживания)
 * удерживая Ctrl во время перемещения точки сглаживания = перемещает только
   данную точку сглаживания
 * LeftClick + drag на точке сглаживания = изменение ее позиции
 * RightClick на одной из предыдущих точек текущей линии в режиме `вставка точек линии'
   = выбрать существующую точку, полезно для редактирования точек сглаживания
 * Esc или LeftClick на последней точке = закончить вставку точек линии
 * LeftClick на первой точке линии = замкнуть начало линии на конец и выйти из режима
   вставки

Редактирование линии
 * LeftClick + drag = перемещение точки линии
 * Ctrl+LeftClick + drag = перемещение точки линии близко от существующей
   (без Ctrl точка "притягивается" к уже существующей точке)
 * LeftClick на точке сглаживания + drag = перемещение точки сглаживания

Добавление точки к существующей линии
 * выбрать точку перед которой вы хотите вставить новую точку;
    - перейдите в режим вставки точки (панель "Линия")
    - вставьте точки (см. Вставка линии);
    - Esc или LeftClick на точке с которой вы начинали = выход из режима вставки

Удаление точки линии
 * выбрать точку линии которую нужно удалить;
    нажмите "Править линию" > "Удалить точку" на панели "Линия"

Зазделение линии
 * выбрать точку в которой вы хотите разделить линию;
    нажмите "Править линию" > "Разделить линию" на панели "Линия"

Вставка области
 * "Ctrl-a" или на панели "Команды в файле" нажмите "Действие" > "Вставить область"
   чтобы переключится в режим "выбор границ области"
 * LeftClick на линии = линия добавляется к списку границ области
 * Esc для того чтобы закончить выбор границ области

Редактировать область
 * Выбрать область для редактирования
 * неажать "Вставка" на панели "Область" для вставки граничных линий в список
   линий после выбраной линии
 * нажать "Вставить по ИД" для вставки линии с указанным идентификатором
 * нажать "Удалить" для удаления выбранной линии из списка гранци области

Выбор существующего объекта
 * LeftClick = выбор верхнего объекта
 * RightClick = перебор объектов закрытых верхним объектом (например полезно, когда
   несколько точек расположены в одной позиции друг над другом)
}
