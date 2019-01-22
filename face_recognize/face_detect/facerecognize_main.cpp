/*
 * facerecognize_main.cpp
 *
 *  Created on: Oct 31, 2018
 *      Author: cui
 */

#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <opencv2/opencv.hpp>
#include <langinfo.h>
#include <locale.h>
#include <jsoncpp/json/json.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <fstream>
#include <deque>
#include <list>

#include "httpclient.h"
#include "mysqlclient.h"
#include "arcfacev2.h"
#include "hik_device.h"
#include "deque_typedef.h"
#include "sdl2.h"
#include "joinus.h"

#define DBG_EN 	1
#if (DBG_EN == 1)
#define printf_dbg(...) 	fprintf(stderr, "[main_dbg]( %d): ", __LINE__); fprintf(stderr, __VA_ARGS__)
#else
#define printf_dbg(...)
#endif

#define printf_info(...) 	fprintf(stderr, "[main_info](%s, %s(), %d): ", __FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__)
#define printf_warn(...) 	fprintf(stderr, "[main_warn](%s, %s(), %d): ", __FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__)
#define printf_err(...)  	fprintf(stderr, "[main_err](xxx%s, %s(), %d): ", __FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__)

struct thread_arg{
	pthread_t sub_thread1;
    int test;
};

typedef struct{
    std::string hk_ip;
    int    in_out;
}hkdev_inout;

bool create_fd_fr_process1_thread(struct thread_arg *arg );
void ctrl_c_exit( int signo );
void save_imagedata2pic();
void hik_deque_free();
void deal_imgs_same_faceid( face_idx_score  &one_id, string &img_name );
void tmp_record_assign_value(db_swipe_info &db_info,swipe_info_str &swipe_info);
void parse_js();
void set_local_unicode();
void mysql_error_to_exit();
void arcsoft_error_to_exit();
bool update_student_message_face_feature_everyday();

bool main_exit = false;
bool fd_fr_exit = false;

httpclient joinus_school;
arcfacev2 fdb_ins;
hik_device  dev_yanfa;
vector<string> src_stu_num_vec;
list<hik_unit>  hikvs_face_list;              	//用于主线程和回调函数间保存数据到图片
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

deque<name_faceid_unit>     faceid_name_deque;		//用于主线程和FDFT线程之间传递图片路径名称
deque<cache_table_info>     transfer_cache_deque;
deque<pic_download_info>     global_pics_deque;

std::string face_pic_name;
std::string bg_pic_name;

std::string facelib_path;

std::string mysql_host;
std::string mysql_user;
std::string mysql_password;
std::string db_name;

std::string appid;
std::string sdkkey;

int face_ensure_threshold;
int pic_show_threshold;

http_custom_info server_info;
interface_file   server_files;

std::vector<struct hk_args> hk_args_vec;
std::vector<hkdev_inout>    inout_vec;

MYSQL       *pface_sql = NULL;

