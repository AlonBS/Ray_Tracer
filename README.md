# Ray_Tracer
Computer Graphics Ray Tracer (CPU) based on Edx CG-course.


Current list of features supported:
====================================
- Intersections with primitives: Triangles, Spheres, Cylinders, Boxs, ..., 
- Transformations: Scale, Rotation and translate. Also support transformations stacks (for transforms hierarchy) 
- Blinn-Phong illumination module - including normals interpolation and attenuation.
- Rendering of models comprised of multiple meshes with (partial) material properties. 
- Textures to primitives and models (even multiple textures types - such as diffuse-texture etc.). 
- Point lights, directional lights. 
- Recursive lighting calculations. Max recursion depth is currently limited to 5. 
- Multi-core rendering and Single-Core rendering. 
- Command line argument parse
- Saved image formats supported: .png, .jpeg, .jpg, .bmp, .tiff




Installation (For Ubuntu and Debian Distros)
=============================================

1. Clone repository from my github: git clone https://github.com/AlonBS/Ray_Tracer.git
2. Install packages: sudo apt-get install libfreeimage3 libfreeimage-dev libglm-dev libglew-dev libboost-all-dev 
	 	     freeglut3-dev libassimp-dev
3. cd to installation directory and use Make: make clean && make -j 


Usage: 
======

basic: ./ray_tracer -i <input>
or run ./ray_tracer -h to get full list of options and arguments:

  -h [ --help ]           show this message
  -v [ --version ]        Current Version of the program
  -i [ --input ] arg      The path to a scene file, or scenes directory 
                          following the syntax specific within README file.
			  Applies only to direct directory. No recursive 
                          sub-directories search is performed.
  -o [ --output ] arg     Optional - The directory where to save the results. 
                          If not specified, results will be saved in the same 
                          directory of input files.
  --format arg (=.png)    Optional - The file format in which scenes result 
                          will be saved. Currently supported types are: png, 
                          jpeg, jpg, bmp, tiff.
  -s [ --single-thread ]  Flag to force single thread rendering. Default 
                          behavior is multi-threaded.


Other values are specified within each scene separately. 



Scene Keywords and Syntax:
==========================
The general syntax is <Command> [Argument_1] [Argument_n] - each argument is separated by a single white space.
Arguments can have various types, each one will be added a suffix, indicating the exact type, where:
	'_i' - integer
	'_f' - float
	'_v3' - vector3 (that is, 3 float numbers)
	'_v2' - vector2
	'_s' - string.
Note the <Command> keywords are case sensitive, and are written in C-naming conventions. 

Scene General:
--------------
	size <width_i> <height_i> - The size of the result in pixels. Example: size 640 480
	maxdepth <depth_i> - The maximum depth of the recursion of the ray tracer. Maximum value allowed is 5, and
        	default value is 2. Note that, for performance considerations, if the contribution to the light was
		neglegent, the recursion will stop even if higher number was requested. 
 
	
Camera:
--------------
	camera <lookfrom_v3> <lookat_v3> <up_v3> <fov_f> - Where lookfrom, lookat and up are 3 dimensional vectors each,
        	representing the camera position, the direction, and the orientation, respectively. Finally, fov is the
		field of view (in Y direction). Example: camera 0 0 -10 0 0 0 0 1 0 45.


