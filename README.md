# Ray_Tracer
Computer Graphics Ray Tracer (CPU) based on Edx CG-course.


Current list of features supported:
====================================
- Intersections with primitives: Triangles, Spheres, Cylinders, Boxes, Cones, Plains, ..., 
- Transformations: Scale, Rotation and translate. Also support transformations stacks (for transforms hierarchy) 
- Blinn-Phong illumination module - including normals interpolation and attenuation.
- Rendering of models comprised of multiple meshes with (partial) material properties. 
- Textures to primitives and models (even multiple textures types - such as diffuse-texture etc.). 
- Point lights, directional lights and area lights, with intesity.
- Recursive lighting calculations. Max recursion depth is currently limited to 5. 
- Multi-core rendering and Single-Core rendering. 
- Command line argument parse
- Saved image formats supported: .png, .jpeg, .jpg, .bmp, .tiff
- Ray tracing accelerations using bounding box and bounding volume (Kay and Kajiya 1986)
- BVH implementation to further accelerate rendering (Kay and Kajiya 1986)
- Statistics about rendering
- Soft shadows and area lights. 
- Full support for reflections and refractions, including mix of the 2 using Fresnel equations. 
- Normals interpolation (by default) for smoother appearance of less detailed models. 
- Glossy reflections and refractions
- Environments mapping and skyboxs (Cubical mapping). Supports both reflective and refractive mappings
- Normal mapping for meshes and primitives. Also include transormation to model space using TBN matrices. 




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
	  --stats                 Generate and print statistics about each scene 
                    		  rendered.
	  --hard-shadows          Indicate whether hard shadows should be simulated. By
	                          default, soft shadows are simulated. Use this to 
	                          improve performance. Note that soft shadows only 
	                          appear if area lights are used. If this flag is true 
	                          then hard shadows will be simulated, even if area 
	                          lights are present.
	  --flat-shading          Indicate whether flat shading should be used. If set,
	                          face normals will be used, otherwise (and by 
	                          default), normals are interpolated for a much 
	                          smoother image.
  	  --no-anti-aliasing      Indicate if anti-aliasing should be disabled, or 
          	                  enabled, as by default
	  --no-bump-maps          Indicate if normals bump maps should be used (as by 
          	                  default), or not. If set, no bump maps will be used 
                  	          even if object has one defined.
	   --fast                 Runs quick rendering. Use this to render complex 
	                          scenes fast - no anti aliasing, hard shadows, flat 
	                          shading, and no bump maps



	Other values are specified within each scene separately. 



