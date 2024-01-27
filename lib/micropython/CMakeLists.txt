# Set location of base MicroPython directory.
if(NOT MICROPY_DIR)
    get_filename_component(MICROPY_DIR ${CMAKE_CURRENT_LIST_DIR}/../.. ABSOLUTE)
endif()

set(SOURCES_PORT
    port/carbon_mp_test.c  
    port/gccollect.c  
    port/mphalport.c  
    port/mpthreadport.c
)

set(SOURCE_SHARED
	micropython_embed/shared/runtime/gchelper_thumb2.s
	micropython_embed/shared/runtime/gchelper_native.c
)

set(SOURCE_PY
	micropython_embed/py/argcheck.c          
	micropython_embed/py/emitnxtensawin.c  
	micropython_embed/py/nlr.c           
	micropython_embed/py/objgetitemiter.c   
	micropython_embed/py/pairheap.c
	micropython_embed/py/asmarm.c            
	micropython_embed/py/formatfloat.c     
	micropython_embed/py/nlrmips.c       
	micropython_embed/py/objint.c           
	micropython_embed/py/parse.c
	micropython_embed/py/asmbase.c           
	micropython_embed/py/frozenmod.c       
	micropython_embed/py/nlrpowerpc.c    
	micropython_embed/py/objint_longlong.c  
	micropython_embed/py/parsenumbase.c
	micropython_embed/py/asmthumb.c          
	micropython_embed/py/gc.c              
	micropython_embed/py/nlrsetjmp.c     
	micropython_embed/py/objint_mpz.c       
	micropython_embed/py/parsenum.c
	micropython_embed/py/asmx64.c            
	micropython_embed/py/lexer.c           
	micropython_embed/py/nlrthumb.c      
	micropython_embed/py/objlist.c          
	micropython_embed/py/persistentcode.c
	micropython_embed/py/asmx86.c            
	micropython_embed/py/malloc.c          
	micropython_embed/py/nlrx64.c        
	micropython_embed/py/objmap.c           
	micropython_embed/py/profile.c
	micropython_embed/py/asmxtensa.c         
	micropython_embed/py/map.c             
	micropython_embed/py/nlrx86.c        
	micropython_embed/py/objmodule.c        
	micropython_embed/py/pystack.c
	micropython_embed/py/bc.c                
	micropython_embed/py/modarray.c        
	micropython_embed/py/nlrxtensa.c     
	micropython_embed/py/objnamedtuple.c    
	micropython_embed/py/qstr.c
	micropython_embed/py/binary.c            
	micropython_embed/py/modbuiltins.c     
	micropython_embed/py/objarray.c      
	micropython_embed/py/objnone.c          
	micropython_embed/py/reader.c
	micropython_embed/py/builtinevex.c       
	micropython_embed/py/modcmath.c        
	micropython_embed/py/objattrtuple.c  
	micropython_embed/py/objobject.c        
	micropython_embed/py/repl.c
	micropython_embed/py/builtinhelp.c       
	micropython_embed/py/modcollections.c  
	micropython_embed/py/objbool.c       
	micropython_embed/py/objpolyiter.c      
	micropython_embed/py/ringbuf.c
	micropython_embed/py/builtinimport.c     
	micropython_embed/py/moderrno.c        
	micropython_embed/py/objboundmeth.c  
	micropython_embed/py/objproperty.c      
	micropython_embed/py/runtime.c
	micropython_embed/py/compile.c           
	micropython_embed/py/modgc.c           
	micropython_embed/py/obj.c           
	micropython_embed/py/objrange.c         
	micropython_embed/py/runtime_utils.c
	micropython_embed/py/emitbc.c            
	micropython_embed/py/modio.c           
	micropython_embed/py/objcell.c       
	micropython_embed/py/objreversed.c      
	micropython_embed/py/scheduler.c
	micropython_embed/py/emitcommon.c        
	micropython_embed/py/modmath.c         
	micropython_embed/py/objclosure.c    
	micropython_embed/py/objset.c           
	micropython_embed/py/scope.c
	micropython_embed/py/emitglue.c          
	micropython_embed/py/modmicropython.c  
	micropython_embed/py/objcomplex.c    
	micropython_embed/py/objsingleton.c     
	micropython_embed/py/sequence.c
	micropython_embed/py/emitinlinethumb.c   
	micropython_embed/py/modstruct.c       
	micropython_embed/py/objdeque.c      
	micropython_embed/py/objslice.c         
	micropython_embed/py/showbc.c
	micropython_embed/py/emitinlinextensa.c  
	micropython_embed/py/modsys.c          
	micropython_embed/py/objdict.c       
	micropython_embed/py/objstr.c           
	micropython_embed/py/smallint.c
	micropython_embed/py/emitnarm.c          
	micropython_embed/py/modthread.c       
	micropython_embed/py/objenumerate.c  
	micropython_embed/py/objstringio.c      
	micropython_embed/py/stackctrl.c
	micropython_embed/py/emitnative.c        
	micropython_embed/py/mpprint.c         
	micropython_embed/py/objexcept.c     
	micropython_embed/py/objstrunicode.c    
	micropython_embed/py/stream.c
	micropython_embed/py/emitnthumb.c        
	micropython_embed/py/mpstate.c         
	micropython_embed/py/objfilter.c     
	micropython_embed/py/objtuple.c         
	micropython_embed/py/unicode.c
	micropython_embed/py/emitnx64.c          
	micropython_embed/py/mpz.c             
	micropython_embed/py/objfloat.c      
	micropython_embed/py/objtype.c          
	micropython_embed/py/vm.c
	micropython_embed/py/emitnx86.c          
	micropython_embed/py/nativeglue.c      
	micropython_embed/py/objfun.c        
	micropython_embed/py/objzip.c           
	micropython_embed/py/vstr.c
	micropython_embed/py/emitnxtensa.c       
	micropython_embed/py/nlraarch64.c      
	micropython_embed/py/objgenerator.c  
	micropython_embed/py/opmethods.c        
	micropython_embed/py/warning.c
)

set(MICROPYTHON_LIB micropython_${PROJECT_NAME})

add_library(${MICROPYTHON_LIB} ${SOURCES_PORT} ${SOURCE_SHARED} ${SOURCE_PY})

target_compile_options(${MICROPYTHON_LIB} PUBLIC -Wno-vla-extension)

target_include_directories(${MICROPYTHON_LIB}
    PUBLIC
    .
    port
    micropython_embed
    ${PROJECT_CONFIG_DIR}
)

target_link_libraries(${MICROPYTHON_LIB} freertos_${PROJECT_NAME})
