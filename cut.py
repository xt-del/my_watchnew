import os
import json
import shutil
import cv2
from PIL import Image
import time

# 创建临时文件夹
def createTempFolder():
    create_path = ['temp1', 'temp2']
    for cpath in create_path:
        if not os.path.exists(cpath):
            os.makedirs(cpath)


# 删除临时文件夹
def removeTempFolder():
    remove_path = ['temp1', 'temp2']
    for rpath in remove_path:
        shutil.rmtree(rpath)

# 读取配置文件，返回配置字典对象
def readConfiguration():
    with open("config.json", 'r') as load_f:
        load_dict = json.load(load_f)
        return load_dict

# 数据转换
def transformation(path,width,height):
    image = Image.open(path)
    frame_pix = image.convert('1')
    cdata = b''
    num = 0
    dta = ''
    print("start")
    for y in range(height):
        for x in range(width):
            modes = frame_pix.getpixel((x,y))
            if modes==255:
                cdata += b'1'
            else:
                cdata += b'0'
            num+=1
            if num==8:
                dta += str(int(cdata,2))
                dta +=','
                num = 0
                cdata = b''
    dta = dta[:-1]
    return dta


if __name__ == '__main__':
    numList = {}
    fcdata = os.listdir('video')
    for num in range(len(fcdata)):
        numList.update({str(num): fcdata[num]})
        print(f'序号：{str(num)}>> {numList[str(num)]}')
    inp = input('请选择想要裁剪的文件 输入序号按回车即可>>>')
    print(f'您选择了序号为 {inp} - {numList[str(inp)]}')
    print(numList[str(inp)])
    createTempFolder()
    time.sleep(1)
    frame_count = 0
    config = readConfiguration()
    capture = cv2.VideoCapture(f'video/{numList[str(inp)]}')
    while True:
        ret, frame = capture.read()
        if ret:
            resize_frame = cv2.resize(frame, (int(config['frame_width']), int(config['frame_height'])),interpolation=cv2.INTER_AREA)
            cv2.imwrite("./temp1/%d.bmp" % frame_count, resize_frame)
            frame_count += 1
        else:
            break
    capture.release()
    d3ta = {}
    print(frame_count)
    d3ta.update({'fc_len': str(frame_count)})  # 获取所有帧数添加到字典
    for dat4 in range(frame_count):  # 开始循环
        res = transformation(f"{os.getcwd()}/temp1/{dat4}.bmp",128,64)
        d3ta.update({str(dat4): str(res)})  # 每一帧的字码存储到字典
        print("OK")
    d2ta = open(f'fc/{numList[str(inp)][:-4]}.fc', 'w')  # 打开文件
    d2ta.write(str(d3ta))  # 写入文件
    d2ta.close()
    removeTempFolder()