# AtlasEngine

## Descripción
**AtlasEngine** es un motor de videojuegos 3D desarrollado en **C++** con **OpenGL**, capaz de renderizar geometría y gestionar escenas con múltiples herramientas de edición y visualización.

El proyecto está en desarrollo activo, y el código está disponible en [GitHub](https://github.com/Serfercont/AtlasEngineFinal).

## Miembros del equipo
- [Sergio Fernandez Contreras](https://github.com/Serfercont)
- [Pau Blanco Valle](https://github.com/PauloWhite2004)
- [Carlos Gonzalez Subiron](https://github.com/gosu00)

## Controles del visor
- **Click derecho**: Mover la vista actual.
- **Click derecho + WASD**: Volar por la escena.
- **Alt + Click derecho o Rueda del ratón**: Acercar/alejar.
- **Alt + Click izquierdo**: Orbitar la cámara alrededor de un punto.
- **Alt + Rueda del ratón**: Panorámica de la cámara.
- **F**: Enfocar el objeto seleccionado.

## Características principales
- **Importación de Modelos**: Compatible con formato FBX.
- **Importación de Texturas**: Soporte para formatos PNG y DDS.
- **Arrastrar y Soltar**: Permite importar archivos fácilmente.
- **Transformaciones**: Modificación de posición, rotación y escala de modelos.
- **Sistema de Logging**: Registro detallado de actividades.
- **Librería de Archivos Personalizada**: Gestión optimizada de recursos.
- **Modos de Sombreado**: Cambiar entre sombreado, wireframe y combinaciones.
- **Monitor del Motor**: Visualización de estadísticas en tiempo real.

## Paneles del Editor
- **Jerarquía**: Gestiona todos los objetos en la escena con funciones de búsqueda, creación de objetos vacíos y primitivas básicas.
- **Inspector**: Permite modificar propiedades como transformación, malla y texturas de los objetos seleccionados.
- **Escena**: Interactúa visualmente con el mundo.
- **Assets**: Visualiza y organiza recursos del proyecto.
- **Consola**: Registro de errores, advertencias e información.
- **Preferencias**: Ajustes de pantalla, grilla y opciones de renderizado.
- **Performance**: Información sobre el rendimiento del motor.
- **Acerca de**: Información general del motor.
- **Game**: Pantalla por la que se ve el prototipo
- **Time Manager**: Controladores para ejecutar, pausar y cerrar el juego. Información del Time Manager.

## Nuevas Funcionalidades
- **Gestión de Recursos**: Visualización de referencias y opciones avanzadas de importación.
- **AABB y OBB**: Visualización de las cajas de los objetos.
- **Octree**: Octree para la carga.
- **Camara GameObject**: Creación de la cámara como un objeto para visualizar el Juego en la ventana "Game".
- **Mouse Picking**: Con el ratón en el editor se puede seleccionar los distintos objetos de la escena.
- **Frustum Culling**: Optimización con visualización en el editor.
- **Simulación**: Iniciar, pausar y detener con retorno al estado inicial.
- **Inspector Avanzado**: Control de componentes como cámaras y materiales.

## Dependencias
- [SDL2](https://github.com/libsdl-org/SDL)
- [OpenGL](https://www.opengl.org/)
- [ImGui](https://github.com/ocornut/imgui)
- [GLM](https://github.com/g-truc/glm)
- [Assimp](https://github.com/assimp/assimp)
- [DeviL](https://github.com/DentonW/DevIL)

## Comentarios adicionales
- **Referencia a otros equipos**: Este proyecto partió del código entregado en la entrega 1 de SpectraEngine. Que con el permiso del profesor y de los compañeros de clase, pudimos hacer un fork del modelo del motor de la entrega 1.

Para más información o comentarios específicos, revisa el código fuente y la documentación adicional en el repositorio.

