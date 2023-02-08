/* Copyright (c) 2023, InterDigital Communications, Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of InterDigital Communications, Inc nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "common.h"
#include "log.h"

#define LUMA_PADDING 8
#define CHROMA_PADDING LUMA_PADDING >> 1

#define UVG_LIBRARY_EXPORT

#if defined(UVG_LIBRARY_EXPORT) // inside DLL
#   define UVG_LIBRARY_API   __declspec(dllexport)
#else // outside DLL
#   define UVG_LIBRARY_API   __declspec(dllimport)
#endif  // XYZLIBRARY_EXPORT

#ifdef __cplusplus
extern "C"
{
#endif

    UVG_LIBRARY_API int uvg_init(H266Config *config);
    UVG_LIBRARY_API int uvg_start();
    UVG_LIBRARY_API int uvg_handle(H266Frame *frame);
    UVG_LIBRARY_API int uvg_stop();
    UVG_LIBRARY_API int uvg_flush();
    UVG_LIBRARY_API int uvg_close();

#ifdef __cplusplus 
}
#endif