Scene Keywords and Syntax:
============================

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
	skybox <index_i> - Sets skybox to encompass the entire scene. Technically, the scene is mapped to a previously defined
                environments (see "envMap" keyword under Primitives and Models). The index must be a valid index for a set of maps that
                was previously declared. If set, the skybox textures will be used when the ray fails to hit any object. 
		Note that in this case, there's no need to bind or unbind the envMaps used. 
 
	
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
	cone <center_v3> <minCap_f> <maxCap_f> - Defines a cone centered around the center point, 
		where maxCap and minCap define top and button caps of the cone. This means
		that its fairly easy to decide wheather its a single cone (where maxCap or minCap == 0) or double cone, or inifinite etc.). 
		It isassumed that maxCap > minCap.
	plain <texture-pattern_e> - Creates a plain passing through the origin(0,0,0) and aligned as XZ plain (that is, with normal (0,1,0).
		Ofcourse this can be tranformed like anyother object to achieve various types. Notice that this plain is infinite. 
		texture-pattern will define how a texture should be applied to this plain, if indeed this is a textured plain. Options are:
		"R" - Repeated pattern. (Only the fraction part of the intersection point will be taken)
		"MR" (Default) - Mirrored repeated pattern. Same as before, but will be mirrored when integer part it odd.
		"CE" - Clamp to edge. For values greater than 1 or smaller than 0, the value will be clamp to [0,1].
		Note that scale for this object can be used to strech the texture, as it won't have any other effect "physically"
	vertex <pos-v3> [<norm-v3>] [<texCoord-v2>] [<tangent-v3>] [<bitangent-v3>]  - Creates a single vertex, we can later be used to create a triangle.
		Apart from "pos" - all values are optional. You can specify normal and texcoors etc. Note that currenlty, the order is that same as written,
		and if tex coords are specified, then so is normal must be specified. 
	vertexSimple <pos-v3> - Creates a single vertex with only a position (This saves some calculation if possible)
	tri <vertex_1_index_i> <vertex_2_index_i> <vertex_3_index_i> - Creates a triangle using 3 vertices
		previously defined (using 'vertex' command. The numbering is the same as arrays. 
		The first 'vertex' is indexed 0 etc. 
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
	bindTexture <index_i> - Binds a previously defined texture with the given index. Any object (primitive or model)
		created from this point on, will be assigned with this texture (assuming it supports textures mapping
		and/or those mapping were given before). 
	unbindTexture - unbind the last bound texture. Any object created from this point on, will not have textures
		applied to it. 
	bindNormalMap <index_i> - Binds a previously defined texture with the given index to serve as a normal map. The index is of a
                texture that was defined before, that can also serve as a regular texture (but this doesn't really make sense). It is also
                possible to bind any image as a normal map, but it is recommended to bind texture and normal map that are related (see ./textures
                for example). Once bound, the returned normal will be taked from this map instead of the one that should have been 
		returned at the intersection point. 
		Note that this servers as a 'global normal map' that will be relevant to all primitives, and to all meshes the comprise an object. 
                It is also possible to define a 'per-mesh' normal map, by adding one using the "map_Bump <path>" within the .mtl file of that object.
                So meshes can have both, but it really makes since to include either per mesh normal or global, but not both.  
	unbindNormalMap - unbind the last bound normal map. Any object created from this point on, will not have this normal map to it.
	envMap <path_s> - Loades and saves a texture specified with the given path, relative to execution directory. The texture
		should be used for (cubical) environment mapping, so it is expected to see 6 such declerations, and it is assumed
		that they will be mapped in this order: PosX, NegX, PosY, NegY, PosZ, NegZ (see Advanced_7_Skybox_and_env_map.rt).
		Each 6 such declerations, are grouped, and indexed, starting at index 0.
	bindEnvMaps <index_i> ["refract"] [<refIndex-f>] - Binds a previously defined envMaps. Any model (and not object!) that will be created next, will be 
		mapped to this environment. By default, reflective mapping is used. It is possible to add 2 more arguments: "refract", to indicate that a refractive
		mapping should be used, followed by the refraction index. if refract is specified, than it must follows with the refraction index or unexpected results
		might show. 
		Important Notes:
			a) This mechanism could have been applied to primitives, but I chose not to, because all of them 
				can be textured already - which is much faster, and less memory consuming. So if this is needed - simply bind
				them to a texture. 
			b) Although models can be textured too, the textures are applied from triangles perspective, so they usually don't look 
				quite good, and this achieves much nicer results. 
			c) You can't bind a texture that was declared as an "envMap" and vice versa. You could declare both with the same path, and use
				them multiple times, but you must use multiple declerations. 
			d) The refraction index that is specified here is deliberatly different from the one that could be specified for the whole model
				(using keyword "refractionIndex" - see materials). 
			e) Currently (and probably always), I see no reason to add blur to these mappings. If one want to achieve this, he can simply ray trace
				the "normal" way. 
	unbindEnvMaps - unbind the last bound envMaps. Any model created from this point on, will not have environment mapping



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
happen. That is, if we want to achieve the usual matrices order: Scale the Rotate then Translate: T*R*S, we would write
Translate followed by rotate, followed by scale command. 

	translate <by_v3> - Applies translation matrix to the top of the transformation matrices. 
	rotate <axis_v3> <degrees_f> - Rotate by angles (in degrees) around the given axis.
	scale <ammount_v3> - Scale by the amount given for each axis (meaning - non-uniform scaling is possible). 
	pushTransform - pushes the current transform matrix on top of the transform stack.
	popTransform - removes the top transform matrix. 