int main( int argc, char *argv[] )
{
    set_local_unicode();
    parse_js();
	signal(SIGINT, ctrl_c_exit);

    joinus_school.http_init( server_info, server_files );

    int ret;
    if(!mysql_db_init_library()){
        joinus_school.http_uninit();
        exit(-1);
    }

    pface_sql = mysql_db_connect( mysql_host.c_str(), mysql_user.c_str(), \
                                  mysql_password.c_str(), NULL, &ret );
    if( (ret < 0) || (NULL == pface_sql) )
        mysql_error_to_exit();
    mysql_db_print_cinfo();

    ret = mysql_db_create_db(pface_sql,db_name.c_str());
    if( ret < 0 )
        mysql_error_to_exit();

    std::vector<std::string> query_result;
    mysql_db_query(pface_sql, "SELECT VERSION(), current_timestamp()", query_result );
    if (mysql_select_db(pface_sql,db_name.c_str()) != 0)
        mysql_error_to_exit();
    ClearVector(query_result);

    mysql_db_show_tables(pface_sql, db_name.c_str());

    if( !joinus_school.post_school_message() ){
        printf_err( "httpclient error, so exit............\n" );
        mysql_error_to_exit();
    }

    struct thread_arg th_args;
    th_args.test = 123;
    th_args.sub_thread1 = 0;
    void* retval_1;

    if( !fdb_ins.engine_init(appid, sdkkey) ){
        mysql_error_to_exit();
    }
    fdb_ins.print_engine_version();


//    joinus_school.post_all_students_info();
    if( !update_student_message_face_feature_everyday() )
        mysql_error_to_exit();

    joinus_school.post_heart_message();

    if( !dev_yanfa.hik_dev_login(hk_args_vec) ){
        printf_err( "Hikvison device login error\n" );
        goto ERROR_EXIT;
    }
    printf_info( "Hikvison device login success\n" );
    if( 0 != dev_yanfa.hik_set_callback_and_alarm() ){
        printf_err( "Hikvison set_callback_and_alarm error\n" );
        goto ERROR_EXIT;
    }
    printf_info( "Hikvison device set_callback_and_alarm success\n" );

	if( !create_fd_fr_process1_thread( &th_args ) ){
		printf_err( "Create fd_fr process1 failed\n" );
		exit(1);
	}
	printf_dbg( "Create fd_fr_thread success\n" );

    while(1){
        if( main_exit )break;
        pthread_mutex_lock( &list_mutex );
        save_imagedata2pic();
        pthread_mutex_unlock(&list_mutex );
    }

//	printf_dbg( "Create socket thread success\n" );
ERROR_EXIT:
    std::cout << "waiting for subthred1" << std::endl;
	ctrl_c_exit( 0 );
    sleep_us(2000000);
	pthread_join( th_args.sub_thread1, &retval_1 );
//	printf_info( "%d thread exit\n", (int*)retval_1 );

    hik_deque_free();
    fdb_ins.engine_uninit();

    mysql_db_disconnect(pface_sql);
    mysql_library_end();
    printf_dbg( "Close database:%s and mysql_library end\n", db_name.c_str() );

    dev_yanfa.hik_dev_logout();

    joinus_school.http_uninit();
    sdl_show_uninit();
	exit(0);
}


void *fd_fr_process1_thread(void * _args )
{

    printf_dbg( "fd_fr pthread_id: %lu \n", (unsigned long)pthread_self() );
    struct thread_arg *args = (struct thread_arg *)_args;
    printf_dbg( "thread args test: %d\n", (int)args->test );

    name_faceid_unit  temp_unit;
    face_idx_score dst_result2mysql;

    std::string prit_time;
    std::string cache_table = "Transfer_Cache";
    std::vector<std::string> db_result2_vec;
	int ret;
	struct tm   time_tm_daily;
    std::string  daily_table_name = mysql_db_get_daily_table_name( time_tm_daily );
	mysql_db_create_daily_table(pface_sql, daily_table_name, db_result2_vec );

    std::string daily_log = daily_table_name;
	daily_log.append("log");
	mysql_db_create_daily_table(pface_sql, daily_log, db_result2_vec );
    float picshow_threahold = pic_show_threshold*1.0 / 100.0;
    float ensure_threahold = face_ensure_threshold*1.0 / 100.0;
    if( ensure_threahold < 0.25 || ensure_threahold > 1) ensure_threahold = 0.3;
    std::cout << "ensure_threahold:" << ensure_threahold << "\tface_ensure_threshold:"
              << face_ensure_threshold*1.0 << std::endl;
    std::cout << "picshow_threahold: " << picshow_threahold << endl;

    std::string query_stuid;
    float tmp_score;
    int   tmp_inout;
    db_swipe_info record_info;
    swipe_info_str tmp_record;

    sdl_show_init();

    for(;;){
        if( fd_fr_exit ){
            break;
        }

        sdl_update();
        if( mysql_db_check_time2build_table(time_tm_daily) ){
            daily_table_name = mysql_db_get_daily_table_name( time_tm_daily );
			mysql_db_create_daily_table(pface_sql, daily_table_name, db_result2_vec );

			daily_log = daily_table_name;
			daily_log.append( "log" );
			mysql_db_create_daily_table(pface_sql, daily_log, db_result2_vec );

            update_student_message_face_feature_everyday();
		}

        if( faceid_name_deque.empty() )
            continue;
        sdl_text_info result_info, log_info;
        std::string  filename1, filename2;
        if( faceid_name_deque.size() >= 1 ){
        	temp_unit = faceid_name_deque.back();
			deal_imgs_same_faceid( dst_result2mysql, temp_unit.img_name );
            filename1 = temp_unit.img_name;
			faceid_name_deque.pop_back();
            query_stuid = "";
            query_stuid = dst_result2mysql.name;
            tmp_inout = temp_unit.in_out;
            if( query_stuid.empty() ) continue;
            tmp_score = dst_result2mysql.score;
            mysql_db_table_score_update( pface_sql, cache_table, query_stuid, tmp_score );    //更新cache分数

			prit_time = mysql_db_get_timestring();
            mysql_db_insert_daily_from_cache( pface_sql, daily_log, query_stuid,
                                              tmp_score, tmp_inout, log_info );     //插入到log表格
            printf_dbg( "\n\tRecognize Picture: %s \n\tResult stuid: %s\n\tScore:%f\n",
                        temp_unit.img_name.c_str(), query_stuid.c_str(), tmp_score );

            ret = mysql_db_transfer_check( pface_sql,query_stuid );
            if( ret == 0 && tmp_score >= ensure_threahold ){          //如果和上次同条记录间隔大于120s,且相似度大于阈值
				printf_info( "%s Transfered %s(similar:%f) to dialy_table\n", \
                             prit_time.c_str(), query_stuid.c_str(), tmp_score );
                mysql_db_table_time_update( pface_sql, cache_table, query_stuid );
                mysql_db_insert_daily_from_cache( pface_sql, daily_table_name,
                                                  query_stuid, tmp_score, tmp_inout, result_info );//插入到待发送表格

                if( get_swipe_record_from_db( pface_sql, daily_table_name, record_info ) ){
                    tmp_record_assign_value(record_info, tmp_record );

                    if( joinus_school.post_swipe_message(tmp_record) ){
                        printf_dbg( "Send swipe record success: \n\tcard_num: %s\n\ttime:%s\n\tin_out:%d\n",
                                    tmp_record.card_code.c_str(), tmp_record.swipe_time.c_str(), tmp_record.in_out);
                        mysql_synchronization_transfer(pface_sql, daily_table_name, stoi(record_info.uni_id));
                    }
                }
                std::cout << std::endl;

            }

            if( fdb_ins.get_name_from_index( dst_result2mysql.name_index, filename2 ) ){
                std::cout << "\tname: " << log_info.name << "\n\tclass: " << log_info.class_id
                     << "\n\ttime: " << log_info.timestr << std::endl;
                if( tmp_score <= picshow_threahold ) {
                    filename2 = facelib_path + "/no_people.jpg";
                    log_info.name.clear();
                    log_info.class_id.clear();
                }
                sdl_show_by_filename(filename1, filename2,
                                      log_info.name, log_info.class_id, log_info.score, log_info.timestr );
             }
        }
    }

    printf_info( "fd_fr --- exit\n" );
    pthread_exit((void*)1);
}


