
#!/bin/bash

g++ -std=c++17 main.cpp JsonNode.cpp JsonParser.cpp JsonWriter.cpp KvDatabase.cpp -o jsonkv
./jsonkv