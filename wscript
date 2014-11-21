import sys
import os
sys.path.append(os.path.abspath('waf/waflib/extras'))

opencv_libs = [
    "IlmImf",
    "libjasper",
    "libjpeg",
    "libpng",
    "libtiff",
    "opencv_calib3d%(ver)s",
    "opencv_contrib%(ver)s",
    "opencv_core%(ver)s",
    "opencv_features2d%(ver)s",
    "opencv_flann%(ver)s",
    "opencv_gpu%(ver)s",
    "opencv_highgui%(ver)s",
    "opencv_imgproc%(ver)s",
    "opencv_legacy%(ver)s",
    "opencv_ml%(ver)s",
    "opencv_nonfree%(ver)s",
    "opencv_objdetect%(ver)s",
    "opencv_ocl%(ver)s",
    "opencv_photo%(ver)s",
    "opencv_stitching%(ver)s",
    "opencv_superres%(ver)s",
    "opencv_ts%(ver)s",
    "opencv_video%(ver)s",
    "opencv_videostab%(ver)s",
    "zlib"
]

def options(opt):
    opt.load('cxx msvc boost')
    opt.add_option('--opencv-lib', dest='opencv_lib', action='store',
        default=False, help='Path to opencv libs')
    opt.add_option('--opencv-include', dest='opencv_include', action='store',
        default=False, help='Path to opencv includes')
    opt.add_option('--opencv-ver', dest='opencv_ver', action='store',
        default=False, help='Opencv lib version')
                
def configure(conf):
    conf.load('cxx msvc boost')
    conf.env['MSVC_TARGETS'] = ['x86']
    
    conf.check_libs_msvc('kernel32 user32 gdi32 oleaut32 ole32 comctl32 vfw32 ws2_32 advapi32')
    conf.check_boost(stlib='thread system filesystem')

    conf.env.LIB_OPENCV = [opencv_lib % {'ver':conf.options.opencv_ver} for opencv_lib in opencv_libs]
    conf.env.LIBPATH_OPENCV = conf.options.opencv_lib
    conf.env.INCLUDES_OPENCV = conf.options.opencv_include
    
    conf.env.CXXFLAGS = '/EHsc'
    
    if sys.platform == 'win32':
        pass

def build(bld):

    tg = bld.program(
       features='cxx',
       source='testbed.cpp',
       target='testbed',
       use='OPENCV WS2_32 KERNEL32 USER32 GDI32 ADVAPI32 COMCTL32 OLEAUT32 OLE32 VFW32 BOOST'

    )
    tg = bld.program(
       features='cxx',
       source='greenshift.cpp',
       target='greenshift',
       use='OPENCV WS2_32 KERNEL32 USER32 GDI32 ADVAPI32 COMCTL32 OLEAUT32 OLE32 VFW32 BOOST'

    )