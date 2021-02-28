package com.iflytek.msp.lfasr;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
//import java.io.FileOutputStream;
import java.io.FileWriter;
import java.util.HashMap;

import org.apache.log4j.PropertyConfigurator;

import com.alibaba.fastjson.JSON;
import com.iflytek.msp.cpdb.lfasr.client.LfasrClientImp;
import com.iflytek.msp.cpdb.lfasr.exception.LfasrException;
import com.iflytek.msp.cpdb.lfasr.model.LfasrType;
import com.iflytek.msp.cpdb.lfasr.model.Message;
import com.iflytek.msp.cpdb.lfasr.model.ProgressStatus;

public class TestLfasr 
{
	// 原始音频存放地址
	private static final String local_file = "C:\\Users\\Jordan23\\Desktop\\DataSource\\新闻联播\\新闻联播20180410\\逐帧-新闻联播20180410-005.wav";
	/*
	 * 转写类型选择：标准版和电话版分别为：
	 * LfasrType.LFASR_STANDARD_RECORDED_AUDIO 和 LfasrType.LFASR_TELEPHONY_RECORDED_AUDIO
	 * */
	private static final LfasrType type = LfasrType.LFASR_STANDARD_RECORDED_AUDIO;
	// 等待时长（秒）
	private static int sleepSecond = 20;
	
	public static void main(String[] args) {
		// 加载配置文件
		PropertyConfigurator.configure("log4j.properties");
		
		// 初始化LFASR实例
		LfasrClientImp lc = null;
		try {
			lc = LfasrClientImp.initLfasrClient();
		} catch (LfasrException e) {
			// 初始化异常，解析异常描述信息
			Message initMsg = JSON.parseObject(e.getMessage(), Message.class);
			System.out.println("ecode=" + initMsg.getErr_no());
			System.out.println("failed=" + initMsg.getFailed());
		}
				
		// 获取上传任务ID
		String task_id = "";
		HashMap<String, String> params = new HashMap<>();
		params.put("has_participle", "true");
		try {
			// 上传音频文件
			Message uploadMsg = lc.lfasrUpload(local_file, type, params);
			
			// 判断返回值
			int ok = uploadMsg.getOk();
			if (ok == 0) {
				// 创建任务成功
				task_id = uploadMsg.getData();
				System.out.println("task_id=" + task_id);
			} else {
				// 创建任务失败-服务端异常
				System.out.println("ecode=" + uploadMsg.getErr_no());
				System.out.println("failed=" + uploadMsg.getFailed());
			}
		} catch (LfasrException e) {
			// 上传异常，解析异常描述信息
			Message uploadMsg = JSON.parseObject(e.getMessage(), Message.class);
			System.out.println("ecode=" + uploadMsg.getErr_no());
			System.out.println("failed=" + uploadMsg.getFailed());					
		}
				
		// 循环等待音频处理结果
		while (true) {
			try {
				// 睡眠1min。另外一个方案是让用户尝试多次获取，第一次假设等1分钟，获取成功后break；失败的话增加到2分钟再获取，获取成功后break；再失败的话加到4分钟；8分钟；……
				Thread.sleep(sleepSecond * 1000);
				System.out.println("waiting ...");
			} catch (InterruptedException e) {
			}
			try {
				// 获取处理进度
				Message progressMsg = lc.lfasrGetProgress(task_id);
						
				// 如果返回状态不等于0，则任务失败
				if (progressMsg.getOk() != 0) {
					System.out.println("task was fail. task_id:" + task_id);
					System.out.println("ecode=" + progressMsg.getErr_no());
					System.out.println("failed=" + progressMsg.getFailed());
					
					// 服务端处理异常-服务端内部有重试机制（不排查极端无法恢复的任务）
					// 客户端可根据实际情况选择：
					// 1. 客户端循环重试获取进度
					// 2. 退出程序，反馈问题
					continue;
				} else {
					ProgressStatus progressStatus = JSON.parseObject(progressMsg.getData(), ProgressStatus.class);
					if (progressStatus.getStatus() == 9) {
						// 处理完成
						System.out.println("task was completed. task_id:" + task_id);
						break;	
					} else {
						// 未处理完成
						System.out.println("task was incomplete. task_id:" + task_id + ", status:" + progressStatus.getDesc());
						continue;
					}
				}
			} catch (LfasrException e) {
				// 获取进度异常处理，根据返回信息排查问题后，再次进行获取
				Message progressMsg = JSON.parseObject(e.getMessage(), Message.class);
				System.out.println("ecode=" + progressMsg.getErr_no());
				System.out.println("failed=" + progressMsg.getFailed());
			}
		}

		// 获取任务结果
		try {
			Message resultMsg = lc.lfasrGetResult(task_id);
			//System.out.println(resultMsg.getData());
			// 如果返回状态等于0，则任务处理成功
			if (resultMsg.getOk() == 0) {
				// 打印转写结果
				System.out.println(resultMsg.getData());
				
				// 将获得的语音转写数据写入文件
				//FileOutputStream fop = null;
				try {
					File file = new File("C:\\Users\\Jordan23\\Desktop\\DataSource\\新闻联播\\新闻联播20180410\\逐帧-新闻联播20180410-005.json");
					//fop = new FileOutputStream(file);
					
					if(!file.exists()) {
						file.createNewFile();
					}
					FileWriter fw = new FileWriter(file);
					BufferedWriter out = new BufferedWriter(fw);
					out.write(resultMsg.getData());
					out.close();
					System.out.println("Done!");
					//fop.write(resultMsg.getData().getBytes());// 会报错，只能是字节流
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} else {
				// 转写失败，根据失败信息进行处理
				System.out.println("ecode=" + resultMsg.getErr_no());
				System.out.println("failed=" + resultMsg.getFailed());
			}
		} catch (LfasrException e) {
			// 获取结果异常处理，解析异常描述信息
			Message resultMsg = JSON.parseObject(e.getMessage(), Message.class);
			System.out.println("ecode=" + resultMsg.getErr_no());
			System.out.println("failed=" + resultMsg.getFailed());
		}
	}
}
