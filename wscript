import sys
import os

opencv_libs = [
"IlmImf",
"libjasper",
"libjpeg",
"libpng",
"libtiff",
"opencv_calib3d249",
"opencv_contrib249",
"opencv_core249",
"opencv_features2d249",
"opencv_flann249",
"opencv_gpu249",
"opencv_highgui249",
"opencv_imgproc249",
"opencv_legacy249",
"opencv_ml249",
"opencv_nonfree249",
"opencv_objdetect249",
"opencv_ocl249",
"opencv_photo249",
"opencv_stitching249",
"opencv_superres249",
"opencv_ts249",
"opencv_video249",
"opencv_videostab249",
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
    
    opencv_dir = r"D:\OpenCV\opencv\build"
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