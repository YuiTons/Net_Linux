Net_Linux

Setup



…or create a new repository on the command line

echo "# Net_Linux" >> README.md

git init

git add README.md

git commit -m "first commit"

git remote add origin https://github.com/YuiTons/Net_Linux.git

git push -u origin master


…or push an existing repository from the command line

git remote add origin https://github.com/YuiTons/Net_Linux.git

git push -u origin master

------------------------------------------------------------------------

Setup tokyo-metro.vim

copy /usr/share/vim/vim80/colors

vi /etc/vim/vimrc

add :colorscheme tokyo-metro
