# Set location of base MicroPython directory.
if(NOT MICROPY_DIR)
    get_filename_component(MICROPY_DIR ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
endif()

execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/mpconfigport.h .)
execute_process(COMMAND make V=1 ROOT_PROJECT=${PROJECT_ROOT_DIR} -f ${CMAKE_CURRENT_SOURCE_DIR}/embed.mk)

set(SOURCES_PORT
    ${CURRENT_BUILD_DIR}/micropython_embed/port/carbon_mp_test.c  
    ${CURRENT_BUILD_DIR}/micropython_embed/port/gccollect.c  
    ${CURRENT_BUILD_DIR}/micropython_embed/port/mphalport.c  
    ${CURRENT_BUILD_DIR}/micropython_embed/port/mpthreadport.c
)

set(SOURCE_SHARED
	${CURRENT_BUILD_DIR}/micropython_embed/shared/runtime/gchelper_thumb2.s
	${CURRENT_BUILD_DIR}/micropython_embed/shared/runtime/gchelper_native.c
)

set(SOURCE_PY
	${CURRENT_BUILD_DIR}/micropython_embed/py/argcheck.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitnxtensawin.c  
	${CURRENT_BUILD_DIR}/micropython_embed/py/nlr.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/objgetitemiter.c   
	${CURRENT_BUILD_DIR}/micropython_embed/py/pairheap.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/asmarm.c            
	${CURRENT_BUILD_DIR}/micropython_embed/py/formatfloat.c     
	${CURRENT_BUILD_DIR}/micropython_embed/py/nlrmips.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/objint.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/parse.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/asmbase.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/frozenmod.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/nlrpowerpc.c    
	${CURRENT_BUILD_DIR}/micropython_embed/py/objint_longlong.c  
	${CURRENT_BUILD_DIR}/micropython_embed/py/parsenumbase.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/asmthumb.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/gc.c              
	${CURRENT_BUILD_DIR}/micropython_embed/py/nlrsetjmp.c     
	${CURRENT_BUILD_DIR}/micropython_embed/py/objint_mpz.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/parsenum.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/asmx64.c            
	${CURRENT_BUILD_DIR}/micropython_embed/py/lexer.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/nlrthumb.c      
	${CURRENT_BUILD_DIR}/micropython_embed/py/objlist.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/persistentcode.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/asmx86.c            
	${CURRENT_BUILD_DIR}/micropython_embed/py/malloc.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/nlrx64.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/objmap.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/profile.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/asmxtensa.c         
	${CURRENT_BUILD_DIR}/micropython_embed/py/map.c             
	${CURRENT_BUILD_DIR}/micropython_embed/py/nlrx86.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/objmodule.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/pystack.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/bc.c                
	${CURRENT_BUILD_DIR}/micropython_embed/py/modarray.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/nlrxtensa.c     
	${CURRENT_BUILD_DIR}/micropython_embed/py/objnamedtuple.c    
	${CURRENT_BUILD_DIR}/micropython_embed/py/qstr.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/binary.c            
	${CURRENT_BUILD_DIR}/micropython_embed/py/modbuiltins.c     
	${CURRENT_BUILD_DIR}/micropython_embed/py/objarray.c      
	${CURRENT_BUILD_DIR}/micropython_embed/py/objnone.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/reader.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/builtinevex.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/modcmath.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/objattrtuple.c  
	${CURRENT_BUILD_DIR}/micropython_embed/py/objobject.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/repl.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/builtinhelp.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/modcollections.c  
	${CURRENT_BUILD_DIR}/micropython_embed/py/objbool.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/objpolyiter.c      
	${CURRENT_BUILD_DIR}/micropython_embed/py/ringbuf.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/builtinimport.c     
	${CURRENT_BUILD_DIR}/micropython_embed/py/moderrno.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/objboundmeth.c  
	${CURRENT_BUILD_DIR}/micropython_embed/py/objproperty.c      
	${CURRENT_BUILD_DIR}/micropython_embed/py/runtime.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/compile.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/modgc.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/obj.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/objrange.c         
	${CURRENT_BUILD_DIR}/micropython_embed/py/runtime_utils.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitbc.c            
	${CURRENT_BUILD_DIR}/micropython_embed/py/modio.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/objcell.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/objreversed.c      
	${CURRENT_BUILD_DIR}/micropython_embed/py/scheduler.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitcommon.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/modmath.c         
	${CURRENT_BUILD_DIR}/micropython_embed/py/objclosure.c    
	${CURRENT_BUILD_DIR}/micropython_embed/py/objset.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/scope.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitglue.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/modmicropython.c  
	${CURRENT_BUILD_DIR}/micropython_embed/py/objcomplex.c    
	${CURRENT_BUILD_DIR}/micropython_embed/py/objsingleton.c     
	${CURRENT_BUILD_DIR}/micropython_embed/py/sequence.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitinlinethumb.c   
	${CURRENT_BUILD_DIR}/micropython_embed/py/modstruct.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/objdeque.c      
	${CURRENT_BUILD_DIR}/micropython_embed/py/objslice.c         
	${CURRENT_BUILD_DIR}/micropython_embed/py/showbc.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitinlinextensa.c  
	${CURRENT_BUILD_DIR}/micropython_embed/py/modsys.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/objdict.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/objstr.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/smallint.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitnarm.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/modthread.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/objenumerate.c  
	${CURRENT_BUILD_DIR}/micropython_embed/py/objstringio.c      
	${CURRENT_BUILD_DIR}/micropython_embed/py/stackctrl.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitnative.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/mpprint.c         
	${CURRENT_BUILD_DIR}/micropython_embed/py/objexcept.c     
	${CURRENT_BUILD_DIR}/micropython_embed/py/objstrunicode.c    
	${CURRENT_BUILD_DIR}/micropython_embed/py/stream.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitnthumb.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/mpstate.c         
	${CURRENT_BUILD_DIR}/micropython_embed/py/objfilter.c     
	${CURRENT_BUILD_DIR}/micropython_embed/py/objtuple.c         
	${CURRENT_BUILD_DIR}/micropython_embed/py/unicode.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitnx64.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/mpz.c             
	${CURRENT_BUILD_DIR}/micropython_embed/py/objfloat.c      
	${CURRENT_BUILD_DIR}/micropython_embed/py/objtype.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/vm.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitnx86.c          
	${CURRENT_BUILD_DIR}/micropython_embed/py/nativeglue.c      
	${CURRENT_BUILD_DIR}/micropython_embed/py/objfun.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/objzip.c           
	${CURRENT_BUILD_DIR}/micropython_embed/py/vstr.c
	${CURRENT_BUILD_DIR}/micropython_embed/py/emitnxtensa.c       
	${CURRENT_BUILD_DIR}/micropython_embed/py/nlraarch64.c      
	${CURRENT_BUILD_DIR}/micropython_embed/py/objgenerator.c  
	${CURRENT_BUILD_DIR}/micropython_embed/py/opmethods.c        
	${CURRENT_BUILD_DIR}/micropython_embed/py/warning.c
)

set(MICROPYTHON_LIB micropython_${PROJECT_NAME})

add_library(${MICROPYTHON_LIB} ${SOURCES_PORT} ${SOURCE_SHARED} ${SOURCE_PY})

target_compile_options(${MICROPYTHON_LIB} PUBLIC -Wno-vla -Wno-undef -Wno-stack-usage)

target_include_directories(${MICROPYTHON_LIB}
    PUBLIC
    ${CURRENT_BUILD_DIR}
    ${CURRENT_BUILD_DIR}/micropython_embed/port
    ${CURRENT_BUILD_DIR}/micropython_embed
    ${PROJECT_CONFIG_DIR}
)

target_link_libraries(${MICROPYTHON_LIB} freertos_${PROJECT_NAME})
