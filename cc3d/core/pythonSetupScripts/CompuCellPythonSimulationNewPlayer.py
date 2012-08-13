def pythonTracebackFormatter():
    import string
    import re
    import traceback
    import sys
    lineNumberExtractRegex=re.compile('^[\s\S]*line[\s]*([0-9]*)')
    # fileNameExtractRegex=re.compile('^[\s]*File[\s]*[\"]?([\s\S]*)[\"]?line[\s]*([0-9]*)')
    fileNameExtractRegex=re.compile('^[\s]*File[\s]*[\"]?([\s\S]*)["]')
    columnNumberExtractRegex=re.compile('^[\s\S]*column[\s]*([0-9]*)')      
    exc_type, exc_value, exc_traceback = sys.exc_info()
    
    
    formatted_lines = traceback.format_exc().splitlines()   
    errorDescriptionLine=formatted_lines[-1]
  
    
    tracebackText="Error: "+errorDescriptionLine+"\n"
    
    fileNameFound=False
    
    
    lineNumber=0
    colNumber=0
    lineHeaderLength=0
    
    skipLine=False
    
    for idx in  range(len(formatted_lines)-1):
    
        line = formatted_lines[idx]
        fileNameGroups=fileNameExtractRegex.search(str(line))
        fileName=""
        if skipLine:
            skipLine=False
            continue
        
        
        try: 
            if fileNameGroups:
                fileName=fileNameGroups.group(1)
                fileNameFound=True
            else:
                # tracebackText+="    "+line+"\n"    
                
                if lineNumber>0:                    
                    
                    
                    try:    
                        colNumber=string.index(formatted_lines[idx+1],'^')    
                    except ValueError, e:    
                        pass    
                    

                    
                    lineHeader="    "+"Line: "+str(lineNumber)+" Col: "+str(colNumber)+" "
                    lineHeaderLength=len(lineHeader)                    
                    tracebackText+=lineHeader+line+"\n"
                    print "lineHeaderLength=",lineHeaderLength
                    
                    if colNumber>0:
                        tracebackText+=" "*lineHeaderLength+formatted_lines[idx+1]+"\n"
                        print lineHeader+line+"\n"
                        print " "*lineHeaderLength+formatted_lines[idx+1]+"\n"
                        
                        skipLine=True
                    else:
                        tracebackText+="    "+line+"\n"
                    lineNumber=0
                    colNumber=0
                    
                else:
                    
                    tracebackText+="    "+line+"\n"                    
                
                continue
                # dbgMsg("Searched text at line=",lineNumber)
                
        except IndexError,e:
            tracebackText+="    "+line+"\n"    
            continue
    
        lineNumberGroups=lineNumberExtractRegex.search(str(line))
        try: 
            if lineNumberGroups:
                lineNumber=int(lineNumberGroups.group(1))
                # dbgMsg("Searched text at line=",lineNumber)
                
        except IndexError,e:
                
            print "\n\n\n\n Could not find line number"        
        # try:    
            # colNumber=string.index(line,'^')    
        # except ValueError, e:    
            # pass
        
        
        
        # tracebackText+="    "+"Line: "+str(lineNumber)+" col: "+str(colNumber)+" "+line+"\n"
        if fileNameFound:
            
            tracebackText+="  File: "+fileName+"\n"
            
                 
            
        
            
    return tracebackText


def pythonErrorFormatter():
    import traceback, sys
    exc_type, exc_value, exc_traceback = sys.exc_info()
    
    # print "******************************************************** print_tb:"
    # traceback.print_tb(exc_traceback, limit=2, file=sys.stdout)
    # print "******************************************************** print_tb:"    
    
    # print "*** ********************extract_tb:"
    # print repr(traceback.extract_tb(exc_traceback))
    # print "**************************** extract_tb:"
    # print
    
    
    
    print "****** format_exc, first and last line:"
    
    
    
    formatted_lines = traceback.format_exc().splitlines()    
    errorDescriptionLine=formatted_lines[-1]
    fileLine=formatted_lines[-4]
    codeLine=formatted_lines[-3]
    columnLine=formatted_lines[-2]
    
    print errorDescriptionLine
    print fileLine
    print codeLine
    print columnLine
    
    
    print "****** format_exc, first and last line:"
    
    import re
    lineNumberExtractRegex=re.compile('^[\s\S]*line[\s]*([0-9]*)')
    
    fileNameExtractRegex=re.compile('^[\s]*File[\s]*([\s\S]*)line[\s]*([0-9]*)')
    
    columnNumberExtractRegex=re.compile('^[\s\S]*column[\s]*([0-9]*)')
    
    
    
    
    lineNumberGroups=lineNumberExtractRegex.search(str(fileLine))
    
    print "\n\n\n lineNumberGroups=",lineNumberGroups
    lineNumber=-1
    colNumber=-1
    try: 
        if lineNumberGroups:
            lineNumber=int(lineNumberGroups.group(1))
            # dbgMsg("Searched text at line=",lineNumber)
            
    except IndexError,e:
            
        print "\n\n\n\n Could not find line number"
    
    print "ERROR IN LINE:",lineNumber
    
    fileNameGroups=fileNameExtractRegex.search(str(fileLine))
    print "\n\n\n fileNameGroups=",fileNameGroups
    fileName=""
    try: 
        if fileNameGroups:
            fileName=fileNameGroups.group(1)
            
            # dbgMsg("Searched text at line=",lineNumber)
            
    except IndexError,e:
            
        print "\n\n\n\n Could not find file name"
        
    print "\n\n\n fileName:",fileName
    # formatted_lines = traceback.format_exc().splitlines()

    # print "formatted_lines:",formatted_lines
    import string
    colNumber=string.index(columnLine,'^')
    
    print "COLUMN NUMBER:",colNumber
    return errorDescriptionLine,fileName,lineNumber,colNumber,codeLine

