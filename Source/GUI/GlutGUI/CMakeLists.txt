cmake_minimum_required(VERSION 3.10)

set(LIB_NAME GlutGUI)
set(LIB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

if(WIN32)                                                               
    link_directories("${PROJECT_SOURCE_DIR}/Extern/OpenGL/lib/Windows/X64")
    link_libraries(freeglut glew32)            
elseif(UNIX)
#    link_directories("${PROJECT_SOURCE_DIR}/Extern/OpenGL/lib/Linux/X64")
    link_libraries(glut GLEW GL GLU cudart)           
endif()

file(                                                                           
    GLOB_RECURSE LIB_SRC                                                        #利用glob命令读取所有源文件list
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "${LIB_SRC_DIR}/*.c*"
    "${LIB_SRC_DIR}/*.h*"
)

list(FILTER LIB_SRC EXCLUDE REGEX .*deprecated/.*)                              #排除deprecated 目录下面的所有文件

if(WIN32)
    add_library(${LIB_NAME} STATIC ${LIB_SRC})                                      #添加编译目标 静态链接库
elseif(UNIX)
    add_library(${LIB_NAME} SHARED ${LIB_SRC}) 
endif()

foreach(SRC IN ITEMS ${LIB_SRC})                                                #为VS工程添加filter 方便查看文件结构目录
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${LIB_SRC_DIR}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "${SRC_PATH_REL}")
    source_group("${GROUP_PATH}" FILES "${SRC}")
endforeach()

if(WIN32)
    target_compile_options(${LIB_NAME} PRIVATE -Xcompiler "/wd 4819")               #禁止编译时报告文件编码不是unicode的warning，由于cuda头文件都不是unicode。使编译报错更清晰
endif()
file(RELATIVE_PATH PROJECT_PATH_REL "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")                 #判断当前project在根目录下的相对路径
set_target_properties(${LIB_NAME} PROPERTIES FOLDER "Engine/GUI")      #为project设定folder目录
set_target_properties(${LIB_NAME} PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON)    #cmake 会自动延迟cuda dlink操作直到静态库被编译成动态库或者exe，
                                                                                #但这样影响了后面其他工程对PhysLeo.lib的引用，现在还有些原因未明，暂时在编译成库之前开启dlink