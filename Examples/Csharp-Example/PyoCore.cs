﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;     // DLL support

namespace PyoCore
{
    class PyoCore
    {
        [DllImport("PyoCore.dll")]
        public static extern bool processImageFileW (
            [param: MarshalAs(UnmanagedType.LPWStr)]
            String imageFileName,
            [param: MarshalAs(UnmanagedType.I4)]
            ImageFileType imageFileType,
            [param: MarshalAs(UnmanagedType.LPWStr)]
            StringBuilder resultBuffer,
            [param: MarshalAs(UnmanagedType.U4)]
            uint resultBufferLen,
            [param: MarshalAs(UnmanagedType.Bool)]
            Boolean isDebug
            );
        
        [DllImport("PyoCore.dll")]
        public static extern bool processImageFileA (
            [param: MarshalAs(UnmanagedType.LPStr)]
            String imageFileName,
            [param: MarshalAs(UnmanagedType.I4)]
            ImageFileType imageFileType,
            [param: MarshalAs(UnmanagedType.LPStr)]
            StringBuilder resultBuffer,
            [param: MarshalAs(UnmanagedType.U4)]
            uint resultBufferLen,
            [param: MarshalAs(UnmanagedType.Bool)]
            Boolean isDebug
            );

        [DllImport("PyoCore.dll")]
        public static extern ErrorCode getErrorCode();
    }

    enum ImageFileType
    {
        IMAGE_FILE_TYPE_PNG,

        /* Not image file type. Just for the number of image file types */
        IMAGE_FILE_TYPE_CNT
    };

    enum ErrorCode
    {
        /* No error */
        ERROR_NONE,
        /* Unknown error */
        ERROR_UNKNOWN,
        /* Not supported image file type */
        ERROR_IMAGE_FILE_TYPE,

        /* Not error code. Just for the number of error codes */
        ERROR_CODE_CNT
    }
}
