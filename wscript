import sys
import os

minor = "10"

opencv_libs = [
"IlmImf",
"libjasper",
"libjpeg",
"libpng",
"libtiff",
"opencv_calib3d24"+minor,
"opencv_contrib24"+minor,
"opencv_core24"+minor,
"opencv_features2d24"+minor,
"opencv_flann24"+minor,
"opencv_gpu24"+minor,
"opencv_highgui24"+minor,
"opencv_imgproc24"+minor,
"opencv_legacy24"+minor,
"opencv_ml24"+minor,
"opencv_nonfree24"+minor,
"opencv_objdetect24"+minor,
"opencv_ocl24"+minor,
"opencv_photo24"+minor,
"opencv_stitching24"+minor,
"opencv_superres24"+minor,
"opencv_ts24"+minor,
"opencv_video24"+minor,
"opencv_videostab24"+minor,
"zlib"]

def options(opt):
    opt.load('cxx msvc')

def configure(conf):

    conf.load('cxx msvc')
    conf.env['MSVC_TARGETS'] = ['x86']

    conf.env.LIB_ADVAPI32 = 'advapi32'
    conf.env.LIB_WS2_32 = 'ws2_32'
    conf.env.LIB_KERNEL32 = 'kernel32'
    conf.env.LIB_USER32 = 'user32'
    conf.env.LIB_GDI32 = 'gdi32'
    conf.env.LIB_COMCTL32 = 'comctl32'
    conf.env.LIB_OLEAUT32 = 'oleaut32'
    conf.env.LIB_OLE32 = 'ole32'
    conf.env.LIB_VFW32 = 'vfw32'
    
    opencv_dir = r"D:\opencv\build"
    conf.env.LIB_OPENCV = opencv_libs
    conf.env.LIBPATH_OPENCV = os.path.join(opencv_dir, "x86", "vc11", "staticlib")
    conf.env.INCLUDES_OPENCV = [os.path.join(opencv_dir, "include")]
    conf.env.CXXFLAGS_OPENCV = ''
    if sys.platform == 'win32':
        pass

def build(bld):

    tg = bld.program(
       features='cxx',
       source='testbed.cpp',
       target='testbed',
       use='OPENCV WS2_32 KERNEL32 USER32 GDI32 ADVAPI32 COMCTL32 OLEAUT32 OLE32 VFW32'

    )
    tg = bld.program(
       features='cxx',
       source='greenshift.cpp',
       target='greenshift',
       use='OPENCV WS2_32 KERNEL32 USER32 GDI32 ADVAPI32 COMCTL32 OLEAUT32 OLE32 VFW32'

    )