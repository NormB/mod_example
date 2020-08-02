# mod_example

mod_example was presented at ClueCon 2020 to demonstrate techniques related to developing a FreeSWITCH event handling module.

This example code is NOT suitable for any use other than as a learning tool.  Specifically, it is NOT suitable for production use.

The code has been trimmed down so that only the elements needed to show various capabilities are present.  There is no validation that functions return legitimate values, nor is there any validation done against a function that may return an error.

## Some of techniques are used in the example

```console
Module load and shutdown functions
Structure containing global variables
Structure suitable for inclusion into a hash table
Parsing entries from a configuration file
Setting up a Command Line Interface (CLI)
Binding to events such as HEARTBEAT
Unbinding events prior to shutdown
Operations surrounding hash table operations
```

Hope this helps you in some way.