try:
    from xml.parsers.expat import ExpatError
    import sys
    from os import environ
    import string
    import traceback

    python_module_path=os.environ["PYTHON_MODULE_PATH"]
    appended=sys.path.count(python_module_path)
    if not appended:
        sys.path.append(python_module_path)    
    
    # sys.path.append(environ["PYTHON_MODULE_PATH"])
    import CompuCellSetup

    sim,simthread = CompuCellSetup.getCoreSimulationObjects(True) # this only parses xml to extract initial info. No CC3D object is created at this point

    if CompuCellSetup.simulationPaths.simulationPythonScriptName != "":
        # fileObj=file(CompuCellSetup.simulationPaths.simulationPythonScriptName,"r")
        # exec fileObj
        # fileObj.close()        
        print "INSIDE IF CompuCellPythonSimulationNewPlayer \n\n\n"
        
        
        execfile(CompuCellSetup.simulationPaths.simulationPythonScriptName)
    else:
        print "INSIDE ELSE CompuCellPythonSimulationNewPlayer \n\n\n"        
        sim,simthread = CompuCellSetup.getCoreSimulationObjects() # here , once initial info has been extracted we starrt creating CC3D objects - e.g. Simulator is created in this Fcn call
        import CompuCell #notice importing CompuCell to main script has to be done after call to getCoreSimulationObjects()
        #import CompuCellSetup

        CompuCellSetup.initializeSimulationObjects(sim,simthread)
        steppableRegistry = CompuCellSetup.getSteppableRegistry()
        CompuCellSetup.mainLoop(sim,simthread,steppableRegistry) # main loop - simulation is invoked inside this function

except IndentationError,e:
    print "CompuCellSetup.simulationObjectsCreated=",CompuCellSetup.simulationObjectsCreated
    if CompuCellSetup.simulationObjectsCreated:        
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Indentation Error",traceback_message)
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except AttributeError,e:
    print "CompuCellSetup.simulationObjectsCreated=",CompuCellSetup.simulationObjectsCreated
    if CompuCellSetup.simulationObjectsCreated:        
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Attribute Error",traceback_message)    
    simthread.handleErrorFormatted(pythonTracebackFormatter())
except SyntaxError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
    # simthread.handleErrorMessage("Python Syntax Error",traceback_message)
except ValueError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Value Error",traceback_message)
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except TypeError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Type Error",traceback_message)    
    simthread.handleErrorFormatted(pythonTracebackFormatter())

except KeyError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Key Error",traceback_message) 
    simthread.handleErrorFormatted(pythonTracebackFormatter())    
    
except IndexError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Index Error",traceback_message)  
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except LookupError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Lookup Error",traceback_message)
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
  
except NameError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Name Error",traceback_message)    
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except FloatingPointError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Floating Point Error",traceback_message) 
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except ZeroDivisionError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Zero Division Error",traceback_message) 
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except OverflowError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Overflow Error",traceback_message)    
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except ArithmeticError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Arithmetic Error",traceback_message)
    simthread.handleErrorFormatted(pythonTracebackFormatter())

except EOFError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python EOF Error",traceback_message)
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except IOError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python IO Error",traceback_message)
    simthread.handleErrorFormatted(pythonTracebackFormatter())

except OSError,e:
    if CompuCellSetup.simulationObjectsCreated:
        #sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python OS Error",traceback_message)
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except ImportError,e:
    if CompuCellSetup.simulationObjectsCreated:
        # sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # stack=traceback.extract_stack()
    # print "THISIS STACK: ",stack
    # simthread.handleErrorMessage("Python Import Error",traceback_message)
    # errorDescriptionLine,fileName,lineNumber,colNumber,codeLine=pythonErrorFormatter()    
    # simthread.handleErrorMessageDetailed(errorDescriptionLine,fileName,lineNumber,colNumber,codeLine)
    simthread.handleErrorFormatted(pythonTracebackFormatter())

