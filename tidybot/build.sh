#!/bin/bash

./lein compile
./lein uberjar

mv ./target/tidybot-1.0.1-snapshot-standalone.jar .
rm -rf ./target
