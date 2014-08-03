#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os
import os.path
import re
import shutil
import logging

log = logging.getLogger("formatter") #root logger, debug, info, warn, error, critical

#format = logging.Formatter('%(levelname)8s:%(funcName)23s:%(lineno)3d: %(message)s')
format = logging.Formatter('%(levelname)8s:%(module)10s:%(funcName)20s:%(lineno)3d: %(message)s')
fh = logging.FileHandler("formatter.log", mode="w")
fh.setFormatter(format)

sh = logging.StreamHandler() #console
sh.setFormatter(format)

log.addHandler(sh)
log.addHandler(fh)

log.setLevel(logging.DEBUG)

class FormatMgr(object):
    FILE_EXTENSIONS = [".hpp", ".cpp"]
    HEADER_GUARD_INFO_TOP_DIR = "src"
    HEADER_GUARD_INFO_PREFIX = "NDNS"
    def __init__(self, rootdir="./src", excludes=[]):
        self.rootdir = rootdir
        self.excludes = excludes
            
    def run(self):
        bakdir = self.rootdir+"-bak"
        if os.path.exists(bakdir):
            shutil.rmtree(bakdir)
            
        shutil.copytree(self.rootdir, self.rootdir+"-bak", ignore=shutil.ignore_patterns('*.pyc', 'tmp*', "~$", "^.", "*.log"))  
        
        for root, dirs, files in os.walk(self.rootdir):
            for file in files:
                fbase, fext = os.path.splitext(file)
                if not fext in FormatMgr.FILE_EXTENSIONS:
                    continue
                
                print file
                filepath = os.path.join(root, file)
                header_guard_info = self.get_header_guard_info(root, file)
                format = FileFormat(filepath=filepath, header_guard_info=header_guard_info)
                
                format.run()
    
    
    def get_header_guard_info(self, fdir, fname):
        temp = fdir.rfind(FormatMgr.HEADER_GUARD_INFO_TOP_DIR)
        temp = fdir[temp+len(FormatMgr.HEADER_GUARD_INFO_TOP_DIR):]
        temp = FormatMgr.HEADER_GUARD_INFO_PREFIX + temp.replace("/", "_")
        t2 = fname.replace("-", "_")
        t2 = t2.replace(".", "_")
        temp = temp+"_" + t2
        return temp.upper()
    
    def format():
        format = FileFormat()
        format.process()


