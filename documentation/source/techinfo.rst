.. technichal_informatio:

Technical information
=====================

vast was written in C/C++ language.

.. _dependecies:

Dependencies
------------

* isis
   The `isis-package <https://github.com/isis-group/isis>`_ provides the io-interface for the viewer. 
   Loading and saving of files and some parts of the data handling is performed by isis.
   It also provides the plugins for the :ref:`supported_file_formats`. This means by adding new 
   io-plugins to isis vast will also benefit from these plugins.

* Qt4
   The `qt4-library <http://qt.nokia.com/>`_ was used to write the GUI and the basic plane view of vast.

* boost
   Some functions of the `boost-library <http://www.boost.org/>`_ were used in the vast project. vast needs boost >= 1.35.
   

.. _supported_file_formats:

Supported file formats
----------------------

As mentioned in the :ref:`dependecies`-section vast supports all file formats as the isis project does. With version **0.4.2** of isis vast supports the following file formats:

+--------------+----------+----------+
| File format  | Readable | Writable |
+==============+==========+==========+
| *Nifti*      |    *     |     *    | 
+--------------+----------+----------+
| *Analyze*    |    *     |     *    |
+--------------+----------+----------+
| *Vista*      |    *     |     *    |
+--------------+----------+----------+
| *Dicom*      |    *     |          |
+--------------+----------+----------+
| *PNG*        |          |     *    |
+--------------+----------+----------+
| *raw*        |    *     |     *    |
+--------------+----------+----------+

Furthermore, vast supports reading of zipped files with the following suffixes:

* tar.bz2
* tar.gz
* tar.Z
* tar
* tgz
* tbz
* taz

.. _supported_data_types:

Supported data types
--------------------

vast supports the following data types:

* bool
* s8bit
* u8bit
* s16bit
* u16bit
* s32bit
* u32bit
* s64bit
* u64bit
* float
* double
* color24
* color48


Graphics system
---------------

If vast was build with Qt (>=4.5) it is capable of setting the graphics system. 
This allows vast to be run in a **raster** graphics system mode which leads to much faster and smoother image rendering.

The graphics system can be modified by using the environment variable *VAST_GRAPHICS_SYSTEM*.

The following values are accepted:

* raster (standard)
   Hardware accelerated graphics mode. 

* native 
   Software rendering. This mode is also used if vast was build with Qt(<4.5).

* opengl
   This is an experimental graphics system mode of Qt which uses `OpenGl <http://www.opengl.org/>`_ for rendering purpose.

On UNIX platforms the graphics system can be changed by typing:

   *export VAST_GRAPHICS_SYSTEM="raster"*

