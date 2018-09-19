# DeepEfficiency
https://arxiv.org/abs/1809.06101

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

<br>
Requirements: Python3 & Tensorflow 1.8+ & ROOT libraries

<br>

## Get ascii (.csv) out from ROOT trees
root printascii.c+ -b -q

## Train DeepEfficiency networks
train.sh

## Obtain efficiency inversion estimates
predict.sh

## Plot differential distributions
make && ./deeplot

<br>
This is a research level proof-of-principle code, depending on the application, additional algorithms, estimators and regularization techniques may be needed.
<br>
mikael.mieskolainen@cern.ch, 2018
