# P2P

Distributed file backup system, which allows for multiple backups of single file on rmeote hosts. Managed by a main server used only for peer discovery.


### Launching

1. Start MainServer on a host without arguments ```P2PMainServer```
2. Start P2P node on any number of hosts providing the following arguments
 - File manager port - any port that the file manager can bind and listen on
 - P2P node port - any port that the p2p node can bind and listen only
 - Main server address - the ip of the host that P2PMainServer was started

### Example:

```
$ ./P2P.exe 31 32 10.0.201.3
Connected to 10.0.201.3 5005
Listening and file manager ports sent !!
5 bytes
Server port:32
File mgr port: 31
P2P server listening at port 32
Enter file path to import:
D:/dev/p2p/test_files/bridge1.jpg
Enter file path to export:
D:/dev/p2p/test_files/output/bridge1.jpg
Imporintg file D:/dev/p2p/test_files/bridge1.jpg
Sending chunk to 10.0.201.3:3001
Imorted chunk with ID 58 to host 10.0.201.3:3001
Sending chunk to 10.0.201.3:3001
Imorted chunk with ID 59 to host 10.0.201.3:3001
Sending chunk to 10.0.201.3:3001
Imorted chunk with ID 60 to host 10.0.201.3:3001
Sending chunk to 10.0.201.3:2001
Imorted chunk with ID 46 to host 10.0.201.3:2001
Sending chunk to 10.0.201.3:2001
Imorted chunk with ID 47 to host 10.0.201.3:2001
Sending chunk to 10.0.201.3:2001
Imorted chunk with ID 48 to host 10.0.201.3:2001
Sending chunk to 10.0.200.92:8888
Imorted chunk with ID 1 to host 10.0.200.92:8888
Sending chunk to 10.0.200.92:8888
Imorted chunk with ID 2 to host 10.0.200.92:8888
Sending chunk to 10.0.200.92:8888
Imorted chunk with ID 3 to host 10.0.200.92:8888
Imported!
Checking for file D:/dev/p2p/test_files/bridge1.jpg in storage
Checking chunk with ID: 58 with host 10.0.201.3:3001
Checking chunk with ID: 59 with host 10.0.201.3:3001
Checking chunk with ID: 60 with host 10.0.201.3:3001
Checking chunk with ID: 46 with host 10.0.201.3:2001
Checking chunk with ID: 47 with host 10.0.201.3:2001
Checking chunk with ID: 48 with host 10.0.201.3:2001
Checking chunk with ID: 1 with host 10.0.200.92:8888
Checking chunk with ID: 2 with host 10.0.200.92:8888
Checking chunk with ID: 3 with host 10.0.200.92:8888
Available!
Exporting file D:/dev/p2p/test_files/bridge1.jpg to D:/dev/p2p/test_files/output/bridge1.jpg
Getting chunk ID: 58 from host 10.0.201.3:3001 ...  Success!
Getting chunk ID: 46 from host 10.0.201.3:2001 ...  Success!
Getting chunk ID: 1 from host 10.0.200.92:8888 ...  Success!
Exported!
```
