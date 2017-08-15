/*
 * desc:计算两个字之间的距离
 *
 * */
#ifndef __WORDDISTANCE_H__
#define __WORDDISTANCE_H__

#ifdef __cplusplus
extern "C"{
#endif

/* 
 * desc:加载词向量数据,程序运行时需要加载一次，必须在调用distance之前
 * param char * path:词向量文件都在的路径
 * return:void
 * */
public void loadwords(char * path);

/**
 * desc:计算两个字之间的距离
 * param1 char * w1:第一个字 
 * param2 char * w2:第二个字
 * param3 float & distance:得到距离
 * return int 0计算成功 1字不存在 2 未知错误 
 */
public int two_word_distance(char * w1,char *w2,float distance);





#ifdef __cplusplus
}
#endif /* cplusplus*/

#endif /* __WORDDISTANCE_H__*/
