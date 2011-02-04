#/bin/bash

NVCC="/usr/local/cuda/bin/nvcc"
SOURCE_FILES="cuenergy_pre8_coalesce.cu main.cu"
INCLUDE_PATH="../../../dependencies/includes"
LIBRARIES_PATH="../../../dependencies/libraries"
EXECUTABLE_FILE="exec.bin"
LIBRARIES="-lparboil"
SO_FILES="/usr/local/cuda/lib64"
INCLUDES_AND_LIBRARIES="-I${INCLUDE_PATH} -L${LIBRARIES_PATH} ${LIBRARIES}"
PTX_CODE_DIR="exec.bin.devcode"
OUTPUT_FILE="instrumented_ptx_code.asm"
PROF_FILES=prof\.*\.txt
echo Cleaning folder...
rm -rvf ${PTX_CODE_DIR}
rm -vf ${OUTPUT_FILE}
rm -vf ${PROF_FILES}
echo Compiling dependences...
${NVCC} -arch compute_13 -int=none -ext=all -dir=${PTX_CODE_DIR} \
              ${INCLUDES_AND_LIBRARIES} ${SOURCE_FILES} -o ${EXECUTABLE_FILE}
PATH_TO_COMPUTE_13=`find . -name compute_13*`
echo Creating analysis directory...
mkdir -p Analysis
echo Copying raw PTX file to analysis directory...
cp ${PATH_TO_COMPUTE_13} Analysis
echo Instrumenting  \"${PATH_TO_COMPUTE_13}\"...
../../../ocelot/iptx -p -i ${PATH_TO_COMPUTE_13} -o ${OUTPUT_FILE}
mv $OUTPUT_FILE ${PATH_TO_COMPUTE_13}
echo Running this benchmark...
./run.sh
echo Moving prof files to Analysis directory...
mv -v ${PROF_FILES} Analysis
echo Analysing...
../../../ocelot/DB -i Analysis/compute_13 -s -p
