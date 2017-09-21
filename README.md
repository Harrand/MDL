# MDL
MDL stands for Minimalist Data Language. This framework is used to interpret MDL data files. The interpreter is designed for use with C++17 to manipulate and read data in a simple and efficient manner.

## Getting Started

DLL, object file and lib file are in the /cpl/ directory. Use those to dynamically link against MDL. Alternatively, the source is located in /src/ so you may simply add it to your project if you wish.

### Prerequisites

C++17 only. The binaries provided are compiled with MinGW-W64 x86_64 g++ v7.1.0-posix-seh-rt_v5-rev2, so if you're not using a toolchain similar to this, you may need to consider compiling it yourself. A batch file (qcpl.bat) is provided to do this for you. If you're on a unix system, don't worry compilation is trivial (Just interpret the batch file yourself).

## Deployment

If you wish to use this framework, go ahead.

## Contributing

Although I do not currently desire any form of third-party developmental contribution whatsoever, I am open to the contribution of entertainment whilst I feebly continue my pointless existence. Now that I think about it, if you're really reading all of this, chances are I'm not the person with the most pointless existence here. Also I love pictures of puppies, especially golden retrievers. It took much willpower on my behalf to not use a picture of a super cute golden retriever pup as the icon to this engine's testing executable. I'm professional.

## Versioning

I use [Git](http://github.com/) for versioning. Bet you didn't see that one coming. No versions are available on this repository's releases page, all binaries for all builds can be found in the corresponding commits /cpl/ directory.

## Authors

* **Harry Hollands** - *Sole Developer* - [Harrand](https://github.com/Harrand)

See also the list of [contributors](https://github.com/Harrand/MDL/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## Acknowledgments

* XML for being so exceedingly verbose that it forced me to write my own data format. And also to YAML for not being my cup of tea (Probably due to Minecraft redpilling me on it).

## Features
Tags and Sequences.

Tag Format:

`tagname: tagdata`

Any string literals can be used for 'tagname' and 'tagdata' so long as they do not include a improper substring of the following:

`%[`

`]%`

`:`

`\n`

This pseudolanguage is almost identical to YAML, although even simpler (For example, MDL lacks the indentation structure that YAML possesses). Due to this, MDL is recommended only for simple data-handling. Although if you wish to describe an object with multiple attributes in MDL, I recommend the following strategy (Shown via an example below):

Problem:
human1 is a tag which requires multiple pieces of data to represent. Simply append ".attribute_name" to the tagname to allow multiple tags to describe the same object. Support for this does not inherently exist, although it is easy to implement if required:

`human1.name: Bob`

`human1.height: 180cm`

`human1.age: 22y`

In addition, a sequence can be used for the tag human1 to represent all the appended attribute names you wish to use.