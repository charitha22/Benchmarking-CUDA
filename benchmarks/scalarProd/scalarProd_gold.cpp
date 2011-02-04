/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NVIDIA Corporation and its licensors retain all intellectual property and
 * proprietary rights in and to this software and related documentation.
 * Any use, reproduction, disclosure, or distribution of this software
 * and related documentation without an express license agreement from
 * NVIDIA Corporation is strictly prohibited.
 *
 * Please refer to the applicable NVIDIA end user license agreement (EULA)
 * associated with this source code for terms and conditions that govern
 * your use of this NVIDIA software.
 *
 */



////////////////////////////////////////////////////////////////////////////
// Calculate scalar products of VectorN vectors of ElementN elements on CPU.
// Straight accumulation in double precision.
////////////////////////////////////////////////////////////////////////////
extern "C"
void scalarProdCPU(
    float *h_C,
    float *h_A,
    float *h_B,
    int vectorN,
    int elementN
){
    for(int vec = 0; vec < vectorN; vec++){
        int vectorBase = elementN * vec;
        int vectorEnd  = vectorBase + elementN;

        double sum = 0;
        for(int pos = vectorBase; pos < vectorEnd; pos++)
            sum += h_A[pos] * h_B[pos];

        h_C[vec] = (float)sum;
    }
}
