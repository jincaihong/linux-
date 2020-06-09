#ifndef _PUBLIC_H_
#define _PUBLIC_H_

typedef struct PackHead// 包头
{
	int func_num;//功能号
						//心跳包-1001，登录包-1002 注册包-1003 忘记密码包-1004
						//视频频道应答包-2001，视频分类应答包-2002，视频地区应答包-2003，视频列表应答包-2004
						//上传视频点播请求包-3001，上传视频点播时长请求包-3002，上传应答包-3001（视频点播）-3002（视频点播时长）
						//播放历史请求包-4001

	int pack_size;//包体总长度
	int counts;    //包体数据个数
	int des_fd;		//目标客户端fd（服务器用到）
}PackHead_t;

typedef struct PackTail//包尾,用来验证数据包的完整性
{
	int pack_tail;//设置为55
}PackTail_t;

typedef struct Login// 登录请求包
{
	int user_id;
	char user_name[10];// 登录用户名
	char passwd[10];//密码
}Login_t;

typedef struct LoginRet// 登录应答包
{
	int user_id;//用户ID
	int login_ret;	//登录结果： 0-登录成功，1-登录失败
}LoginRet_t;

typedef struct Register// 注册请求包
{
	char user_name[10];// 注册用户名
	char passwd[10];//注册密码
	char passwd2[10];//二次密码
}Register_t;

typedef struct RegisterRet// 注册应答包
{
	int user_id;//用户ID
	int Register_ret;	//注册结果： 0-注册成功，1-注册失败
}RegisterRet_t;

typedef struct Forget// 忘记密码请求包
{
	char user_name[10];// 登录用户名
	char passwd[10];//新密码
	char passwd2[10];//二次密码
}Forget_t;

typedef struct ForgetRet// 忘记密码应答包
{
	int user_id;//用户ID
	int Register_ret;	//修改密码结果： 0-修改成功，1-修改失败
}ForgetRet_t;

typedef struct VideoChannel//视频频道应答包
{
	int channel_id; //频道ID
	char channel_name[10];//频道名称
}VideoChannel_t;

typedef struct VideoType//视频分类应答包
{
	int type_id;//分类ID
	int channel_id;//频道ID
	char type_name[10];//分类名称
}VideoType_t;

typedef struct VideoArea//视频地区应答包
{
	int area_id;//地区ID
	int channel_id;//频道ID
	char area_name[10];//地区名称
}VideoArea_t;

typedef struct selectVideoList//选择分类后视频列请求包
{
	int channel_id;//频道ID
	int sort_id;//排序ID
	int type_id;//分类ID
	int area_id;//地区ID
	int time_id;//年份id
}selectVideoList_t;

typedef struct VideoList//视频列表应答包
{
	int video_id;//视频ID
	int channel_id;//频道ID
	int type_id;//分类ID
	int area_id;//地区ID
	char video_name[18];//视频名称
	int play_times;//播放次数
	char play_locad[70];
}VideoList_t;

typedef struct VideoPlay//上传视频点播请求包
{
	int user_id;//用户ID（没有登录的话为0）
	int video_id;//视频ID
}VideoPlay_t;

typedef struct VideoRecord//上传视频点播时长请求包（没有登录的话不需要请求）
{
	int user_id;//用户ID
	int video_id;//视频ID
	int video_seek;//播放时长
}VideoRecord_t;

typedef struct UpLoad//上传应答包
{
	int results;//上传结果，0-上传成功，1-上传失败
}UpLoad_t;

typedef struct PlayHistorys//播放历史请求4001
{
	int user_id;//用户ID
}Play_t;
typedef struct PlayHistory//播放历史应答包
{
	int video_id;//视频ID
	char video_name[18];//视频名称
	int play_times;//播放次数
	int video_seek;//播放时长
}PlayHistory_t;

typedef struct Search// 搜索视频请求包1005
{
	char vedio_name[30];
}Search_t;

typedef struct SearchRet// 搜索视频应答包1005
{
	int Search_ret;	//修改密码结果： 0-搜索成功，1-搜索失败
}SearchRet_t;
typedef struct ClearHistory//清空播放记录4002
{
	int user_id;//用户ID
}ClearHistory_t;
#endif