Primitives and Models:
-----------------------
	sphere <position_v3> <radius_f>: Creates a sphere at the given position, with a radius of the specified size.
		Note that the position is in world-coordinates. Which means the transformations applied to this object
		might cause it to render in another location. This is true for all primitives and models.
	cylinder <position_v3> <height> <radius_f>: Creates a finite cylinder centered at position, with the given height and radius.
		This means that the height define the caps of the cylinder. 
	box <min_bound_v3> <max_bound_v3> - Define an axis aligned box with minimum vertex at min_bound and max vertex at max_bound.
		transformed just like any other object. Also supports textures. 
	vertex <pos-v3> - Creates a single vertex, we can later be used to create a triangle.
	vertexNormal <pos-v3> <norm-v3> - Defined a vertex as before, but also defined the surface normal.
		Note the vertecies defined in this way are completely indepetnted of vertecies defined using the
		'vertex' command. Meaning - you  can't create a mixed triangle.
	vertexTex <pos-v3> <tex-v2> - Creates a single vertex, and associate it with textures mapping (values 
		between 0 to 1). The right pixel will be direved according to this values, and with regards to the
		currently bound texture (see below). 
	tri <vertex_1_index_i> <vertex_2_index_i> <vertex_3_index_i> - Creates a triangle using 3 vertices
		previously defined (using 'vertex' command. The numbering is the same as arrays. 
		The first 'vertex' is indexed 0 etc. 
	triNormal - Creates a triangle from vertices defined us the 'vertexNormal' command.
	triTex - Creates a triangle from vertices defined by the 'vertexTex' command. 
	maxverts <num-i> - Declares the number of vertices that will be declared following this command.
		This is a legacy command, to be compatible with Edx.org/CG specifications. It is not needed. 
	maxVertNorms <num-i> - same as above, but for the vertexNorm command.
	model <path-s> - Creates a model based the path the model files. This is used using ASSIMP library, and it is
		best that the model will be described using .obj files. Other types may not be fully supported. 
		Important Notes:
			1) The format of an .obj file can be found here:
				https://en.wikipedia.org/wiki/Wavefront_.obj_file. This files can be generated using
				most graphical softwares (Blender, Maya, etc.).
			2) The .obj files can reference to a .mtl file (materiel), which can give the described object
				many material properties, many of them are supported, including ambient, diffusive,
				specular lighing, and shininess, as well as textures to each of this components and
				many more. The format of these files can be found here:
				http://paulbourke.net/dataformats/mtl/. 
			3) These files are much easier to understand, and it is recommended for the user to play with
				these values to get the desired affect on the model being rendered. In particular,
				one can apply various texture to each part of the object, of different types
				(specular texture for intance). Using command "map_Ks <file_name>" where file_name is
				the name of the file, relative to the .mtl object. 
			4) This means that an object can have different textures applied to each of its component,
				as well as a single texture applied to them all (using the 'texture and bindTexture'
				mechanism, within the scene file).
			5) Free 3d modules can be found on various places on the internet. Make sure the types are
				supported, and try to integrate them in the scene. Note that in many cases, these files
				contain a very large numbers of polygons, which means the the render time can take
				much longer. Also note that usually these objects tend to be very large, so scaling is
				needed (See transformations section below). 
	texture <path_s> - Loads and saves a texture specified with the given path, relative to the execution directory.
		This is done using FreeImage library, which supports nomerous files formats (JPEG, PNG, TIF, BMP
		to name a few. See full list here: http://freeimage.sourceforge.net/features.html).
		A defined texture will be automatically assigned with an index (starting with 0), for being able to bind
		it to an object (see below).
	bindTextrue <index_i> - Binds a previously defined texture with the given index. Any object (primitive or model)
		created from this point on, will be assigned with this texture (assuming it supports textures mapping
		and/or those mapping were given before). 
	unbindTexture - unbind the last bound texture. Any object created from this point on, will not have textures
		applied to it. 


Transformations:
-----------------
Transformations are implemented using transformation matrices stack. This allowed the divide the scene into hierarchies,
and apply the same transformation to multiple objects (and lights) at the same time. The transformations stack always
starts with the identity matrix. The commands 'translate', 'rotate, and 'scale' (see below), will apply these
transformations to the top of the transform matrix. Every object (and light) created from this point on, will be
transformed by the transformation matrix at the top of the stack. To allow hierarchies, we can use the pushTransform
which pushes the current transform matrix on the top of the stack, and popTransform, which removes the top matrix. 
An Example of usage: Assume we want to create a sphere with size 1, and we want to create 1 smaller sphere that will
be 1 unit down and right to this sphere, and another smaller sphere that will be 1 unit up and left to this sphere
(regardless of where the original sphere will be transformed to). This is done the following 
way (note the indentation for better readability):

pushTransform
translate 0 5 0 # we want the big sphere to be a 5 y-value
sphere 0 0 0 1
	pushTransform
	translate 1 -1 0 # this is down right by one unit from the big sphere
	sphere 0 0 0 0.5
	popTransform # we remove the last transform (translate)

	pushTransform # we start a new one, based on the top transform - which is of the big sphere
	translate -1 1 0 # this is up left by one unit from the big(!) sphere
	sphere 0 0 0 0.5
	popTransform
popTransform

Important Note: Matrices are 'Right-Multiplies'. So counter to intuition, the last command written, is the first to
happen. That is, if we want to achieve the usual matrices order: Scale the Rotate then Translate: T*R*S, we would right
Translate followed by rotate, followed by scale command. 


	translate <by_v3> - Applies translation matrix to the top of the transformation matrices. 
	rotate <axis_v3> <degrees_f> - Rotate by angles (in degrees) around the given axis.
	scale <ammount_v3> - Scale by the amount given for each axis (meaning - non-uniform scaling is possible). 
	pushTransform - pushes the current transform matrix on top of the transform stack.
	popTransform - removes the top transform matrix. 


Lights:
--------
Lights are also subjected to the current transform on top of the transformation stack. 

	directional <dir_v3> <color_v3> - Creates a directional light with direction <dir> and with color <color>. 
		Color valus should range between 0 to 1.
	point <pos_v3> <color_v3> - Creates a point light at the position <pos> with color <color>. Colors should range
		between 0 to 1.
	attenuation <constant_f> <linear_f> <quadratic_f> - Sets the constant, linear and quadratic coefficients in the
		attenuation equation:
			Atten = 1 / (K_c + (K_l*d) + (K_q*d^2)) where:
			K_c - is the constant factor.
			K_l - is the linear factor.
			K_q - the quadratic factor.
			d - Distance between light source and surface (this means that this only affect point lights,
				and not directional lights). 



Materials:
----------
These commands define the properties of the surface that should be taken into consideration when calculating the
light at the specific point. Currently, the shading model implemented is Blinn-Phong:
https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model. 
They way these are used is first to define the material properties, then any object created after that will get these
properties until they are overwritten. It's not mandatory to specify each value - default values will be used otherwise. 
Note: If complex object is created (using "model" command) - it can have it's own material properties, per part, in which
case, the properties listed here will be multiplied with them. Making these values 'global' in that manner. 

	ambient <color_v3> - The ambient factor. Default vec3(0.2f, 0.2f, 0.2f)
	diffuse  <color_v3> - The diffusive factor. Default vec3(0.0f, 0.0f, 0.0f)
	specular  <color_v3> - The specular factor. Default vec3(0.2f, 0.2f, 0.2f)
	shininess  <color_i> - The shininess factor. Default 0f
	emission  <color_v3> - The emission factor. Default vec3(0.0f, 0.0f, 0.0f)



Scene Example:
--------------
With this repository, I've included examples of many scenes and their rendered result. For simplicity sake,
let's look at a simple scene.
The result image can be found in "./Rendered_Scenes/ExampleScene.png
{
	""""""""""""
	# HELLO Scene - An example scene to show the syntax and usage 

	size 640 480 # The result image will be 640 by 480 pixels.
	camera 0 4 13 0 0 0 0 1 0 45 # create a camera

	attenuation 1 0.0 0.0 # define attenuation factor 

	texture ./scenes/a/wood2.jpeg # declare a texture - this texture is automatically indexed to 0
	texture ./scenes/a/earth.png # indexed to 1

	#directional 1 -1 0 1 0 0 
	# at the point lights
	point 1 7 5 1 1 1
	point -1 7 5 1 1 1
	# defined material properties for the first object to be created. Notice that this is overwritten next, so it has no affect.
	diffuse 0.3 0.3 0.3 
	specular 0.3 0.3 0.3
	shininess 32

	maxVerts 4 #legacy command - not mandatory

	# Define 4 verticies (with texture mapping coordinates) to later simulate a wooden floor
	vertexTex +8 0 +8 1 1 # index is 0
	vertexTex +8 0 -8 0 1 # index is 1
	vertexTex -8 0 +8 1 0 # index is 2
	vertexTex -8 0 -8 0 0 # index is 3

	#vertex +8 0 +8
	#vertex +8 0 -8
	#vertex -8 0 +8
	#vertex -8 0 -8 

	# create the floor
	pushTransform

		bindTexture 0 # we bind the wood2.jpeg texture. every object created from now on, will have
			      # this texture applied (globally)
		ambient 0.1 0.1 0.1 # the object material properties
		diffuse 0.1 0.1 0.1
		specular .8 0.8 0.8
		shininess 16
		triTex 0 1 2 # actually create a triangle with texture. The newly created triangle will have
			     # the bound texture and the last specified materials properties (and transformations)
			     # the verticies to used are by index, which was assigen when those were created (vertexTex cmd)
		triTex 1 3 2

		unbindTexture


	popTransform # no transformations were applied to the floor, but if there were any, this would make sure that the 
		     # following objects won't be affected by this
	pushTransform
		# The indentation is for better readability 
		bindTexture 1 # bind earth.png texture
		diffuse 0.3 0.3 0.3 
		specular 0.0 0.0 0.0
	#	translate 0 1 -2
		sphere -0.5 2.8 -.65 1.5
		scale 3 3 3 # this will have no affect, since the object was already created. If we we're to create the model below,
			    # then it will be scaled by the given factors. 
	#	model ./3ds/cube/cube.obj
		unbindTexture

	popTransform
}

"""""""""""" (End Scene)





Version History:

1.8:
- Added box primitive with full texture support
- Added example scene for box
- Bug fix on scene parser
- Bounding box is still not support. That is, still not used as AABB.

1.7:
- Added cylinder texture support with transormations
- Added example scene with cylinder and texture. See ExampleCylinder (on rendered scenes)

1.6:
- Added cylinder intersection.
- Fixes to rotation transformation on primitives (sphere and cylinder)
- fixes on transformations stack to primitives. 
- fixed scene parser buffered not cleanup bug. 

1.5:
- Major code refactor. Removed Render Info file, and change scene / camera hierarcies.
- Code clean up

1.4:
- Fixed textures not showing in primitives or models.
- Added single thread rendering
- Added example scene + description in README
- minor bug fixes

1.3:
- Added command line arguemnts support.
- Also added brtter error handling. Some minor code cleanup were made. 

1.2:
- Changes mechanism to arguments, and remove output file name from scene.

1.1:
- Moved to a separate git and added readme and instructions. 

1.0:
- Added mesh support

0.9:
- Added textures on basic shapes
- 

0.8:
- Added illumination calculations: Ambient + Diffusive + Specular
- 

0.7: 
- Completed Rays creation

0.6:
- Added triangles tests and spheres representations. 

0.5:
- Initial release, simple red square. 
- Parse commands