bool create_fd_fr_process1_thread( struct thread_arg *arg )
{
    pthread_t fr_process1_tid;
    pthread_attr_t  fr_process1_attr;

    pthread_attr_init(&fr_process1_attr);
    pthread_attr_setdetachstate(&fr_process1_attr, PTHREAD_CREATE_JOINABLE);        //属性为结PTHREAD_CREATE_DETACHED合
    if( pthread_create(&fr_process1_tid, &fr_process1_attr, fd_fr_process1_thread, (void *)(arg) ) != 0) {
        printf_err("create face recognize process1 thread FAIL!!!\n");
        return false;
    }
    arg->sub_thread1 = fr_process1_tid;
    pthread_attr_destroy(&fr_process1_attr);
    return true;
}

void ctrl_c_exit( int signo )
{
    main_exit = true;
    fd_fr_exit = true;
    printf_dbg( "Catch Ctrl+C signal\n" );
}

BOOL CALLBACK MessageCallback(LONG lCommand, NET_DVR_ALARMER *pAlarmer,\
                              char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
    switch(lCommand)
    {
        case COMM_UPLOAD_FACESNAP_RESULT: //人脸抓拍报警信息
        {
            NET_VCA_FACESNAP_RESULT struFaceSnap = {0};
            memcpy(&struFaceSnap, pAlarmInfo, sizeof(NET_VCA_FACESNAP_RESULT));

            NET_DVR_TIME struAbsTime = {0};
            struAbsTime.dwYear = GET_YEAR(struFaceSnap.dwAbsTime);
            struAbsTime.dwMonth = GET_MONTH(struFaceSnap.dwAbsTime);
            struAbsTime.dwDay = GET_DAY(struFaceSnap.dwAbsTime);
            struAbsTime.dwHour = GET_HOUR(struFaceSnap.dwAbsTime);
            struAbsTime.dwMinute = GET_MINUTE(struFaceSnap.dwAbsTime);
            struAbsTime.dwSecond = GET_SECOND(struFaceSnap.dwAbsTime);


            //保存抓拍场景图片 dwFacePicLen
            hik_unit    temp_face_unit;
            if( (struFaceSnap.dwFacePicLen > 0) && (struFaceSnap.pBuffer1 != NULL)\
                    && (struFaceSnap.dwBackgroundPicLen > 0) && (struFaceSnap.pBuffer2 != NULL) )
            {
                char cFilename[256] = {0};
                char cFilename_whole[256] = {0};
                char chTime[128];
                char dev_ip[128];

                sprintf(chTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",struAbsTime.dwYear,struAbsTime.dwMonth,struAbsTime.dwDay,\
                        struAbsTime.dwHour,struAbsTime.dwMinute,struAbsTime.dwSecond);
                sprintf(cFilename, "face%d-%d-%s-%s.jpg",\
                        struFaceSnap.dwFacePicID, struFaceSnap.dwFaceScore, struFaceSnap.struDevInfo.struDevIP.sIpV4, chTime);
                sprintf(dev_ip, "%s", struFaceSnap.struDevInfo.struDevIP.sIpV4);
                std::string ip_judge = dev_ip;
                sprintf(cFilename_whole, "face%d-%d-%s-%swhole.jpg",\
                        struFaceSnap.dwFacePicID, struFaceSnap.dwFaceScore, struFaceSnap.struDevInfo.struDevIP.sIpV4, chTime);

                temp_face_unit.pimage = (unsigned char *)malloc( struFaceSnap.dwFacePicLen );
                temp_face_unit.pwhole_img = (unsigned char *)malloc( struFaceSnap.dwBackgroundPicLen );
                if( (NULL != temp_face_unit.pimage) && (temp_face_unit.pwhole_img) ){
                    pthread_mutex_lock( &list_mutex );

                    memcpy(temp_face_unit.pimage, struFaceSnap.pBuffer1, (ssize_t)struFaceSnap.dwFacePicLen );
                    temp_face_unit.img_len = struFaceSnap.dwFacePicLen;
                    temp_face_unit.face_num = struFaceSnap.dwFacePicID;
                    temp_face_unit.face_score = struFaceSnap.dwFaceScore;
                    temp_face_unit.img_name = cFilename;

                    memcpy(temp_face_unit.pwhole_img, struFaceSnap.pBuffer2, (ssize_t)struFaceSnap.dwBackgroundPicLen );
                    temp_face_unit.whole_img_len = struFaceSnap.dwBackgroundPicLen;
                    temp_face_unit.whole_img_name = cFilename_whole;

                    temp_face_unit.is_read = true;
                    temp_face_unit.is_write2img = false;
                    for( unsigned int i = 0; i < inout_vec.size(); i++ ){
                        if( 0 == ip_judge.compare(inout_vec.at(i).hk_ip))
                            temp_face_unit.in_out =inout_vec.at(i).in_out;
                    }

                    hikvs_face_list.push_front( temp_face_unit );				//插入到最前端
                    pthread_mutex_unlock(&list_mutex );
//                    std::cout << "push unit to hikvs_face_list front, size():" << hikvs_face_list.size() << endl;
                }
//                printf_info( "Picture face ID: %d, Score: %d\n", struFaceSnap.dwFacePicID, struFaceSnap.dwFaceScore );
            }

#if 0
//            printf("人脸抓拍报警[0x%x]: Abs[%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d] Dev[ip:%s,port:%d,ivmsChan:%d] face ID: %d, Score: %d \n",
//                lCommand, struAbsTime.dwYear, struAbsTime.dwMonth, struAbsTime.dwDay, struAbsTime.dwHour,
//                struAbsTime.dwMinute, struAbsTime.dwSecond, struFaceSnap.struDevInfo.struDevIP.sIpV4,
//                struFaceSnap.struDevInfo.wPort, struFaceSnap.struDevInfo.byIvmsChannel,
//                struFaceSnap.dwFacePicID, struFaceSnap.dwFaceScore);
#endif
    }
            break;
        default:
            printf_err("其他报警，报警信息类型: 0x%x\n", lCommand);
            break;
    }

    return TRUE;
}

void save_imagedata2pic()
{
    if( hikvs_face_list.empty() )
		return;
    if( hikvs_face_list.size() >= 1 ){
        hik_unit last_unit  = hikvs_face_list.back();					//取最末端元素
        if( last_unit.is_read ){
            std::string today_time = get_today_timestr();
            check_and_mkdir_dir(today_time);
            face_pic_name.assign("");
            face_pic_name = today_time + "/" + last_unit.img_name;
            bg_pic_name.assign("");
            bg_pic_name =today_time + "/" + last_unit.whole_img_name;

            int fd1 = open( face_pic_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, FILE_MODE );
            int fd2 = open( bg_pic_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, FILE_MODE );
            if( fd1 < 0 || fd2 < 0 ){
                printf_err( "create image *.jpg failed!\n" );
            }else{
                write(fd1, last_unit.pimage, last_unit.img_len );
                write(fd2, last_unit.pwhole_img, last_unit.whole_img_len);
                fsync(fd1);
                fsync(fd2);
                close(fd1);
                close(fd2);
                last_unit.is_write2img = true;
            }

            if( (last_unit.pimage != nullptr) && (last_unit.pwhole_img != nullptr) ){
                free(last_unit.pimage);
                free(last_unit.pwhole_img);
                last_unit.pimage = nullptr;
                last_unit.pwhole_img = nullptr;
            }
//            printf_dbg( "Before pop_back hikvs_face_list size: %d, write:%s %s\n",
//                        (int)hikvs_face_list.size(),last_unit.img_name.c_str(),
//                        last_unit.whole_img_name.c_str() );
            hikvs_face_list.pop_back();        //弹出最末端元素

            name_faceid_unit  temp_nameid_unit;
            temp_nameid_unit.img_name = face_pic_name;
            temp_nameid_unit.face_number = last_unit.face_num;
            temp_nameid_unit.face_score = last_unit.face_score;
            temp_nameid_unit.in_out = last_unit.in_out;
            faceid_name_deque.push_front( temp_nameid_unit );
        }
    }
}

void hik_deque_free()
{
    std::list<hik_unit>::iterator    it;
   for(it = hikvs_face_list.begin(); it != hikvs_face_list.end(); ) {
        if( it->pimage != NULL ){
            free( it->pimage );
            it->pimage = NULL;
        }
        if( it->pwhole_img != NULL ){
            free(it->pwhole_img);
            it->pwhole_img = NULL;
        }
        it++;
    }
}

void deal_imgs_same_faceid( face_idx_score  &one_id, std::string &img_name )
{
    cv::Mat tt;
    tt = imread( img_name, IMREAD_COLOR);
    if( !tt.empty() ){
    	one_id = fdb_ins.get_faceID_score(tt);
    }else{
        one_id.name_index = -1;
        one_id.name.clear();
    	printf_err( "imread img_name errorxxxxxxxxxxxxxxxxxxxxx\n" );
    }
}

void tmp_record_assign_value(db_swipe_info &db_info,swipe_info_str &swipe_info)
{
    swipe_info.card_code = db_info.card_num;
    swipe_info.swipe_time = db_info.time;
    swipe_info.in_out = db_info.in_out;
}
void parse_js()
{
    ifstream ifs("config.js");
    if( !ifs.is_open() ){
        printf_err( "Open config.js error, please put it with app same path\n" );
        exit(-1);
    }
    Json::Reader reader;
    Json::Value obj;
    reader.parse(ifs, obj); // reader can also read strings
    facelib_path = obj["src_pics_path"].asString();

    mysql_host = obj["mysql_host"].asString();
    mysql_user = obj["mysql_user"].asString();
    mysql_password = obj["mysql_passwd"].asString();
    db_name = obj["db_name"].asString();

    appid = obj["appid"].asString();
    sdkkey = obj["sdkkey"].asString();
    face_ensure_threshold = obj["face_ensure_threshold"].asUInt();
    pic_show_threshold = obj["pic_show_threshold"].asUInt();

    server_info.server_ip = obj["server_ip"].asString();    //"http://192.168.5.58:8090";
    server_info.dev_id = obj["dev_id"].asString();           //"371088881111";
    server_info.sch_name = obj["school_name"].asString();         //"郑州家音顺达通讯有限公司";

    server_files.swipe = "swipe.htm";
    server_files.heart = "terminal.htm";
    server_files.school_msg = "school.htm";
    server_files.grade_msg = "grade.htm";
    server_files.class_msg = "class.htm";
    server_files.student_msg = "students.htm";
    server_files.student_increase_msg = "studentIncrement.htm";
    server_files.student_global_msg = "allStudent.htm";
    server_files.holiday = "rest.htm";
    server_files.attence = "attendanceRule.htm";
    server_files.picture_increment = "studentPhotoIncrement.htm";
    server_files.student_photo = "studentPhoto.htm";
    server_files.unswiped_card_list.clear();

    if( facelib_path.empty() || mysql_host.empty() || mysql_user.empty() || mysql_password.empty()
            || db_name.empty() || appid.empty() || sdkkey.empty() ||
            server_info.server_ip.empty() || server_info.dev_id.empty() || server_info.sch_name.empty()
            ){
        std::cout << "Check config.js file, some args empty" << std::endl;
        exit(1);
    }
    const Json::Value& hk_cameras = obj["hk_cameras"]; // array of characters
    for (unsigned int i = 0; i < hk_cameras.size(); i++){
       struct hk_args hk_args;
       hkdev_inout  dev_inout;
       hk_args.asynlogin = hk_cameras[i]["asynlogin"].asUInt();
       hk_args.ip = hk_cameras[i]["ip"].asString();
       hk_args.port = hk_cameras[i]["port"].asUInt();
       hk_args.user = hk_cameras[i]["user"].asString();
       hk_args.passwd = hk_cameras[i]["passwd"].asString();
       hk_args.hklog = hk_cameras[i]["hklog"].asString();
       hk_args.in_out = hk_cameras[i]["in_out"].asUInt();
       hk_args_vec.push_back( hk_args );

       dev_inout.hk_ip = hk_cameras[i]["ip"].asString();
       dev_inout.in_out = hk_cameras[i]["in_out"].asUInt();
       inout_vec.push_back(dev_inout);
    }
    if(hk_args_vec.empty() ){
        printf_err("No camera args set in config.js\n");
        exit(1);
    }
}

void set_local_unicode()
{
    setlocale(LC_CTYPE, "");
    setlocale(LC_NUMERIC, "");

    printf("%s\n", nl_langinfo(CODESET));
    printf("%s\n", nl_langinfo(RADIXCHAR));

    if( !strcmp(nl_langinfo(CODESET), "UTF-8") ){
        std::cout << " UTF8 mode....................." << std::endl;
    }
}

void mysql_error_to_exit()
{
    mysql_db_die( pface_sql );
    joinus_school.http_uninit();
    exit(-1);
}
void arcsoft_error_to_exit()
{
    mysql_db_die( pface_sql );
    joinus_school.http_uninit();
    fdb_ins.engine_uninit();
    exit(-1);
}

bool update_student_message_face_feature_everyday()
{
    if( !joinus_school.post_grade_message()
            || !joinus_school.post_class_message()){
        printf_err( "httpclient error............\n" );
        return false;
    }

    //transfer_cache_deque 存储了全部在校学生的信息(学生id,班级，性别，卡号等)
    //src_stu_num_vec 存储了全部在校学生的信息stu_id
    joinus_school.post_students_message(transfer_cache_deque, src_stu_num_vec);
    if( !mysql_db_create_cachetable_from_deque(pface_sql, transfer_cache_deque) ){
        printf_err( "create cache_table from deque failed\n" );
        return false;
    }
    printf_info( "update cache_table from transfer_cache_deque success\n" );

    //global_pics_deque  存储了有照片学生的信息(stu_id,full_photo_name,photo_url)
    //nopics_stuid_vec   存储了没有照片学生的stu_id
    std::vector<std::string>  nopics_stuid_vec;
    ClearVector(nopics_stuid_vec);
    joinus_school.post_all_pictures(facelib_path,
                                    src_stu_num_vec,
                                    global_pics_deque,
                                    nopics_stuid_vec );
    joinus_school.http_download_picture(global_pics_deque);
    std::string today_time = get_today_timestr();
    check_and_mkdir_dir(today_time);
    std::string nopic_file = today_time  +"/a_no_picture_students.txt";
    printf_dbg( "Get pictures: %d\n", (int)global_pics_deque.size());
    write_stu_vec_feature2file( transfer_cache_deque,
                                  nopics_stuid_vec,
                                  nopic_file);

    //feature_err_vec  存储了特征点提取失败的学生stu_id
    std::vector<std::string>  feature_err_vec;
    ClearVector(feature_err_vec);
    int ret = fdb_ins.build_feature_list( global_pics_deque, feature_err_vec );
    if( 0 != ret ){
       return false;
    }
    std::string feature_err_file = today_time + "/a_extract_feature_err_info.txt";
    write_stu_vec_feature2file(transfer_cache_deque, feature_err_vec, feature_err_file);
    return true;
}
