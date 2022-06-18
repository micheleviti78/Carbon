#!/bin/sh

if [ $# != 2 ]
then
    echo "Usage: $0 <firmware.elf> <EXCEPTION.LOG>"
    exit 1
fi

awk '
  /CPU[1-2]+ [0-9A-Za-z_]+ fault PC [0-9A-Fa-fx]+/ {
    print "### Exception", $2, "occurred at", $5
    system("arm-none-eabi-addr2line -sCfe '$1' " $5);
  }
  /## ([0-9A-Fa-fx]+) ([0-9A-Fa-fx]+)/ {
    print "------------------------------------------------------------"
    print "### Stack frame ", $2, $3
    system("arm-none-eabi-addr2line -sCfe '$1' " $3);
    print "------------------------------------------------------------"
    system("arm-none-eabi-addr2line -sCfe '$1' " $2);
  }
' < $2

exit 0
