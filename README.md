MDL -- Minimalist Data Language
Written by Harry "Harrand" Hollands 2017.
Designed for use with C++11 to manipulate and read data in a simple and efficient manner.

Features (2):
- Tags
- Sequences

Tag Format:
tagname: tagdata
Any string literals can be used for 'tagname' and 'tagdata' so long as they do not include a improper substring of the following:
%[
]%
:
\n

Sequence Format:
sequencename: %[
- sequencedata_1
- sequencedata_2
	...
- sequencedata_n
]%

This pseudolanguage is almost identical to YAML, although even simpler (For example, MDL lacks the indentation structure that YAML possesses).
Due to this, MDL is recommended only for simple data-handling. Although if you wish to describe an object with multiple attributes in MDL, I recommend the following strategy (Shown via an example below):

Problem:
human1 is a tag which requires multiple pieces of data to represent. Simply append ".attribute_name" to the tagname to allow multiple tags to describe the same object. Support for this does not inherently exist, although it is easy to implement if required:

human1.name: Bob
human1.height: 180cm
human1.age: 22y

In addition, a sequence can be used for the tag human1 to represent all the appended attribute names you wish to use:

human1: %[
- name
- height
- age
]%

Although this functionality is optional.
