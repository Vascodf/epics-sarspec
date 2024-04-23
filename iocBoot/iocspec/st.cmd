#!../../bin/linux-x86_64/spec

#- You may have to change spec to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/spec.dbd"
spec_registerRecordDeviceDriver pdbbase

specAsynPortDriverConfigure("testAPD", 1000)

## Load record instances
#dbLoadRecords("db/xxx.db","user=bernardo")
dbLoadRecords("db/specAsynPortDriver.db","P=ISTTOK:,R=sarspec:,PORT=testAPD,ADDR=0,TIMEOUT=1,NPOINTS=3648")
dbLoadRecords("${ASYN}/db/asynRecord.db","P=ISTTOK:,R=asyn1,PORT=testAPD,ADDR=0,OMAX=80,IMAX=80")

#asynSetTraceMask("testAPD",0,0xff)
asynSetTraceIOMask("testAPD",0,0x2)

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=bernardo"
