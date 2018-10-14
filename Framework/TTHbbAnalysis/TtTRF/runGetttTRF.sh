root -b -l <<EOF
.include ./
.L util/Get_ttTRF_2018_C.so
Get_ttTRF_2018("$1","$2","$3", false, false)
.q
EOF
