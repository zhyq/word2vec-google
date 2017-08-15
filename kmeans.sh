#!/usr/bin/env sh
input=$1
class=$2

#./kmeans -input /data1/hanleyzhang/word2vec/wxp_20170407/w2v -output /data1/hanleyzhang/word2vec/wxp_20170407/w2v_kmeans_out_class"_$class" -kmeans-center /data1/hanleyzhang/word2vec/wxp_20170407/w2v_kmeans_center_class"_$class" -classes $class
./kmeans -input $input -output $input"_kmeans_out_class_$class" -kmeans-center $input"_kmeans_center_class_$class" -classes $class

