@echo off
for %%f in (data/*.jpg) do if not exist label/%%~nf.txt (
    type nul > label/%%~nf.txt
)