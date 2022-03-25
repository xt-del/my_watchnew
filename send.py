# -*- coding: utf-8 -*-
import socket
import time
import json
import os
import ast
import struct

# fc文件转换成字典
def flTodic(flPath):
    data = open(flPath, 'r')
    res = data.read()
    data.close()
    result_dic = ast.literal_eval(res)
    return result_dic

# fc帧文件转换成数组
def flH2xToList(flvalue):
    r2es = flvalue.strip('[')
    r3es = r2es.strip(']')
    arr = r3es.split(',')
    a2rr = list(map(int, arr))
    return a2rr


if __name__ == '__main__':
      client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      PORT = 8000
      numList = {}
      fcdata = os.listdir('fc')
      for num in range(len(fcdata)):
            numList.update({str(num): fcdata[num]})
            print(f'序号：{str(num)}>> {numList[str(num)]}')
      inp = input('请选择想要发送的文件 输入序号按回车即可>>>')
      print(f'您选择了序号为 {inp} - {numList[str(inp)]}')
      res = flTodic(f'fc/{numList[str(inp)]}')  # 读取.fc文件
      fc_len = int(res['fc_len'])  # 获取总帧数
      print(f'文件长度：{fc_len}')
      for dat in range(fc_len):
          imageCode = flH2xToList(res[str(dat)])
          d2ata = struct.pack("%dB" % (len(imageCode)), *imageCode)
          start = time.time()  # 获取当前时间
          server_address = ("192.168.0.109", PORT)  # 接收方 服务器的ip地址和端口号
          client_socket.sendto(d2ata, server_address)  # 将msg内容发送给指定接收方
          now = time.time()  # 获取当前时间
          run_time = now - start  # 计算时间差，即运行时间
          time.sleep(0.03)