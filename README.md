# DeepEfficiency
optimal efficiency inversion in higher dimensions at the LHC

<br>
Tested with Python3 + Tensorflow 1.8 + ROOT libraries

<br>
<br>

## Get ascii (.csv) out from ROOT trees
root printascii.c+ -b -q

## Train the networks
train.sh

## Obtain efficiency correction values
predict.sh

## Plot differential distributions
make && ./deeplot

<br>
For more information: mikael.mieskolainen@cern.ch
