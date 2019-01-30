@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

rem ===========================================================================
set releasepath=..\9.Template\2.Client
set localpath=.

rd /Q /S %releasepath%
if not exist %releasepath% (mkdir %releasepath% )

rem ===========================================================================