Lights:
--------
Lights are also subjected to the current transform on top of the transformation stack. 
Colors can also be expressed in [0-225] RGB format. For value 1, it is assumed the the color component should be maximized. 

	directional <dir_v3> <color_v3> <intesity_f> - Creates a directional light with direction <dir> and with color <color> and
		with the given intesity. The intesity should be greater than 0.
	point <pos_v3> <color_v3> <intesity_f> - Creates a point light at the position <pos> with color <color>. and with the given intesity.
		The intesity should be greater than 0.
	area <pos_v3> <color_v3> <intesity_f> <radius_f> - Creates an area light (simulated as a 2D-disc), at the given position, with the
		given color and intensity (>0), and with the given radius. By defualt, the disc it perpendicular to the Y axis, and the user should
		use the various transforms in order to achieve any kind of area light. 
		Note: Currently, the number of samples, and the randomeness when generation the point is not visible to the user - this 
		might change on next stages. 
	attenuation <constant_f> <linear_f> <quadratic_f> - Sets the constant, linear and quadratic coefficients in the
		attenuation equation:
			Atten = 1 / (K_c + (K_l*d) + (K_q*d^2)) where:
			K_c - is the constant factor.
			K_l - is the linear factor.
			K_q - the quadratic factor.
			d - Distance between light source and surface (this means that this only affect point lights and area lights,
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
Colors can also be expressed in [0-225] RGB format. For value 1, it is assumed the the color component should be maximized. 
Also note you can simply write: "white" or "black" for vec3(1.0f, 1.0f, 1.0f) and vec3(0.0f, 0.0f, 0.0f) respectively.

	ambient <color_v3> - The ambient factor. Default vec3(0.2f, 0.2f, 0.2f)
	diffuse  <color_v3> - The diffusive factor. Default vec3(0.0f, 0.0f, 0.0f)
	specular  <color_v3> - The specular factor. Default vec3(0.2f, 0.2f, 0.2f)
	shininess  <color_i> - The shininess factor. Default 0f
	emission  <color_v3> - The emission factor. Default vec3(0.0f, 0.0f, 0.0f)
	reflection <color_v3> - The amount of light that will be reflected from this surface.
	reflectionBlur <float_f> - Should range [0-1]. The ammount of perturbation to apply to the stochastically generated rays when simulating glossy reflections  
	refraction <color_v3> - The amout of light that will be refracted from this surface. 
	reflectionIndex <color_f> - The index of refraction. (See https://en.wikipedia.org/wiki/Refractive_index for common indecies)
	refractionBlur <float_f> - Should range [0-1]. The ammount of perturbation to apply to the stochastically generated rays when simulating glossy refractions  
	clearProps - Removes all previous defined properties. (Sets everything to 0, except ambient which is set to 0.2;



Scene Example:
--------------
With this repository, I've included examples of many scenes and their rendered result. For simplicity sake,
let's look at a simple scene.
The result image can be found in "./Rendered_Scenes/ExampleSphere_result.png

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
	vertex +8 0 +8 1 1 # index is 0
	vertex +8 0 -8 0 1 # index is 1
	vertex -8 0 +8 1 0 # index is 2
	vertex -8 0 -8 0 0 # index is 3

	# create the floor
	pushTransform

		bindTexture 0 # we bind the wood2.jpeg texture. every object created from now on, will have
			      # this texture applied (globally)
		ambient 0.1 0.1 0.1 # the object material properties
		diffuse 0.1 0.1 0.1
		specular .8 0.8 0.8
		shininess 16
		tri 0 1 2 # actually create a triangle with texture. The newly created triangle will have
			     # the bound texture and the last specified materials properties (and transformations)
			     # the verticies to used are by index, which was assigen when those were created (vertexTex cmd)
		tri 1 3 2

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

	# End scene



Selected Scenes Examples:

1) Primitives with textures and soft shadows: 
![ExampleCone_soft_result](https://user-images.githubusercontent.com/6772259/58382641-5148d500-7fd5-11e9-80a2-ffc510628c6c.png)

2) Complex mesh on top of infinite textured plain:
![ExamplePlant_soft_result](https://user-images.githubusercontent.com/6772259/58382654-72a9c100-7fd5-11e9-95e6-aaa228f6c962.png)

3) Flat shading vs. smooth shading (same number of vertices in mesh):
![Advanced_3_smooth_vs_flat_shading_flat_result](https://user-images.githubusercontent.com/6772259/58382662-953bda00-7fd5-11e9-8b52-a9b53f556155.png)
![Advanced_3_smooth_vs_flat_shading_smooth_result](https://user-images.githubusercontent.com/6772259/58382664-979e3400-7fd5-11e9-952d-80f285e28009.png)

4) Glossy reflections and refractions:
![Advanced_5_Glossy_Refractions_30_per_blur_w_AA_result](https://user-images.githubusercontent.com/6772259/58382668-abe23100-7fd5-11e9-9e03-511413dcd8ba.png)

5) Refraction through glass medium:
![Advanced_6_Refraction_In_Medium_Glass_No_Blur](https://user-images.githubusercontent.com/6772259/58382671-bdc3d400-7fd5-11e9-97d6-506848468e37.png)

6) Environment mapping and sky boxes. (Both reflective and refractive mapping on the infamous Utah teapot):
![Advanced_7_Skybox_and_env_map_no_AA](https://user-images.githubusercontent.com/6772259/58382683-e9df5500-7fd5-11e9-96b6-66ae9e15b265.png)
![Advanced_8_Skybox_and_refr_env_map_w_skybox](https://user-images.githubusercontent.com/6772259/58382684-ed72dc00-7fd5-11e9-9f6a-df28188fcd32.png)


Version History:
=================

6.0:
-----
- Yes, I skipped version 5 since I've put so many features in this, so I actually combined two planed version. 
- Added normal mapping on primitives and meshes:
  On meshes: this is supported by adding a normal map (a texture) to the .mtl file that comes with the model. 
  The keywork used for this is "map_Bump <path>". Note that this is not my syntax, but of .mtl files. This means
  that I also ask of Assimp importer to calculate tangents and bi-tangents in order to later transform the extracted
  normal using the infamous TBN matrix.
  Important Note: traditional OpenGl approcach for TBN transforms means to transform the entire lights etc. in the scene
  and not the object, this is because it is more efficient since this could be done within the vertex shader, and not the
  fragment shader. However, I chose not to do this because I didn't need to. Obviously I don't have shaders at all, but I
  could achieve the same effect by creating a TBN matrix per vertex of a triangle (upon its creation), and when calculating
  the normal at a given point P, I used the uv coordinate to interpolate these TBN matrices that achieve the 'final' TBN 
  matrix. and this is done per vertex - so effeicitly, just list vertex shader. 
  On Primitives: This is supported by adding 2 new keywords: "bindNormalMap" and "unbindNormalMap". (See above for full
  explaination). Basically, you need to bind a previously defined texture, to serve as a normal map for the global texture
  on that object. Once normal map is bound, a normal from this map will be extracted, trasnformed using a calculated TBN 
  matrix, and used for the lighting calculations. I added this feature for all primitives currently supported.
- Important note: Since a mesh is also an object, it can have a global texture and one per mesh. It is also possible to
  bind a normal map on a model, that has already normal map on one (or all) of its meshes. When talking about textures this
  makes sense - to allow adding a global texture to per-mesh defined texture, but when dealing with normal, it doesn't really
  make sense because there's no good rule how to apply the 2 normals. However, I still left it for the simple reason to allow 
  the user to define a global normal map on the entire Object (all of its meshes), just like global texture. 
- Also added multiple scenes with all the normal mapping examples for primitives. There were some corner cases that had to
  be taked care of careflly.  
- For this version I also dove deep into more advanced c++ features. Specifically, I got tired of all the memory managemnt,
  so I switch to smart pointers (Unique and Shared primiarlly). I also used move mechanisms alot, and applied some 'const' 
  guidelines as suggested by Scott Meyers (In Effective C++). For this reason, almost everyfile once refactored and a lot of  
  code moved and removed (hopefully for the last time for now).
- Revised the code for triangles and removed all the stupid duplications that were caused due to my desire to stay compatible
  with Edx course. From now on - this is no longer supported, and there only 2 ways to create a triangle now. (And still, it 
  is recommended to use meshes instead). This means that the following keywords are removed: "vertexNormal", "vertexTex", "triNormal", "triTex" and "maxVertNorms".
  They were replaced with a "vertex" and "vertexSimple" keywords. (see above). I also updated all the scenes not to include these
  keywords anymore.
- Bug fixes:
  a) Fixed a bug where meshes textures were not removed between session
  b) Fixed a bug where meshes textures cannot be shared
  c) Fixed a bug where meshes where not cleaned propely. 
  d) fixed a bug for memory access violation when creating verticies. 
  e) Fixed a bug where Extents of BVH weren't properly initialized, causing segmentation faluts on some compilers.
  f) Fixed a bug where environment mapping were return correctly. 
  g) some minor fixes in the Makefile. 
