make
if [ ! -e text8 ]; then
  wget http://mattmahoney.net/dc/text8.zip -O text8.gz
  gzip -d text8.gz -f
fi
time ./word2vec -train xaa -output xaa_vectors.bin -cbow 1 -size 200 -window 8 -negative 25 -hs 0 -sample 1e-5 -threads 20 -binary 0 -iter 15
#./distance vectors.bin
