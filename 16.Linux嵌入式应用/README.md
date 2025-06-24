# 专栏笔记总结大全







#### **启动Docker服务**

---

### **2. 使用 `docker` 命令**


#### **检查 Docker 是否运行**

---

### **3. 检查 Docker 进程**
通过 `ps` 命令查看 Docker 进程是否在运行。

#### **查看 Docker 进程**
```bash
ps aux | grep docker
```

**输出示例：**
- **Docker 已启动**：
  ```
  root      12345  0.0  0.1 123456 7890 ?        Ssl  10:00   0:00 /usr/bin/dockerd -H fd:// --containerd=/run/containerd/containerd.sock
  ```
- **Docker 未启动**：
  没有与 Docker 相关的进程。

---

### **4. 检查 Docker 套接字**
Docker 通过 Unix 套接字（`/var/run/docker.sock`）与客户端通信。检查该文件是否存在。

#### **检查套接字文件**
```bash
ls -l /var/run/docker.sock
```

**输出示例：**
- **Docker 已启动**：
  ```
  srw-rw---- 1 root docker 0 Jan  1 10:00 /var/run/docker.sock
  ```
- **Docker 未启动**：
  ```
  ls: cannot access '/var/run/docker.sock': No such file or directory
  ```

---

### **5. 使用 `service` 命令（适用于非 systemd 系统）**
在一些较旧的 Linux 发行版中，可能使用 `service` 命令管理服务。

#### **检查 Docker 状态**
```bash
sudo service docker status
```

**输出示例：**
- **Docker 已启动**：
  ```
  docker is running
  ```
- **Docker 未启动**：
  ```
  docker is stopped
  ```

#### **启动 Docker 服务**
```bash
sudo service docker start
```

---

### **6. 检查 Docker 日志**
如果 Docker 未启动，可以查看日志以排查问题。

#### **查看 Docker 日志**
```bash
sudo journalctl -u docker.service
```

---

### **7. 总结**
通过以下方法可以检查 Docker 是否启动：
1. **`systemctl status docker`**：查看 Docker 服务状态。
2. **`docker info`**：直接运行 Docker 命令。
3. **`ps aux | grep docker`**：检查 Docker 进程。
4. **`ls -l /var/run/docker.sock`**：检查 Docker 套接字文件。
5. **`service docker status`**：适用于非 systemd 系统。
6. **`journalctl -u docker.service`**：查看 Docker 日志。

如果 Docker 未启动，可以使用 `sudo systemctl start docker` 或 `sudo service docker start` 启动它。









