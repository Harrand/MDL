MDL Testing Program -- Simple C++ Program used to test the functionality of MDL wirrten by Harry "Harrand" Hollands 2017.
Dependencies:
- mdl.dll
- libmdl.a (Only if not using g++; using g++ should detect mdl.dll without this implib)
Features:
- Command Line Arguments:
	- test.exe <filename>
		Where <filename> is the name of the MDL file which you wish to view/edit
- The following commands:
	- stop
		Stops the Program
	- exit
		Stops the Program (stop alias)
	- addTag [tagname] [data]
		Adds a new tag to the datafile with the name [tagname] with value [data]
	- addSequence [sequencename] [data1] [data2] [dataN]
		Adds a new sequence to the datafile with the name [sequencename] with the values [data1], [data2], [dataN]
	- deleteTag <tagname>
		Deletes the tag named <tagname>, removing it permanently from the datafile
	- deleteSequence <sequencename>
		Deletes the sequence named <sequencename> and all its members, removing them permanently from the datafile
	- getTag <tagname>
		Prints out the value of the tag with the name <tagname>. Will print out 0 if the tag doesn't exist
	- getSequence <sequencename>
		Prints out the members of the sequence with the name <sequencename>. Will print out nothing (Or crash) if the sequence doesn't exist
	- tags
		Prints out all tags in the datafile aswell as their respective values
	- sequences
		Prints out all sequences in the datafile aswell as their respective list of values
	- update
		Updates the MDL storage variables safely. This must be used if you've added/removed a tag/sequence and want to interact with it in any way.