class FileFormat:
    
    IF_N_DEF_KEYWORD = "#ifndef"
    DEFINE_KEYWORD = "#define"
    END_IF_KEYWORD = "#endif"
    PARA_FIRST_PATTERN = re.compile("^:.*\(.*\),$")
    PARA_MIDDLE_PATTERN = re.compile("\(.*\),$")
    PARA_LAST_PATTERN = re.compile("\(.*\)$")
    COPYRIGHT_LAST = r"* NDNS, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>."
    AUTO_LINE = re.compile("// TODO Auto-generated .* stub")
    
    def __init__(self, filepath, **kwargs):
        self.filepath = filepath
        self.header_guard_info = kwargs.get("header_guard_info", None)
        self.lines = None;
        
        self.first_code_line = -1
        self.guard_info_lines = [-1, -1, -1]
        self.func_paras = []
        self.last_blank_lines = []
        self.auto_lines = []
        
    def run(self):
        self.read()
        
        self.do_guard_info()
        
        self.do_construction_paras()
        
        self.do_copyright()
        
        self.do_auto_line()
        
        self.do_last_blank_line()
        
        self.write()
        
        
    def read(self):
        file = open(self.filepath, "rw")
        self.lines = file.readlines()
        
        i = 0
        while i < len(self.lines):
            line = self.lines[i]
            line = line.strip()
            if (line == None or line == ""):
               pass
           
            if line == FileFormat.COPYRIGHT_LAST:
                self.first_code_line = i + 2
            elif re.findall(FileFormat.AUTO_LINE, line):
                self.auto_lines.append(i)
            elif line.startswith(FileFormat.IF_N_DEF_KEYWORD):
                self.guard_info_lines[0] = i
            elif line.startswith(FileFormat.DEFINE_KEYWORD):
                self.guard_info_lines[1] = i
            elif line.startswith(FileFormat.END_IF_KEYWORD):
                self.guard_info_lines[2] = i
            
            elif re.findall(FileFormat.PARA_FIRST_PATTERN, line):
                temp = [i, 0]
                #log.debug("match first consturction paras. i=%d. line=%s" %(i, self.lines[i][:-1]))
                j = i + 1
                while j < len(self.lines):
                    line = self.lines[j]
                    line = line.strip()
                    #print line
                    if (line == None or line == ""):
                        pass
                    else:
                        if re.findall(FileFormat.PARA_MIDDLE_PATTERN, line):
                            pass
                        else:
                            temp[1] = j
                            if re.findall(FileFormat.PARA_LAST_PATTERN, line):
                                self.func_paras.append(temp)
                                #log.debug("add a construction para list. i=%d.    %s" %(j, self.lines[j][:-1]))
                            else:
                                pass
                                #log.debug("this is not the construction para list. i=%d.    line=%s" %(j, self.lines[j][:-1]))
                            
                            i = j
                            break

                    j += 1        
                
            i += 1    
        
        
        i = len(self.lines) - 1
        while i > 0:
            line = self.lines[i]
            line = line.strip()
            if (line == None or line == ""):
                self.last_blank_lines.append(i)
            else:
                break
            i -= 1
        
        if self.guard_info_lines[0] == -1:
            log.info("no guard info")
        else:
            self.printLine("guard  first", self.guard_info_lines[0])
            self.printLine("guard middle", self.guard_info_lines[1])
            self.printLine("guard   last", self.guard_info_lines[2])
        
        for [begin, end] in self.func_paras:
            self.printLine("consturction para  first", begin)
            i = begin + 1
            while i < end:
                self.printLine("construction para middle", i)
                i += 1
                
            self.printLine("construction para   last", end)
        
        for i in self.last_blank_lines:
            self.printLine("last blank line", i)
        
    
    def printLine(self, msg, index):
        log.debug(msg+" : LineIndex=%d : %s" %(index, self.lines[index][:-1]))
        
    
    
    def do_auto_line(self):
        for lineid in self.auto_lines:
            self.lines[lineid] = ""
    
    def do_copyright(self):
        if self.first_code_line == -1:
            return
        
        line = self.lines[self.first_code_line]
        temp = line.strip()
        if temp == "":
            self.lines[self.first_code_line] = "\n"
        else:
            self.lines[self.first_code_line] = "\n" + line
        

    def do_construction_paras(self):
        if self.func_paras == []:
            return
        
        
        for [begin, end] in self.func_paras:
            
            line = self.lines[begin]
            i1 = line.find(":")
            line = line.rstrip()[:-1] + "\n"
            spacebefore = line[:i1]
            self.lines[begin] = line
            
            temp = begin + 1    
            while temp <= end: 
                line = self.lines[temp]
                line = line.replace(",", "")
                line = spacebefore + ", " + line.lstrip()
                self.lines[temp] = line
                temp += 1
                
    
    def do_guard_info(self):
        if self.header_guard_info == None:
            return
        if self.guard_info_lines[0] == -1:
            return
        
        log.debug("header guard info: %s" %(self.header_guard_info))
        temp = self.guard_info_lines[0]
        self.lines[temp] = FileFormat.IF_N_DEF_KEYWORD + " " + self.header_guard_info + "\n"
        
        temp = self.guard_info_lines[1]
        self.lines[temp] = FileFormat.DEFINE_KEYWORD + " " + self.header_guard_info + "\n"
        
        temp = self.guard_info_lines[2]
        self.lines[temp] = FileFormat.END_IF_KEYWORD + "\n"
        
        
        
        
        
        
        
    def do_last_blank_line(self):
        #MUST from the LAST
        for lineid in self.last_blank_lines:
            #self.lines.pop(lineid)
            self.lines[lineid] = "" 
        


    def write(self):
        f = open(self.filepath, "w")
        
        for line in self.lines:
            f.write(line)
        
        f.close()    

if __name__ == "__main__":
    mgr = FormatMgr()
    mgr.run()
    
