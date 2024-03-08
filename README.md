# Operating-Systems

1.  make sure there is a git ignore file, if not add one and 
add the following to it:
    .git
    .gitignore
    .gitattributes
    folder/*
    *.zip

2. run the following line to make a zip file as well exclude the folder folder
zip -r clean.zip * -x "folder/*" -x "folder" -x *.zip
3. submit the zip file

4. make sure to delete the zip file after