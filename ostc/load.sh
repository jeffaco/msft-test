########################################################
#!/bin/bash

source /opt/microsoft/scx/bin/tools/setup.sh

while true; do
    omicli ei root/scx SCX_Agent
    omicli ei root/scx SCX_Application_Server
    omicli ei root/scx SCX_DiskDrive
    omicli ei root/scx SCX_DiskDriveStatisticalInformation
    omicli ei root/scx SCX_EthernetPort
    omicli ei root/scx SCX_EthernetPortStatistics
    omicli ei root/scx SCX_FileSystem
    omicli ei root/scx SCX_FileSystemStatisticalInformation
    omicli ei root/scx SCX_EthernetPortStatistics
    omicli ei root/scx SCX_IPProtocolEndpoint
    omicli ei root/scx SCX_LANEndpoint
    omicli ei root/scx SCX_LogFile
    omicli ei root/scx SCX_Memory
    omicli ei root/scx SCX_MemoryStatisticalInformation
    omicli ei root/scx SCX_OperatingSystem
    omicli ei root/scx SCX_Processor
    omicli ei root/scx SCX_ProcessorStatisticalInformation
    omicli ei root/scx SCX_UnixProcess
    omicli ei root/scx SCX_UnixProcessStatisticalInformation
done
#########################################################