# except BufferError,e:
    # if CompuCellSetup.simulationObjectsCreated:
        # # sim.finish()
        # sim.unloadModules()
    # traceback_message=traceback.format_exc()
    # print traceback_message
    # import PlayerPython
    # # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Buffer Error",traceback_message)    

except MemoryError,e:
    if CompuCellSetup.simulationObjectsCreated:
        # sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Memory Error",traceback_message) 
    simthread.handleErrorFormatted(pythonTracebackFormatter())    

except ReferenceError,e:
    if CompuCellSetup.simulationObjectsCreated:
        # sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Reference Error",traceback_message)
    simthread.handleErrorFormatted(pythonTracebackFormatter())

except RuntimeError,e:
    if CompuCellSetup.simulationObjectsCreated:
        # sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python Runtime Error",traceback_message)  
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except SystemError,e:
    if CompuCellSetup.simulationObjectsCreated:
        # sim.finish()
        sim.unloadModules()
    traceback_message=traceback.format_exc()
    print traceback_message
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    # simthread.handleErrorMessage("Python System Error",traceback_message)  
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except ExpatError,e:
    if CompuCellSetup.simulationObjectsCreated:
        # sim.finish()
        sim.unloadModules()
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    xmlFileName=CompuCellSetup.simulationPaths.simulationXMLFileName
    print "Error in XML File","File:\n "+xmlFileName+"\nhas the following problem\n"+e.message
    
    import re
    lineNumberExtractRegex=re.compile('^[\s\S]*line[\s]*([0-9]*)')
    columnNumberExtractRegex=re.compile('^[\s\S]*column[\s]*([0-9]*)')
    
    # simthread.handleErrorMessage("Error in XML File","File:\n "+xmlFileName+"\nhas the following problem\n"+e.message)
    print "SEARCHING MESSAGE=",e.message
    lineNumberGroups=lineNumberExtractRegex.search(str(e.message))
    
    print "\n\n\n lineNumberGroups=",lineNumberGroups
    lineNumber=-1
    colNumber=-1
    try: 
        if lineNumberGroups:
            lineNumber=int(lineNumberGroups.group(1))
            # dbgMsg("Searched text at line=",lineNumber)
            
    except IndexError,e:
            
        print "\n\n\n\n Could not find line number"
    
    columnNumberGroups=columnNumberExtractRegex.search(str(e.message))    
    print "\n\n\n columnNumberGroups=",columnNumberGroups
    try: 
        if columnNumberGroups:
            colNumber=int(columnNumberGroups.group(1))
            
            # dbgMsg("Searched text at line=",lineNumber)
            
    except IndexError,e:
            
        print "\n\n\n\n Could not find column number"
    
    # simthread.handleErrorMessageDetailed("Error in XML File",xmlFileName,lineNumber,colNumber,e.message)
    tracebackMessage="Error: Error in XML File"+"\n"
    tracebackMessage+="  File: "+xmlFileName+"\n"    
    tracebackMessage+="    Line: "+str(lineNumber)+" Col: "+str(colNumber)+" "+e.message
    simthread.handleErrorFormatted(tracebackMessage)

except AssertionError,e:
    if CompuCellSetup.simulationObjectsCreated:
        # sim.finish()
        sim.unloadModules()
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    print "Assertion Error: ",e.message
    # simthread.handleErrorMessage("Assertion Error",e.message)
    simthread.handleErrorFormatted(pythonTracebackFormatter())
    
except CompuCellSetup.CC3DCPlusPlusError,e:
    # for C++ exceptions we do not call sim.finish() because modules are unloaded immediately in the exception handling call in the C++
    # print "CompuCellSetup.simulationObjectsCreated=",CompuCellSetup.simulationObjectsCreated
    # if CompuCellSetup.simulationObjectsCreated:
        # sim.finish()
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()    
    print "RUNTIME ERROR IN C++ CODE: ",e.message
    simthread.handleErrorMessage("RUNTIME ERROR IN C++ CODE",e.message)
    # simthread.handleErrorFormatted(pythonTracebackFormatter())

except:
    # if CompuCellSetup.simulationObjectsCreated and CompuCellSetup.playerType=="new":
        # sim.finish()
    if CompuCellSetup.simulationObjectsCreated:
        # sim.finish()       
        sim.unloadModules()        
    traceback_message=traceback.format_exc()
    import PlayerPython
    # simthread=PlayerPython.getSimthreadBasePtr()
    print "Unexpected Error:",traceback_message
    # simthread.handleErrorMessage("Unexpected Error",traceback_message)
    simthread.handleErrorFormatted(pythonTracebackFormatter())