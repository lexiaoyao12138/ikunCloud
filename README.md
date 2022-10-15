# ikunCloud
621 first project</br>
<img src="https://github.com/lexiaoyao12138/ikunCloud/blob/main/config/png/621.jpg?raw=true" width="400px" alt="大帅逼">

## 项目依赖
- cJSON 开源库
address: [cJSON](https://github.com/DaveGamble/cJSON)
### 使用步骤
```shell
mkdir build
cd build && cmake ..
make
sudo make install
```
### 编译
```shell
$ sudo cp /usr/local/lib/libcjson* /lib
$ ls 
main.c 
$ gcc main.c -o main -lcjson
```

###client.c编译
gcc client.c client_head.c ../server/transfer.c ../server/user_fun.c -o client -lcrypt

###执行时
sudo ./client
sudo ./server
