echo ---------------------------------------------------------------------------------------------------
echo Note that for the word analogy to perform well, the model should be trained on much larger data set
echo Example input: paris france berlin
echo ---------------------------------------------------------------------------------------------------
time ./word2vec -train tencentnews_out  -output vectors_600.bin -cbow 0 -size 600 -window 8 -negative 25 -hs 0 -sample 1e-5 -threads 40 -binary 0 -iter 15
./word-analogy vectors.bin
