#!/bin/bash
mkdir run-offline;
cd run-offline;
ln -s /afs/cern.ch/work/s/sosen/ChongbinTop/common-framework/postProd/* .;
echo "/eos/atlas/atlascerngroupdisk/phys-top/4tops/ljetsOS/cutflow/v1/user.sberlend.410080.MadGraphPythia8EvtGen.DAOD_TOPQ1.e4111_e5984_s3126_r9781_r9778_p3390.AZ_mc16c_test10_output.root/user.sberlend.14095570._000001.output.root" > list-input.txt;
ttH-offline configuration_bucketTopson.txt list-input.txt;
