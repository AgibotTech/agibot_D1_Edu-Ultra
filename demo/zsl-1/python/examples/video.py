import cv2
import time
from datetime import datetime

def read_rtsp_stream(rtsp_url, save_video=True, save_path=None):
    # -------------------------- 保存参数配置 --------------------------
    if save_video:
        # 默认保存路径：当前目录 + 时间戳命名（避免文件名冲突）
        if not save_path:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            save_path = f"rtsp_record_{timestamp}.mp4"
        
        # 视频编码器配置（MP4格式推荐使用 H.264 编码器）
        fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # mp4v 是兼容所有播放器的 H.264 编码器
        video_writer = None  # 视频写入器对象（后续根据流分辨率初始化）
        frame_width = 800
        frame_height = 600
        video_writer = cv2.VideoWriter(
                            save_path,
                            fourcc,
                            20,  # 手动指定帧率 default:50
                            (frame_width, frame_height),
                        )

    # -------------------------- 流初始化（原有优化保留） --------------------------
    # 1. 初始化视频捕获，强制使用TCP
    cap = cv2.VideoCapture(rtsp_url + "?tcp")
    
    # 2. 降低缓冲区大小（只保留1帧，减少延迟）
    if hasattr(cv2, 'CAP_PROP_BUFFERSIZE'):
        cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)
    
    # 3. 禁用硬件加速（避免兼容性问题）
    if hasattr(cv2, 'CAP_PROP_HW_ACCELERATION'):
        cap.set(cv2.CAP_PROP_HW_ACCELERATION, cv2.VIDEO_ACCELERATION_NONE)
    
    # 4. 检查流是否打开，失败则重试默认协议
    if not cap.isOpened():
        print(f"TCP协议无法打开流: {rtsp_url}")
        cap = cv2.VideoCapture(rtsp_url)
        if not cap.isOpened():
            print("所有协议均无法打开流，退出")
            return
    print(f"RTSP流已打开：{rtsp_url}")
    if save_video:
        print(f"视频将保存到：{save_path}")

    # -------------------------- 时间戳显示配置（新增） --------------------------
    # 字体：选择支持中文的字体（cv2.FONT_HERSHEY_SIMPLEX 兼容性好）
    font = cv2.FONT_HERSHEY_SIMPLEX
    # 字体大小（根据画面分辨率调整，800x600用1.0合适）
    font_scale = 1.0
    # 字体颜色（白色，BGR格式）
    font_color = (255, 255, 255)
    # 边框颜色（黑色，增加文字对比度，避免与背景融合）
    border_color = (0, 0, 0)
    # 字体厚度（2px，确保清晰）
    font_thickness = 2
    # 边框厚度（4px，突出文字）
    border_thickness = 4
    # 时间戳位置（左上角，x=10, y=30，留出边距）
    text_position = (10, 30)

    # -------------------------- 流读取与保存循环 --------------------------
    try:
        while True:
            # 读取最新帧（跳过旧帧，保留原有低延迟逻辑）
            ret, frame = False, None
            for _ in range(3):  # 连续读取3次，确保拿到最新帧
                ret_temp, frame_temp = cap.read()
                if ret_temp:
                    ret, frame = ret_temp, frame_temp
                else:
                    break
            
            if not ret:
                print("流中断，尝试重连...")
                time.sleep(1)
                # 重连时重新初始化捕获对象
                cap = cv2.VideoCapture(rtsp_url + "?tcp")
                if hasattr(cv2, 'CAP_PROP_BUFFERSIZE'):
                    cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)
                continue

            # 缩小分辨率（保持原有逻辑）
            frame = cv2.resize(frame, (800, 600))
            frame_height, frame_width = frame.shape[:2]

            # -------------------------- 新增：绘制时间戳 --------------------------
            # 获取当前时间（格式：年月日 时:分:秒）
            current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            # 先绘制黑色边框文字（增加对比度，确保在任何背景下都可见）
            cv2.putText(
                img=frame,
                text=current_time,
                org=text_position,
                fontFace=font,
                fontScale=font_scale,
                color=border_color,
                thickness=border_thickness,
                lineType=cv2.LINE_AA  # 抗锯齿，文字更清晰
            )
            # 再绘制白色前景文字（叠加在黑色边框上）
            cv2.putText(
                img=frame,
                text=current_time,
                org=text_position,
                fontFace=font,
                fontScale=font_scale,
                color=font_color,
                thickness=font_thickness,
                lineType=cv2.LINE_AA
            )

            # -------------------------- 视频保存逻辑 --------------------------
            if save_video:
                # 初始化视频写入器（首次获取到帧时初始化，避免分辨率未知问题）
                if video_writer is None:
                    # 获取流的帧率（若获取失败，默认设为30fps）
                    fps = cap.get(cv2.CAP_PROP_FPS) or 30
                    video_writer = cv2.VideoWriter(
                        save_path,
                        fourcc,
                        fps,
                        (frame_width, frame_height)  # 必须与帧分辨率一致
                    )
                    print(f"视频写入器初始化完成：{fps}fps | {frame_width}x{frame_height}")
                
                # 写入当前帧到MP4文件（包含时间戳）
                video_writer.write(frame)

            # -------------------------- 实时预览 --------------------------
            cv2.imshow("RTSP Stream (按q退出)", frame)

            # 检测退出按键（按q退出，确保视频文件正常关闭）
            if cv2.waitKey(1) & 0xFF == ord('q'):
                print("正在退出，保存视频文件...")
                break

    finally:
        # -------------------------- 资源释放 --------------------------
        # 关闭视频写入器（关键！否则文件会损坏）
        if save_video and video_writer is not None:
            video_writer.release()
            print(f"视频已保存：{save_path}")
        # 关闭流捕获和窗口
        cap.release()
        cv2.destroyAllWindows()
        print("所有资源已释放")

if __name__ == "__main__":
    # 配置参数
    RTSP_URL = "rtsp://192.168.234.1:8554/test"  # 机器狗RTSP流地址
    SAVE_VIDEO = True  # 是否保存视频（False则只预览不保存）
    SAVE_PATH = None   # 自定义保存路径（如："D:/record.mp4"，None则使用默认路径）
    
    # 启动流读取与保存
    read_rtsp_stream(rtsp_url=RTSP_URL, save_video=SAVE_VIDEO, save_path=SAVE_PATH)
