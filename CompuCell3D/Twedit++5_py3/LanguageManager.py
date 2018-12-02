"""
 Need to add more languages
# """
# from PyQt4.QtCore import *
# from PyQt4.QtGui import *
# from PyQt4.Qsci import *
# import PyQt4.Qsci
#
# from PyQt4 import QtCore, QtGui
from .utils.global_imports import *

from .Messaging import stdMsg, dbgMsg, errMsg, dbgMsg
from . import Configuration
import os
import sys


class LanguageManager:
    def __init__(self, _editorWindow):
        self.editorWindow = _editorWindow
        self.actionDict = {}
        self.lexerObjectDict = {}  # it maps lexer class name to lexer class object e.g. "QsciLexerCPP":QsciLexerCPP()
        self.languageMapper = QSignalMapper(self.editorWindow)
        # self.editorWindow.connect(self.languageMapper,SIGNAL("mapped(const QString&)"),  self.selectLexer)
        self.languageMapper.mapped[str].connect(self.selectLexer)

        self.actionChecked = None
        self.importAllAvailableLexers()
        self.apiDict = {}
        self.installAutocompletionAPIs()
        # format [lexer,begin comment, end comment, brace matching (0- nor matching, 1 matching), codeFolding]
        # e.g. "Bash":[QsciLexerBash(),"# ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        self.languageLexerDictionary = {}
        self.lexerLanguageDictionary = {}

        # self.languageLexerDictionary={
        # "Bash":[QsciLexerBash(),"# ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Batch":[QsciLexerBatch(),"REM ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "C":[QsciLexerCPP(),"// ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "C#":[QsciLexerCSharp(),"// ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "CSS":[QsciLexerCSS(),"/* "," */",1,5,QsciScintilla.SCWS_INVISIBLE],
        # "D":[QsciLexerD(),"// ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Diff":[QsciLexerDiff(),None,None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Python":[QsciLexerPython(),"# ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "CMake":[QsciLexerCMake(),"# ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Fortran":[QsciLexerFortran(),"! ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Fortran77":[QsciLexerFortran77(),"c ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "HTML":[QsciLexerHTML(),"<!-- "," -->",1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Java":[QsciLexerJava(),"// ", None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "JavaScript":[QsciLexerJavaScript(),"// ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Lua":[QsciLexerLua(),"-- ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Makefile":[QsciLexerMakefile(),"# ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Pascal":[QsciLexerPascal(),"{ "," }",1,5,QsciScintilla.SCWS_INVISIBLE],        
        # "Perl":[QsciLexerPerl(),"# ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "PostScript":[QsciLexerPostScript(),"% ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Properties":[QsciLexerProperties(),"; ",None,1,5,QsciScintilla.SCWS_INVISIBLE],        
        # "POV":[QsciLexerPOV(),"// ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # #"Spice":[QsciLexerSpice(),"* ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "SQL":[QsciLexerSQL(),"/* "," */",1,5,QsciScintilla.SCWS_INVISIBLE],
        # "Ruby":[QsciLexerRuby(),"# ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "TCL":[QsciLexerTCL(),"# ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # #"Verilog":[QsciLexerVerilog(),"// ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "VHDL":[QsciLexerVHDL(),"-- ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "TeX":[QsciLexerTeX(),"# ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # "XML":[QsciLexerXML(),"<!-- "," -->",1,5,QsciScintilla.SCWS_INVISIBLE],
        # "YML":[QsciLexerYAML(),"# ",None,1,5,QsciScintilla.SCWS_INVISIBLE],
        # }

        self.addLanguageLexerDictionaryEntry("Bash", ["QsciLexerBash", "# ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Batch",
                                             ["QsciLexerBatch", "REM ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("C", ["QsciLexerCPP", "// ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("C#", ["QsciLexerCSharp", "// ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("CSS", ["QsciLexerCSS", "/* ", " */", 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("D", ["QsciLexerD", "// ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Diff", ["QsciLexerDiff", None, None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Python",
                                             ["QsciLexerPython", "# ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("CMake",
                                             ["QsciLexerCMake", "# ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Fortran",
                                             ["QsciLexerFortran", "! ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Fortran77",
                                             ["QsciLexerFortran77", "c ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("HTML",
                                             ["QsciLexerHTML", "<!-- ", " -->", 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("IDL", ["QsciLexerIDL", "; ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Java", ["QsciLexerJava", "// ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("JavaScript",
                                             ["QsciLexerJavaScript", "// ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Lua", ["QsciLexerLua", "-- ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Makefile",
                                             ["QsciLexerMakefile", "# ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Matlab",
                                             ["QsciLexerMatlab", "% ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Octave",
                                             ["QsciLexerOctave", "% ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Pascal",
                                             ["QsciLexerPascal", "{ ", " }", 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Perl", ["QsciLexerPerl", "# ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("PostScript",
                                             ["QsciLexerPostScript", "% ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Properties",
                                             ["QsciLexerProperties", "; ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("POV", ["QsciLexerPOV", "// ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Spice",
                                             ["QsciLexerSpice", "* ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("SQL", ["QsciLexerSQL", "/* ", " */", 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Ruby", ["QsciLexerRuby", "# ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("TCL", ["QsciLexerTCL", "# ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("Verilog",
                                             ["QsciLexerVerilog", "// ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("VHDL", ["QsciLexerVHDL", "-- ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("TeX", ["QsciLexerTeX", "# ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("XML",
                                             ["QsciLexerXML", "<!-- ", " -->", 1, 5, QsciScintilla.SCWS_INVISIBLE])
        self.addLanguageLexerDictionaryEntry("YML", ["QsciLexerYAML", "# ", None, 1, 5, QsciScintilla.SCWS_INVISIBLE])

        self.lDict = {"XML": QsciLexerXML()}

    def importAllAvailableLexers(self):
        lexerNames = ["QsciLexerBash", "QsciLexerBatch", "QsciLexerCPP", "QsciLexerCSharp", "QsciLexerCSS",
                      "QsciLexerD", "QsciLexerDiff", "QsciLexerPython", "QsciLexerCMake", \
                      "QsciLexerFortran", "QsciLexerFortran77", "QsciLexerHTML", "QsciLexerIDL", "QsciLexerJava",
                      "QsciLexerJavaScript", "QsciLexerLua", "QsciLexerMakefile", "QsciLexerMatlab", "QsciLexerOctave",
                      "QsciLexerPascal", "QsciLexerPerl", \
                      "QsciLexerPostScript", "QsciLexerProperties", "QsciLexerPOV", "QsciLexerSpice", "QsciLexerSQL",
                      "QsciLexerRuby", "QsciLexerTCL", "QsciLexerVerilog", "QsciLexerVHDL", "QsciLexerTeX", \
                      "QsciLexerXML", "QsciLexerYAML"]

        for lexerName in lexerNames:
            try:
                # exec ("from PyQt4.Qsci import " + lexerName + "\n")
                exec ("from PyQt5.Qsci import " + lexerName + "\n")
                self.lexerObjectDict[lexerName] = eval(lexerName + "()")
            except ImportError:
                pass

    def loadSingleAPI(self, _lexerName, _apiName):
        try:
            self.apiDict[_lexerName] = QsciAPIs(self.lexerObjectDict[_lexerName])
            self.apiDict[_lexerName].load(_apiName)
            self.apiDict[_lexerName].prepare()
            self.lexerObjectDict[_lexerName].setAPIs(self.apiDict[_lexerName])
        except KeyError:
            return

    def installAutocompletionAPIs(self):
        # first determine where APIs are located
        # initial guess is in the "APIs" subrirestory of the directory which holds Configuration.py        
        tweditRootPath = os.path.dirname(Configuration.__file__)
        apisPath = os.path.join(tweditRootPath, "APIs")
        # check if it exists
        if not os.path.exists(apisPath):
            # when packaging on Windows with pyinstaller the path to executable is accesible via sys.executable as Python is bundled with the distribution
            # os.path.dirname(Configuration.__file__) returned by pyInstaller will not work without some modifications so it is best tu use os.path.dirname(sys.executable) approach
            tweditRootPath = os.path.dirname(sys.executable)
            apisPath = os.path.join(tweditRootPath, "APIs")

        dbgMsg("apisPath=", os.path.abspath(apisPath))

        self.loadSingleAPI("QsciLexerCPP", os.path.abspath(os.path.join(apisPath, "cplusplus.api")))
        self.loadSingleAPI("QsciLexerCSharp", os.path.abspath(os.path.join(apisPath, "csharp.api")))
        self.loadSingleAPI("QsciLexerCSS", os.path.abspath(os.path.join(apisPath, "css.api")))
        self.loadSingleAPI("QsciLexerHTML", os.path.abspath(os.path.join(apisPath, "html.api")))
        self.loadSingleAPI("QsciLexerJava", os.path.abspath(os.path.join(apisPath, "java.api")))
        self.loadSingleAPI("QsciLexerJavaScript", os.path.abspath(os.path.join(apisPath, "javascript.api")))
        self.loadSingleAPI("QsciLexerPearl", os.path.abspath(os.path.join(apisPath, "perl.api")))
        self.loadSingleAPI("QsciLexerPython", os.path.abspath(os.path.join(apisPath, "python.api")))
        self.loadSingleAPI("QsciLexerRuby", os.path.abspath(os.path.join(apisPath, "ruby.api")))

    def addLanguageLexerDictionaryEntry(self, _languageName, _languagePropertiesData):
        """
            _languagePropertiesData=["QsciLexerTeX","# ",None,1,5,QsciScintilla.SCWS_INVISIBLE]
        """

        try:
            _languagePropertiesData[0] = self.lexerObjectDict[_languagePropertiesData[0]]

            self.languageLexerDictionary[_languageName] = _languagePropertiesData

            self.lexerLanguageDictionary[self.languageLexerDictionary[_languageName][0]] = _languageName

        except (KeyError, IndexError):
            pass

    def createActions(self):
        keys = list(self.languageLexerDictionary.keys())
        keys.sort()
        for key in keys:
            action = self.editorWindow.languageMenu.addAction(key)
            self.actionDict[key] = action
            action.setCheckable(True)
            # self.editorWindow.connect(action, SIGNAL("triggered()"), self.languageMapper, SLOT("map()"))
            action.triggered.connect(self.languageMapper.map)
            self.languageMapper.setMapping(action, key)
            # self.actionDict[key]=QtGui.QAction(key, self, shortcut="",
            # statusTip=key, triggered=self.increaseIndent)

    def selectLexerBasedOnLexerObject(self, _lexerObj):
        try:
            languageName = self.lexerLanguageDictionary[_lexerObj]
            self.updateLexerActions(languageName)
        except (KeyError, IndexError):
            self.updateLexerActions('')  # no lexer is selected
            pass

    def updateLexerActions(self, _language):
        # print "selecting language ", _language
        if _language == '':  # in case language could not be figured out we uncheck current action (if one is checked) in the Language menu
            if self.actionChecked:
                self.actionChecked.setChecked(False)
            return
        try:
            if self.actionChecked:
                self.actionChecked.setChecked(False)
                self.actionDict[str(_language)].setChecked(True)
                self.actionChecked = self.actionDict[str(_language)]
            else:
                self.actionDict[str(_language)].setChecked(True)
                self.actionChecked = self.actionDict[str(_language)]
        except (KeyError, IndexError):
            pass

    def selectLexer(self, _language):
        # print "selecting language ", _language  
        try:
            if self.actionChecked:
                self.actionChecked.setChecked(False)
                self.actionDict[str(_language)].setChecked(True)
                self.actionChecked = self.actionDict[str(_language)]
            else:
                self.actionDict[str(_language)].setChecked(True)
                self.actionChecked = self.actionDict[str(_language)]

            # setting Lexer
            editor = self.editorWindow.getActiveEditor()
            editor.setLexer(self.languageLexerDictionary[str(_language)][0])
            editor.lexer().setFont(self.editorWindow.baseFont)
            # print "commentStyle=",self.languageLexerDictionary[str(_language)][1:3]
            self.editorWindow.commentStyleDict[editor] = self.languageLexerDictionary[str(_language)][1:3]

            self.editorWindow.setEditorProperties(
                editor)  # after changing lexer we have to set its properties incluging those defined by themes
        except (KeyError, IndexError):
            pass
