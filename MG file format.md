# MG file format documentation

Mgine files contain all the data for a game or program made in Mgine. Its purpose is to tell the engine which objects have been defined, which components can exist, the materials used, etc. On startup, this file is read and the functions used to normally manually create these assets get called automatically. Of course, this means that manual asset registration can happen alongside the automatic asset registration done by providing an Mgine file.

This file is generally generated alongside the executable by Mgine editors, but can also be created manually or with non-editor software. This filetype's filename extension is `.mg`. The limits found in the file documentation are mainly for preventing junk data from allocating large amounts of memory during runtime. These limits may be increased in the future.

## Format


| Offset        | Name                         | Type/Size  | Description                                                                          |
| --------------- | ------------------------------ | :----------- | -------------------------------------------------------------------------------------- |
| 0             | `MGINE`                      | 5 * char   | The bytes⠀`4D 47 49 4E 45` to identify the file.                                    |
| 5             | version number               | uint_32    | The version number of this MG file to prevent giving newer files to old versions.    |
| 9             | shader code count (`A`)      | uint_32    | The number of shader code files stored in this file. Limit of 1024.                  |
| i(`A`) (`a`)  | shader code type             | uint_8     | 0 = vertex shader, 1 = fragment shader.                                              |
| `a` + 1       | shader code length (`B`)     | uint_32    | Length of shader code string. Limit of 65536 characters per shader code file.        |
| `a` + 5       | shader code                  | `B` * char | Shader source code.                                                                  |
| -             | shader count (`C`)           | uint_32    | The number of shader assets stored in this file. Limit of 1024.                      |
| i(`C`) (`b`)  | shader vertex file           | uint_32    | Index into⠀`A`. Must be smaller than⠀`A`.                                          |
| `b` + 4       | shader fragment file         | uint_32    | Index into⠀`A`. Must be smaller than⠀`A`.                                          |
| -             | texture count (`D`)          | uint_32    | The number of textures stored in this file. Limit of 32768.                          |
| i(`D`) (`c`)  | texture filepath len (`E`)   | uint_16    | Length of texture filepath string. Limit of 1024 characters per filepath.            |
| `c` + 2       | texture filepath             | `E` * char | Local filepath from executable to texture file.                                      |
| -             | material count (`F`)         | uint_32    | The number of materials stored in this file. Limit of 4096.                          |
| i(`F`) (`d`)  | material diffuse RGBA        | 4 * float  | Base color of the material.                                                          |
| `d` + 0x10    | material texture             | uint_32    | Index into⠀`D`. Value larger or equal to⠀`D` means no texture.                     |
| `d` + 0x14    | material transparency        | uint_8     | 0 = no transparency, other = contains transparency.                                  |
| `d` + 0x15    | material shader              | uint_32    | Index into⠀`C`. Value must be smaller than `C`.                                     |
| `d` + 0x19    | mateiral SVT count (`G`)     | uint_16    | Number of Shader Variable Translators in the mateiral. Limit of 1024.                |
| i(`G`) (`e`)  | material SVT name len (`H`)  | uint_16    | Length of shader variable name string. Limit of 256.                                 |
| `e` + 2       | material SVT name            | `H` * char | Name of shader variable.                                                             |
| `e` + `H`     | material SVT type            | uint_32    | GLenum value representing variable type.                                             |
| `e` + `H` + 4 | material SVT offset          | uint_32    | Offset in bytes from base of material struct, where the shader will get its data.    |
| -             | model count (`I`)            | uint_32    | The number of model filepaths stored in this file. Limit of 4096.                    |
| i(`I`) (`f`)  | model filepath len (`J`)     | uint_16    | Length of model filepath string. Limit of 1024 characters per filepath.              |
| `f` + 2       | model filepath               | `J` * char | Local filepath from executable to model file.                                        |
| -             | prefab count (`K`)           | uint_32    | The number of prefabricated objects stored in this file.                             |
| i(`K`) (`g`)  | prefab name len (`L`)        | uint_32    | Length of prefabricated object name string. Limit of 1024 characters per name.       |
| `g` + 4       | prefab name                  | `L` * char | Name of the prefabricated object.                                                    |
| `g` + `L` + 4 | prefab flags                 | uint_32    | Property flags for the prefabricated object.                                         |
| `g` + `L` + 8 | prefab component count (`M`) | uint_16    | The number of components owned by the prefabricated objects. Limit of 1024.          |
| i(`M`) (`h`)  | prefab comp. name len (`N`)  | uint_32    | Length of the name of the component in the prefab object. Limit of 1024 chars.       |
| `h` + 4       | prefab comp. name            | `N` * char | Name of the component in the prefabricated object.                                   |
| `h` + `N` + 4 | prefab comp. data len (`O`)  | uint_32    | Size in bytes of data to be stored in prefabricated object component.                |
| ... + 8       | prefab comp. data            | `O` bytes  | Data to be stored in prefab. object component. First 2*(pointer size) bytes ignored. |
| ... + 8 +`O`  | prefab child count (`P`)     | uint_16    | Number of child objects attached to prefab object. Limit of 2048.                    |
| ?             | prefab children (recursive)  | `P` * ?    | For every child, a name, flags, components, and more children just like we saw.      |
| -             | scene count                  | uint_16    | Number of scenes stored in the file.                                                 |
|               |                              |            |                                                                                      |
|               |                              |            |                                                                                      |
|               |                              |            |                                                                                      |
|               |                              |            |                                                                                      |
|               |                              |            |                                                                                      |
