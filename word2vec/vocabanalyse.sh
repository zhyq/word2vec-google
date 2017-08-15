#!/usr/bin/env sh
datafile="/home/oicq/multimedia/hanleyzhang/sparktextprocess/data/segout/FC_20160724_wash_segout_trim"
datavocab="/home/oicq/multimedia/hanleyzhang/sparktextprocess/data/vocab/FC_20160724_wash_segout_trim_vocab"
datavocabspark="/home/oicq/multimedia/hanleyzhang/sparktextprocess/data/vocab/FC_20160724_wash_segout_trim_vocab_spark"
#./vocabanalyse -train $datafile -save-vocab $datavocab -min-count 30
mincount=30
echo $mincount
awk -F" "  'BEGIN{ORS=" "} {if($2 >= 30){print $1} }' $datavocab > $datavocabspark