- Added to flags for ray_tracer arguments: --no-bump-maps, --fast (see above)
- Fixed bug when counting statistics. Also changed mechanism not to count anything if flag is off, to save expansive atomic locks. 
- ran valgrind again on all scenes to make sure no memory violations of any kind (except the known FreeImage() issue - see below)
- Added known issue of valgrind. 



4.6:
- Added refractive environment mapping. By default, reflective mapping will be used.
- Added scenes to show this (see advanced shading 8). 
- More code refactoring - moved some code to General.h

4.5:
-----
- Added environment mapping for models (i.e. meshes). This does not apply to primitives (sphere etc.), because you can simply texture them. I've
implemented cubical mapping, so the textures should comply to the arrangment of cube-map, like defined here: https://en.wikipedia.org/wiki/Cube_mapping
- Did some folders reordering. Creating a directory called "environments", which contains sub-directories for each environment mapping I tests. See keywords
description as to how these should be ordered.
- Added skybox to scene. This works similar to environment mapping to models, except that the ray is not reflected. If the ray misses every object, and given that
the scene is skyboxed - (with keyword "skybox" used), then the colors will be of the appropriate texture. (See Advanced_7_Skybox_and_env_map.rt as an example)
- Added keywords: "skybox", "envMap", "bindEnvMaps", "unbindEnvMaps" - see description above.
- Fixed a bug in BVH module where properties of last intersected model were chosen, even if this object was more far than a previously intersected mesh. 
- BVH oct-tree depth is now proportional to the number of meshes in the scene, as for not to consume too much memory. 
- moved getTextureColor from object to General.h, and also added cubeMapping to there, as it is more appropriate there. 
- Removed unused dead-code. 


