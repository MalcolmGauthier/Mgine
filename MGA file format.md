# MGA file format documentation

Mgine Asset files contain a collection of asset files stored as-is. This is basically just a folder as a file, but without compression. This is done to reduce the number of i/o calls and provide some basic (albeit not useful) form of obfuscation for the assets. However, the reduced file count also makes it quicker to move/copy the game's files. Games can have as many of these as needed, although usually there are closely tied to the scene count.

This file is generally generated alongside the executable by Mgine editors, but can also be created manually or with non-editor software. This filetype's filename extension is `.mga`. The limits found in the file documentation are mainly for preventing junk data from allocating large amounts of memory during runtime. These limits may be increased in the future.

## Format

| Offset | Name                | Type/Size     | Description                                                                       |
| -------- | --------------------- | :-------------- | ----------------------------------------------------------------------------------- |
| 0x0    | `MGINEA`            | 6 * char      | The bytes⠀`4D 47 49 4E 45 41` to identify the file.                              |
| 0x6    | Asset count (`A`)   | uint_32       | The number of asset files stored in this file. Limit of 65536.                   |
| 0xA    | Asset indices (`B`) | `A` * uint_64 | The offset from the very start of this file on where the files are located.       |
| `B`    | Asset data          | ?             | The asset data. This is just a regular file, whatever is supported by the engine. |

## Notes

- The asset indices can also be used as a method of calculating the size of the files, as the files are packed with no extra room.
- All asset indices should be given in ascending order.
