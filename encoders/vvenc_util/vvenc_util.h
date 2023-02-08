#include "common.h"
#include "log.h"

#define VVENC_LIBRARY_EXPORT

#if defined(VVENC_LIBRARY_EXPORT) // inside DLL
#   define VVENC_LIBRARY_API   __declspec(dllexport)
#else // outside DLL
#   define VVENC_LIBRARY_API   __declspec(dllimport)
#endif  // XYZLIBRARY_EXPORT

#ifdef __cplusplus
extern "C"
{
#endif

    VVENC_LIBRARY_API int vvenc_init(H266Config *config);
    VVENC_LIBRARY_API int vvenc_start();
    VVENC_LIBRARY_API int vvenc_handle(H266Frame *frame);
    VVENC_LIBRARY_API int vvenc_stop();
    VVENC_LIBRARY_API int vvenc_flush();
    VVENC_LIBRARY_API int vvenc_close();

#ifdef __cplusplus 
}
#endif