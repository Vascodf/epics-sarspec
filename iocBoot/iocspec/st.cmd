#!../../bin/linux-aarch64/spec

#- You may have to change spec to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/spec.dbd"
spec_registerRecordDeviceDriver pdbbase

specAsynPortDriverConfigure("sarspec")

## Load record instances
#dbLoadRecords("db/xxx.db","user=oper")
dbLoadRecords("db/specAsynPortDriver.db","P=ISTTOK:,R=sarspec:,PORT=sarspec,ADDR=0,TIMEOUT=1")
dbLoadRecords("${ASYN}/db/asynRecord.db","P=ISTTOK:,R=asyn1,PORT=sarspec,ADDR=0,OMAX=80,IMAX=80")

#asynSetTraceMask("sarspec",0,0xff)
asynSetTraceIOMask("sarspec",0,0x2)

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=oper"
