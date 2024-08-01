ALIBUILD_ARCH_PREFIX=${ALIBUILD_ARCH_PREFIX:-rhel9_x86-64}
[ -z ${GENERATORS_REVISION+x} ] && source ${WORK_DIR}/${ALIBUILD_ARCH_PREFIX}/generators/v1.0-local1/etc/profile.d/init.sh
[ -z ${SIMULATION_REVISION+x} ] && source ${WORK_DIR}/${ALIBUILD_ARCH_PREFIX}/simulation/v1.0-local1/etc/profile.d/init.sh
[ -z ${DEFAULTS_RELEASE_REVISION+x} ] && source ${WORK_DIR}/${ALIBUILD_ARCH_PREFIX}/defaults-release/v1-local1/etc/profile.d/init.sh
export ADVSNDSW_ROOT=${WORK_DIR}/${ALIBUILD_ARCH_PREFIX}/advsndsw/main-local1
export ADVSNDSW_VERSION=main
export ADVSNDSW_REVISION=local1
export ADVSNDSW_HASH=e563c14a94bc8f5fbec7d19487c37baa2e9ee394
export ADVSNDSW_COMMIT=0
export LD_LIBRARY_PATH=$ADVSNDSW_ROOT/lib${LD_LIBRARY_PATH+:$LD_LIBRARY_PATH}
export PATH=$ADVSNDSW_ROOT/bin${PATH+:$PATH}
