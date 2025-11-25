# MG file format documentation

Mgine files contain all the data for a game or program made in Mgine. Its purpose is to tell the engine which objects have been defined, which components can exist, the materials used, etc. On startup, this file is read and the functions used to normally manually create these assets get called automatically. Of course, this means that manual asset registration can happen alongside the automatic asset registration done by providing an Mgine file.

This file is generally generated alongside the executable by Mgine editors, but can also be created manually or with non-editor software. This filetype's filename extension is `.mg`. The limits found in the file documentation are mainly for preventing junk data from allocating large amounts of memory during runtime. These limits may be increased in the future.

## Format


| Offset          | Name                        | Type/Size  | Description                                                                          |
| --------------- | :-------------------------- | :--------- | ------------------------------------------------------------------------------------ |
| 0               | `MGINE`                     | 5 * char   | The bytes⠀`4D 47 49 4E 45` to identify the file.                                     |
| 5               | version number              | uint_32    | The version number of this MG file to prevent giving newer files to old versions.    |
| 9               | shader code count (`A`)     | uint_32    | The number of shader code files stored in this file. Limit of 1024.                  |
| 0               | shader code length (`B`)    | uint_32    | Length of shader code string. Limit of 65536 characters per shader code file.        |
| 4               | shader code                 | `B` * char | Shader source code.                                                                  |
| -               | shader count (`C`)          | uint_32    | The number of shader assets stored in this file. Limit of 1024.                      |
| 0               | shader vertex file          | uint_32    | Index into⠀`A`. Must be smaller than⠀`A`.                                            |
| 4               | shader fragment file        | uint_32    | Index into⠀`A`. Must be smaller than⠀`A`.                                            |
| 8               | shader define count         | uint_16    | The number of defines for this shader. Limit of 256.                                 |
| 0               | shader define type          | uint_8     | 0 = vertex shader, 1 = fragment shader                                               |
| 1               | shader define length (`X`)  | uint_16    | Length of shadeer define. Must be between 1 and 255.                                 |
| 3               | shader define               | `X` * char | Define added to shader. Example: "ENABLE" -> "... #define ENABLE\n ..."              |
| -               | texture count (`D`)         | uint_32    | The number of textures stored in this file. Limit of 32768.                          |
| 0               | texture filepath len (`E`)  | uint_16    | Length of filepath to the mga file containing the texture. Max 1024.                 |
| 2               | texture filepath            | `E` * char | Local filepath from executable to mga file containing the texture.                   |
| `E` + 2         | texture file index          | uint_32    | Index in the mga file for the texture.                                               |
| -               | material count              | uint_32    | The number of materials stored in this file. Limit of 4096.                          |
| 0               | material size (`Z`)         | uint_32    | Length of additional custom data found in this material. Limit of 65536 bytes        |
| 4               | material diffuse RGBA       | 4 * float  | Base color of the material.                                                          |
| 0x14            | material texture            | uint_32    | Index into⠀`D`. Value larger or equal to⠀`D` means no texture.                       |
| 0x18            | material transparency       | uint_8     | 0 = no transparency, other = contains transparency.                                  |
| 0x19            | material shader             | uint_32    | Index into⠀`C`. Value must be smaller than `C`.                                      |
| 0x1D            | material extra data         | `Z` bytes  | Additional custom data found in this material.                                       |
| `Z` + 0x1D      | material SVT count          | uint_16    | Number of Shader Variable Translators in the material. Limit of 1024.                |
| 0               | material SVT name len (`F`) | uint_16    | Length of shader variable name string. Limit of 256.                                 |
| 2               | material SVT name           | `F` * char | Name of shader variable                                                              |
| `F` + 2         | material SVT type           | uint_32    | GLenum value representing variable type.                                             |
| `F` + 6         | material SVT size           | uint_32    | GLenum value representing variable type.                                             |
| `F` + 0xA       | material SVT offset         | uint_32    | Offset in bytes from base of material struct, where the shader will get its data.    |
| -               | model count (`G`)           | uint_32    | The number of model filepaths stored in this file. Limit of 4096.                    |
| 0               | model filepath len (`H`)    | uint_16    | Length of filepath to the mga file containing the texture. Max 1024.                 |
| 2               | model filepath              | `H` * char | Local filepath from executable to mga file containing the model.                     |
| `H` + 2         | model file index            | uint_32    | Index in the mga file for the model.                                                 |
| -               | prefab count                | uint_32    | The number of prefabricated objects stored in this file. Limit of 4096.              |
| 0               | prefab name len (`I`)       | uint_32    | Length of prefabricated object name string. Limit of 1024 characters per name.       |
| 4               | prefab name                 | `I` * char | Name of the prefabricated object.                                                    |
| `I` + 4         | prefab flags                | uint_32    | Property flags for the prefabricated object.                                         |
| `I` + 8         | prefab component count      | uint_16    | The number of components owned by the prefabricated objects. Limit of 1024.          |
| 0               | prefab comp. name len (`J`) | uint_32    | Length of the name of the component in the prefab object. Limit of 1024 chars.       |
| 4               | prefab comp. name           | `J` * char | Name of the component in the prefabricated object.                                   |
| `J` + 4         | prefab comp. data len (`K`) | uint_32    | Size in bytes of data to be stored in prefabricated object component.                |
| `J` + 8         | prefab comp. data           | `K` bytes  | Data to be stored in prefab. object component. First 2*(pointer size) bytes ignored. |
| `J` + `K` + 8   | prefab child count (`L`)    | uint_16    | Number of child objects attached to prefab object. Limit of 2048.                    |
| `J` + `K` + 0xA | prefab children (recursive) | `L` * ?    | For every child, a name, flags, components, and more children just like we saw.      |
| (conditional)   | prefab object pref. id      | uint_32    | If the name length of an object in here is 0, read next int as index in prefab list. |
| -               | sound count                 | uint_32    | Number of sound file paths stored in this file. Max of 4096.                         |
| 0               | sound filepath length (`Y`) | uint_16    | Length of local filepath to MGA file containing sound.                               |
| 2               | sound filepath              | `Y` * char | Local filepath to MGA file containing sound.                                         |
| `Y` + 2         | sound file index            | uint_32    | Index in the MGA file of the sound file.                                             |
| -               | scene count                 | uint_16    | Number of scenes stored in the file. Limit of 1024.                                  |
| 0               | scene name len (`M`)        | uint_32    | Length of the name of the scene.                                                     |
| 4               | scene name                  | `M` * char | Name of the scene. Limit of 1024 chars.                                              |
| `M` + 4         | scene object count (`N`)    | uint_32    | Number of objects pre-loaded in this scene. Only top-level objects included.         |
| `M` + 8         | scene objects (recursive)   | `N` * ?    | See prefab section for data contained in objects.                                    |
| 0               | scene texture count (`O`)   | uint_32    | Number of textures pre-loaded for this scene.                                        |
| 4               | scene textures              | `O` * u32  | Textures to be pre-loaded when scene is loaded. Index into⠀`D`.                      |
| `O` * u32 + 4   | scene model count (`P`)     | uint_32    | Number of models pre-loaded for this scene.                                          |
| `O` * u32 + 8   | scene models                | `P` * u32  | Models to be pre-loaded when scene is loaded. Index into⠀`G`.                        |

## Notes

- The first scene in the list will be the one to be automatically loaded on game start.
