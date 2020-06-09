BEGIN TRANSACTION;

CREATE TABLE Tbl_user   --客户端用户表
 (
   user_id numeric(6) PRIMARY KEY,  --用户ID
   user_name varchar(10) NOT NULL,	--用户登陆名
   user_pwd varchar(10)				--用户密码
 );   
INSERT INTO "Tbl_user" VALUES(1,'admin','123456');
INSERT INTO "Tbl_user" VALUES(2,'usera','123456');
INSERT INTO "Tbl_user" VALUES(3,'userb','123456');
INSERT INTO "Tbl_user" VALUES(4,'runshan','123456');

 CREATE TABLE Tbl_user_play   --用户播放明细表
 (
   play_id numeric(6) PRIMARY KEY, --明细ID
   user_id numeric(6),   			--用户ID
   video_id numeric(6),				--视频ID
   play_times integer,			   --播放时长
   last_play_time text			   --最后播放时间
 );                                       
INSERT INTO "Tbl_user_play" VALUES(1,1,1005,45,'');
INSERT INTO "Tbl_user_play" VALUES(2,1,1015,90,'');
INSERT INTO "Tbl_user_play" VALUES(2,1,1012,120,'');
INSERT INTO "Tbl_user_play" VALUES(3,2,1003,69,'');
INSERT INTO "Tbl_user_play" VALUES(4,2,1008,74,'');
INSERT INTO "Tbl_user_play" VALUES(2,3,1017,56,'');
INSERT INTO "Tbl_user_play" VALUES(2,3,1014,88,'');

 CREATE TABLE Tbl_video_message  --视频信息表
(
  video_id numeric(6) PRIMARY KEY,--视频ID
  channel_id numeric(6),		  --频道ID
  area_id numeric(6),			  --地区ID
  type_id numeric(6),	          --类型ID
  video_name varchar(20),		  --视频名称
  play_counts integer	          --播放次数
 );
 --电影频道
 INSERT INTO "Tbl_video_message" VALUES(1000,1,1,1,'英雄',100);
 INSERT INTO "Tbl_video_message" VALUES(1001,1,1,2,'大话西游',100);
 INSERT INTO "Tbl_video_message" VALUES(1002,1,1,3,'赤壁',100);
 INSERT INTO "Tbl_video_message" VALUES(1003,1,2,1,'叶问',100);
 INSERT INTO "Tbl_video_message" VALUES(1004,1,2,2,'国产凌凌漆',100);
 INSERT INTO "Tbl_video_message" VALUES(1005,1,2,3,'墨攻',100);
 INSERT INTO "Tbl_video_message" VALUES(1006,1,3,1,'猛龙过江',100);
 INSERT INTO "Tbl_video_message" VALUES(1007,1,3,2,'阿甘正传',100);
 INSERT INTO "Tbl_video_message" VALUES(1008,1,3,3,'生死狙击',100);
 --电视剧频道
 INSERT INTO "Tbl_video_message" VALUES(1009,2,4,4,'奋斗',100);
 INSERT INTO "Tbl_video_message" VALUES(1010,2,4,5,'白发魔女传',100);
 INSERT INTO "Tbl_video_message" VALUES(1011,2,4,6,'步步惊心',100);
 INSERT INTO "Tbl_video_message" VALUES(1012,2,5,4,'假如生活欺骗',100);
 INSERT INTO "Tbl_video_message" VALUES(1013,2,5,5,'小李飞刀',100);
 INSERT INTO "Tbl_video_message" VALUES(1014,2,5,6,'神话',100);
 INSERT INTO "Tbl_video_message" VALUES(1015,2,6,4,'梁祝',100);
 INSERT INTO "Tbl_video_message" VALUES(1016,2,6,5,'神雕侠侣',100);
 INSERT INTO "Tbl_video_message" VALUES(1017,2,6,6,'四大才子',100);

 CREATE TABLE Tbl_video_area  --视频地区表
(
  area_id numeric(6) PRIMARY KEY,--地区ID
  channel_id numeric(6),		--频道ID
  area_name text,				--地区名称
  area_remark text				--备注
 );
 INSERT INTO "Tbl_video_area" VALUES(1,1,'大陆','');
 INSERT INTO "Tbl_video_area" VALUES(2,1,'香港','');
 INSERT INTO "Tbl_video_area" VALUES(3,1,'欧美','');
 INSERT INTO "Tbl_video_area" VALUES(4,2,'大陆','');
 INSERT INTO "Tbl_video_area" VALUES(5,2,'台湾','');
 INSERT INTO "Tbl_video_area" VALUES(6,2,'香港','');

 CREATE TABLE Tbl_video_channel  --视频频道表
(
  channel_id numeric(6) PRIMARY KEY,--频道ID
  channel_name text					--频道名称
 );
 INSERT INTO "Tbl_video_channel" VALUES(1,'电影');
 INSERT INTO "Tbl_video_channel" VALUES(2,'电视剧');
 
 CREATE TABLE Tbl_video_type  --视频分类表
(
  type_id numeric(6) PRIMARY KEY,--分类ID
  channel_id numeric(6),		--频道ID
  type_name text				--分类名称
 );
 INSERT INTO "Tbl_video_type" VALUES(1,1,'动作');
 INSERT INTO "Tbl_video_type" VALUES(2,1,'喜剧');
 INSERT INTO "Tbl_video_type" VALUES(3,1,'战争');
 INSERT INTO "Tbl_video_type" VALUES(4,2,'爱情');
 INSERT INTO "Tbl_video_type" VALUES(5,2,'武侠');
 INSERT INTO "Tbl_video_type" VALUES(6,2,'历史');
 
COMMIT;
