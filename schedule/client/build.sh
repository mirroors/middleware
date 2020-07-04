#!/bin/bash 

rm -rf ./*.o
rm -rf ./*.a 

source /opt/poky/2.4.2/environment-setup-aarch64-poky-linux

$CC -c SchMClient.c -o SchMClient.o 

$CC -c SchCommon.c -o SchCommon.o 

$AR cqs libSchMClient.a SchMClient.o SchCommon.o

$CC CtApPrediction.c -o CtApPrediction -L./ -lSchMClient -lpthread -lrt 

$CC CtApTransformer.c -o CtApTransformer -L./ -lSchMClient -lpthread -lrt 

$CC CpApMatchPosition.c -o CpApMatchPosition -L./ -lSchMClient -lpthread -lrt 

$CC CtApAppFicmProxy.c -o CtApAppFicmProxy -L./ -lSchMClient -lpthread -lrt 

$CC CtApMatch.c -o CtApMatch -L./ -lSchMClient -lpthread -lrt 

$CC CtApUWBProcess.c -o CtApUWBProcess -L./ -lSchMClient -lpthread -lrt 
