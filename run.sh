#!/bin/bash

INPUT=input.mp4
OUTPUT=encoded.bin
CORRUPTED=corrupted.bin
DECODED=decoded.mp4

if [ ! -f ${INPUT} ]; then
    curl -o input.mp4 https://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4
fi

gcc hamming.c -O3 -o hamming

echo encoding...
./hamming encode ${INPUT} ${OUTPUT}

echo corrupting...
./hamming corrupt ${OUTPUT} ${CORRUPTED}

echo decoding...
./hamming decode ${CORRUPTED} ${DECODED}

md5sum ${INPUT}
md5sum ${DECODED}