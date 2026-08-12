Memgraph files:
sysdiagnose generates memgraph files by running leaks on some set of processes.
Since these files are self contained with symbol information, sysdiagnose no 
longer runs vmmap, heap and leaks on them. The users of sysdiagnose are 
expected to run these specific tasks, if necessary, using the generated 
memgraph files.

.csstoredump files:
sysdiagnose generates the output of lregister/lsaw in a binary form. To convert
these .csstoredump files to text files, use the following command: 

	lsaw dump --file "PATH TO DUMP FILE" > lsaw.txt

These files can also be opened in CSStore Viewer.