4.1:
- Added a scene dipiction rays refraction when passsing through a refractive medium (with changed parameters) (See advanced scene 6). 
- Fixed a bug where objects that are purely reflective and refractive would not render properly. 
- Fixed a bug where object that were only diffusive or specular (but not both) were not rendered properly (Solved)
- Fix to ray direction calculation when refractive a ray. This means that sphere refract differently than before, but this gives more realistic results.

4.0:
-----
- Added glossy reflections and refractions. These are supported with a new keyword (reflectionBlur <float(0,1)>, and refractionBlur <float(0,1)> respectively.
To get nice result, usually somewhere between 0.1 to 0.3 is nice. 0 means no blur at all. 
- Added multiple scenes with these new options, each with values a little different.
- changed folder structure a little bit.
- Fixed a bug when refracting a ray wrongfully. Still another bug exists and will be fixed on later version.
- Changed random sampling to C++11 standard using mersenee twister engine. 
- removed some dead code from RayTracer.cpp
- Changed the mechanism for additional render options (anti-aliasing, flat-shading and hard-shadows - for now)
- Changed keyword "refIndex" -> "reflectionIndex" for better clarity. Also updated scene where this was relevant 
- Some problem with AA is still present. Will be fixed next verion.
- Created a folder with problematic scenes which produce bugs. Also changed folders hierarchy a little bit.

3.8:
- Added lights intesity. The intesity is a float, that should be greater than 0. Use this to create larger effect of a light. (Point light and area
lights can look nicer). Consider using attenuation for more realistic results.
- Added two key words: "vertexNormTex" for creating a vertex with normals and textures, and triNormTex for creating a triangle with these properties 
on each vertex.
- Fixed a bug that was added due to normals interpolations. Scenes didn't have the normals definition, so old scene that uses triangles as planes
didn't have proper lighting. This was fixed, and the scenes were fixed and re-uploaded. In general, the triangle is legacy to Edx course and should
be avoided. Use plains intead is much better and easier.
- Some scene with "Soft" shadows were really with hard shadows. So they were removed and re-added, this time with proper shadows. 

3.7:
- Implemented the fast ray-triangle intersection as suggested by Möller–Trumbore (1997). This speeds rendering for complex meshes by 3 times!
- minor bug fixes.

3.6:
- More on shading: Added normals interpolation when rendering meshes (and triangles). By default, normals are interpolated, unless stated otherwise (--flat-shading flag)
- Added a scene with the difference between the modes. The model is exactly the same. this means that small models can be rendered to look quite nice withouth the need of many vertices and faces.
- Removed dead code from triangle intersections

3.5:
-----
- Full support in reflections and refractions, and the mixture of the two, using Frensel equations. To support this, the basic rendering function was changed, so that recursive ray tracing only happnes from reflective (and/or) refractive surfaces. The specular term that was used before as a reflection, surves only for the bling_phong lighing calculations, and not the recursive calculations. 
the
- This means that all previous scenes that features reflections had to be adjacted to get the previous results. If a surface is not reflective or refractive, then only bling_phong lighing will be taken into account. Note that this means the the recursion depth is only relevant for reflective / refractive surfaces. So no point in high depth if those surfaces aren't present. 
- added "clearProps" key word to remove all material properties so they won't be passing threw other object if not wanted.
- Also added the key words "black" and "white" to define the (0,0,0) and (1,1,1) colors respectively, so it will be a little easier to change the scene if needed. 
- Fixed a bug when rendering multiple scenes were not all values were cleared
- improved error handling a bit
- Changed Plane.h/.cpp to Plain.h/.cpp (silly me)  (also changed the keyword spelling)
- Readded the fixed scenes featuring the new reflections and refraction.
- Added an example scene of reflection and refraction including frensel effect. See "ExampleAdvancedShading_1.rt". The scene looks very simple, but it actually shows nice effect. More complex scenes will be on later version when more shading techniques will be implemented. 
- The changes above mean that the scenes given from Edx are no longer compatible, and we're finally detaching from this project. I could change them - but there's really no need for that. 


3.3:
- Fixed dead pixels when rendering scenes with plane at large distance.
- Recursion max depth is 5, and it will be stopped if contribution to light become negilgent. 

3.2: 
- Added soft shadows simulation. This can be achieved using area lights which were added also.
- soft shadows won't be displayed if area lights are not present within the scene. 
- Added the ability to simulate hard-shadows regardless if area lights are present in the scene: use the "hard-shadows" flag to achieve this. This will speed performance, but will reduce quality obviously.
- Fixed bugs when transform point and directional lights. 
- Added simple example with soft shadows. Note that 'Plane' object currently has a bug which causes black pixels on the bottom left of the screen - which will be fixed on next version.
- Also added previous scenes with soft shadows for comparison. From now on, scenes will be rendered with soft shadows by default. 
- Small fix to "indoor-plant" object using blender, to get rid of stupid mistake there. also created a smaller model to accelerate scene rendering (full object takes almost 35 minutes to render i7 8700)

3.1:
- Fixed normal were not transformed for models as they should have. 
- Changed object constructor so now it required transformations and properties upon creation.
- Fixed an issue with valgrind complaining about uninitialized value that may cause problems when switching between rendering threads. 


3.0:
-----
- Full Support in Bounding Volume Hierarchies. The implementation is based on Kay and Kajiya (1986) and includes slabs, extents, and octrees for scene subsurface divisions.
- Also added bounding boxes (AABB) for primitive objects which are finite.
- To support the above - the code was refactored: Model is no longer an object, but rather a static factory class which creates meshes, which are derived from object. The model class now acts as a way to define a set of meshes that will all share the same properties. (Note that it is still possible to define per-mesh properties from within the .mtl files). 
- Meshes are inserted into an octree, so ray-object intersection tests are reduced dramatically. 
- Also - primitives are first intersected with their bounding box, and this value is used to determine the nearest object so far before checking for complex objects (i.e. meshes). Obviously, this increased render time for the example scene, because they feature a small number of primitives - but when tested against large number of objects - the results were improved dramatically. (up to 100 times faster for some scenes). 
- Meshes and primitives are now intersected separately. First primitives, and then meshes. the same applies for shadow rays. 
- Fixed a bug were object translated upon creation affected objects created after with same translation. 
- Fixed Cone example scene after fixing the bug above.
- Fixed texture apply on meshes vs. object (="general" textures")
- Changed object properties to a specific struct. This will be further changed - an object must be built using properties and transformations.
- Small fixed in Cone and Cylinder radius and textures calculations. 
- Added AABB.h and .cpp and removed BoundingVolume.h and .cpp. Added BVH.h and .cpp.
- Removed many comments and irrelevant code. 


2.5:
- Major update - code was refactor to support bounding volume optimizations. Tracing no longer 
  happens from rays to objects, but rather to bounding volumes. 
- In order to support all features as before, code was modeled so that meshes are bounded by extents, and primitive objects are left unbounded. 
- Next stages will include adding bounding volumes for primitives, and also gathering multiple objects in the same extent.
- This change made it possible to render complex scenes using models that contain large number of polygons. It was a matter of hours before, and now some
  scenes render time is cut down to few minutes, and even seconds. Still - more optimizations are needed. 
- Added new scene "ExamplePlant" depicting this feature. Note that the 3d module is bugged , more accurate 3d models are to come. 

2.2:
- Added bounding box as a (simple) acceleration structure.
- Added bounding volume as suggested by Kay and Kajiya (1986) 
- Both optimizations currently only apply to complex objects (i.e. - non primitives) 
- Added stats when rendering each scene.
- Change ray-intersection routine 
- Fixed bugs regarding meshes and model loading. 

2.1:
- Added primitive - (infinite plane). Also supports all transformations, to achieve general plane. 
- Added 3 textures patterns to infinite plane - Repeated, Mirrored-Repeated and Clamp-To-Edge. 
- Added example scene with plane. 
- Small refactor to scene parser when creating new objects. 

2.0:
-----
- Fixed bugs with intersection at caps for cone and cylinder that were calculated wrong.
- Fixed bug with texture for those primitives, also applied at cone.
- Fixed a bug with edge values of texture (0 or 1) tend not to show properly. 

1.9:
- Added (Double, possibily infinite) cone with texture support. The cone can be transformed as usual, and caps can be set to determine its size etc.
- Added Example scene for cone.
- Fixed a bug where non valid ray were produced by normal reflections.
- more minor bugs fixes.
- Fixed a bug where objects cast shadows on themselves (even though ray was moved by some distance)  

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
-----
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


Known Issues:
==============
- Under old version of gcc (7 or less), compilation sometime causes internal compiler error due to usage of BOOST lib, when using
parrallel compilation. This problem seemed to be fixed on later version of gcc (8 or later).-
- If you run valgrind on any of the scenes supplied, or one you created, you'll see this line: "still reachable: 1,496 bytes in 31 blocks".
This is a known issue, which is due to the usage of FreeImage library. 



