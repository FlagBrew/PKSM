# servepkx (java)

Java Client to send .pk6/.pk7 files to a server running on [PKSM](https://github.com/BernardoGiordano/PKSM)

### Prerequisites

JRE 1.6+ installed.

Script works when PKSM's server is actually running.

### Usage (script)

```
java -jar servepkx.jar [console ip address] [path1] [path2] ...
```

### Usage (batch)

Edit your `servepkx.bat` file with a notepad to replace your console IP address.

Then, drag&drop files on `servepkx.bat` while PKSM's server is running.

### Usage (GUI)


Start the server on PKSM and enter the 3DS' IP address and the port number (optional) in servepkx. The default port nr. is 9000.

Press "Confirm host and port", then press "Select .pk6/.pk7 files" and select your files.

When you're done, press "Send .pk6/.pk7 files" to send the selected files to the PKSM server.

## Credits

@slownic for the whole client
