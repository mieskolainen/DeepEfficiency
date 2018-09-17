# DeepEfficiency
DeepEfficiency - optimal efficiency inversion in higher dimensions at the LHC

Fast instructions
Tested with Python3 + Tensorflow 1.8 + ROOT libraries

# Get ascii (.csv) out from ROOT trees
root printascii.c+ -b -q

# Train the networks
train.sh

# Obtain efficiency correction values
predict.sh

# Plot differential distributions
make && ./deeplot

mikael.mieskolainen@cern.ch, 26/07/2018
