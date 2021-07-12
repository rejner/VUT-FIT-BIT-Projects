#!/bin/bash
# Script will download library necessary for aplication runtime
# Author: Michal Rein

# JSON parse utils library
json_parser_url="https://repo1.maven.org/maven2/org/json/json/20190722/json-20190722.jar"

if [ ! -f "json-20190722.jar" ]; then
  echo "Downloading json-20190722.jar library from ${json_parser_url}."
  wget $json_parser_url
else
  echo "Library is already downloaded."
fi


if [ $? -ne 0 ]; then
  echo "Failed to download library from url ${json_parser_url}! Note that this library is essential!"
fi
