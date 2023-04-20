copy ..\.\bin\Debug-windows-x86_64\DEL-10\*.exe . /Y
for /f %%a in ('dir /b *.exe') do start "" "C:\Program Files\RenderDoc\qrenderdoc.exe" "%%a"