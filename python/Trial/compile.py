#!/usr/bin/dev python
# coding: utf-8

'''
Created on 2014年3月1日

@author: huaiyu
'''

import py_compile as pc
import compileall as ca

def main():
    #pc.compile('./Trial.py')
    ca.compile_dir('./', 0, force=True)
    print 'end compile'
    pass

if __name__ == '__main__':
    main()