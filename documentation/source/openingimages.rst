.. Opening images:

Opening images in vast
======================

There are two ways to open images with vast. It can be done by :ref:`usingterminal` or by clicking the vast symbol on the desktop or a menu-entry and :ref:`usingvastopendialog`.

.. _usingterminal:

Using a terminal
----------------

To get an overview of the commands and options you can pass to vast you can type:

   ``# vast --help``

This also gives you a list of all supported file formats. The simplest way to open vast without any images is to type:

   ``# vast``

Then you have to use the vast open dialog to load images ( :ref:`usingvastopendialog` )

Opening files
`````````````

To open simple files ( e.g. Nifti, Vista ) all you have to type is:

   ``# vast -in iamge.nii``

It is also possible to open a list of images:

   ``# vast -in image1.nii image2.v image3.nii.gz`` [#f1]_

This will open vast and load all **3** images in **1** widget-ensemble [#f2]_. So the 3 images are displayed superimposed. 

.. image:: _images/opening_images_superimposed.jpg 
   :width: 600


If you want to vast to display each image in one widget you simply have to type:

   ``# vast -in image1.nii image2.v image3.nii.gz -dist`` [#f1]_
   
This will open vast, load all **3** images and distribute them among **3** widget-ensembles [#f2]_. 

.. image:: _images/opening_images_splitted.jpg 
   :width: 600


.. _usingvastopendialog:

Using the vast open dialog
--------------------------

| If you are not familiar with using the terminal or just like clicking you can use the open dialog of vast. 
| It can be showed by pressing either *Ctrl+O* or *File->Open Image(s)...*.

.. image:: _images/open_dialog_all_options.jpg
   :width: 600

The above image shows the open dialog with the *basic section* (green), the *advanced option section* (blue) and the *favorites section* (orange). 
The two later ones can be hidden by clicking the respective check box buttons.

Basic section
`````````````

First, we will take a look at the basic section which is the one you will need at each time using the open dialog.

.. image:: _images/open_dialog_basic_section.jpg
   :width: 600

What you have to do first is to specify whether you want to open a simple file (**1**) (in most cases you will choose this option) or a directory (**2**) (with all its containing images).
This option also influences the type of the file browse dialog which will be opened if you click *browse* button (**4**).

Then you have two options to specify the file(s) or directory want to open:

* by typing the path to the file/directory into the text field (**3**)
* choosing the file(s)/directory with a file browser (**4**)

Once specified the file(s)/directory vast will instantly check the existence and file type. If vast thinks it can not open it the path will be colored red. 
Otherwise it will be colored green.


Advanced section
````````````````

.. image:: _images/open_dialog_advanced_section.jpg
   :width: 600
   
If you do not see the advanced section you can make it visible by clicking the check box button (**1**).



.. [#f1] You can mix any file formats vast is capable of reading.
.. [#f2] A widget-ensemble is simply a set of 3 widgets (axial, sagittal and coronal).