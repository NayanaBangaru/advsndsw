#!/bin/bash -e
if [[ "$WORK_DIR" == '' ]]; then
  echo 'Please, define $WORK_DIR'
  exit 1
fi
OP=rhel9_x86-64/advsndsw/main-local1
PP=${PKGPATH:-rhel9_x86-64/advsndsw/main-local1}
PH=e563c14a94bc8f5fbec7d19487c37baa2e9ee394
mv -f $PP/etc/modulefiles/advsndsw $PP/etc/modulefiles/advsndsw.forced-relocation && sed -e "s|[@][@]PKGREVISION[@]$PH[@][@]|local1|g" $PP/etc/modulefiles/advsndsw.forced-relocation > $PP/etc/modulefiles/advsndsw
