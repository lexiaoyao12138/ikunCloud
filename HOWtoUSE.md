###client.c编译
gcc client.c client_head.c ../server/transfer.c ../server/user_fun.c -o client -lcrypt

###执行时
sudo ./client
sudo ./server

####支持命令
cd ./ | ../ | /
ls    | /
rm ./ | /
mkdir
get filename
put filename
